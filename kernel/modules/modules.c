#include "../kernel.h"

/*
	To initialize the module system, we load the symbol table and string table of OSExt into memory.
	Whenever a module searches for a function, the symbol table gets searched for a function with that name, and it's address is returned.
	
	
	The entry point of modules is a module_init function.
	The arguments they take is a pointer to the search function, a uint32_t that describes the version of OSExt that tries to run the module
	and a boolean that tells the module whether the OSExt runs in standalone or normal mode.
	This enables modules to find all other function in the kernel.
	
	The version uint32_t decodes like this:
	The first byte is the mayor version, a module should reject an OSExt that isn't the same mayor version as itself.
	The second byte is the minor version. A bigger minor version than the compiled module should be fine, but a smaller one may not.
	The third byte is the patch version. Another patch version should always be compatible.
	
	Due to the project not being finished yet, the mayor version isn't the first number in the github releases, but the second one: 0.mayor.minor.patch
	
	
	modules also have to provide a module_end function, that is called when the kernel uninstalls this module.
	A pointer to module_end should be returned by module_start.
	
*/


static const char* modules_path = "/documents/";

#define EXPORTED_NUM 44
static void* modules_export[EXPORTED_NUM];
static const char* modules_export_names[EXPORTED_NUM];


static struct module *modules = NULL;


const struct module* module_get_modules()
{
	return modules;
}


void module_system_init()
{
	modules_export[0] = kmalloc;
	modules_export[1] = kfree;
	modules_export[2] = uart_printf;
	modules_export[3] = useConsecutivePages;
	modules_export[4] = freeConsecutivePages;
	modules_export[5] = register_draw_function;
	modules_export[6] = unregister_draw_function;
	modules_export[7] = register_file_function;
	modules_export[8] = unregister_file_function;
	modules_export[9] = framebuffer_setpixel565;
	modules_export[10] = framebuffer_draw_img565;
	modules_export[11] = framebuffer_setpixel;
	modules_export[12] = framebuffer_fillrect;
	modules_export[13] = framebuffer_drawrect;
	modules_export[14] = framebuffer_write10pchar;
	modules_export[15] = framebuffer_write10pdigit;
	modules_export[16] = framebuffer_write10pstring_ascii;
	modules_export[17] = panic;
	modules_export[18] = systime_unix;
	modules_export[19] = systime_unix_milis;
	modules_export[20] = systime_timestamp_to_unix;
	modules_export[21] = systime_unix_to_timestamp;
	modules_export[22] = debug_shell_println;
	modules_export[23] = debug_shell_println_rgb;
	modules_export[24] = isKeyPressed;
	modules_export[25] = keypad_press_release_barrier;
	modules_export[26] = touchpad_get_report;
	modules_export[27] = touchpad_is_arrow;
	modules_export[28] = get_back_framebuffer_address;
	modules_export[29] = blitLCDBuffer;
	modules_export[30] = k_memset;
	modules_export[31] = k_memcpy;
	modules_export[32] = k_strlen;
	modules_export[33] = k_strcmp;
	modules_export[34] = sprintf_safe;
	modules_export[35] = msleep;
	modules_export[36] = systime_set_unix;
	modules_export[37] = rgbto565;
	modules_export[38] = signedtounsigned32;
	modules_export[39] = rtc_get_value;
	modules_export[40] = nuc_fopen;
	modules_export[41] = nuc_fclose;
	modules_export[42] = nuc_fread;
	modules_export[43] = nuc_fwrite;
	
	
	
	
	modules_export_names[0] = "kmalloc";
	modules_export_names[1] = "kfree";
	modules_export_names[2] = "uart_printf";
	modules_export_names[3] = "useConsecutivePages";
	modules_export_names[4] = "freeConsecutivePages";
	modules_export_names[5] = "register_draw_function";
	modules_export_names[6] = "unregister_draw_function";
	modules_export_names[7] = "register_file_function";
	modules_export_names[8] = "unregister_file_function";
	modules_export_names[9] = "framebuffer_setpixel565";
	modules_export_names[10] = "framebuffer_draw_img565";
	modules_export_names[11] = "framebuffer_setpixel";
	modules_export_names[12] = "framebuffer_fillrect";
	modules_export_names[13] = "framebuffer_drawrect";
	modules_export_names[14] = "framebuffer_write10pchar";
	modules_export_names[15] = "framebuffer_write10pdigit";
	modules_export_names[16] = "framebuffer_write10pstring_ascii";
	modules_export_names[17] = "panic";
	modules_export_names[18] = "systime_unix";
	modules_export_names[19] = "systime_unix_milis";
	modules_export_names[20] = "systime_timestamp_to_unix";
	modules_export_names[21] = "systime_unix_to_timestamp";
	modules_export_names[22] = "debug_shell_println";
	modules_export_names[23] = "debug_shell_println_rgb";
	modules_export_names[24] = "isKeyPressed";
	modules_export_names[25] = "keypad_press_release_barrier";
	modules_export_names[26] = "touchpad_get_report";
	modules_export_names[27] = "touchpad_is_arrow";
	modules_export_names[28] = "get_back_framebuffer_address";
	modules_export_names[29] = "blitLCDBuffer";
	modules_export_names[30] = "k_memset";
	modules_export_names[31] = "k_memcpy";
	modules_export_names[32] = "k_strlen";
	modules_export_names[33] = "k_strcmp";
	modules_export_names[34] = "sprintf_safe";
	modules_export_names[35] = "msleep";
	modules_export_names[36] = "systime_set_unix";
	modules_export_names[37] = "rgbto565";
	modules_export_names[38] = "signedtounsigned32";
	modules_export_names[39] = "rtc_get_value";
	modules_export_names[40] = "nuc_fopen";
	modules_export_names[41] = "nuc_fclose";
	modules_export_names[42] = "nuc_fread";
	modules_export_names[43] = "nuc_fwrite";
	
	
	
}

// only works in kernel space
void module_uninstall(const char *name)
{
	struct module *modprev = NULL;
	struct module *mod = modules;
	while (mod != NULL)
	{
		if (k_streq(name,mod->name,100))
		{
			mod->module_end();
			freeConsecutivePages(mod->start,mod->pages);
			if (modprev == NULL)
			{
				modules = mod->next;
			}
			else
			{
				modprev->next = mod->next;
			}
			kfree(mod->name);
			kfree_hint(mod,sizeof(struct module));
			return;
		}
		modprev = mod;
		mod = mod->next;
	}
}

// only works in kernel space
void module_install(const char *name)
{
	uint32_t pathlen = k_strlen(modules_path,100)+k_strlen(name,100)+4+k_strlen(".elf.tns",100);
	char *path = kmalloc(pathlen);
	k_memset(path,'\0',pathlen);
	k_memcpy(path,modules_path,k_strlen(modules_path,100));
	k_memcpy(path+k_strlen(modules_path,100),name,k_strlen(name,100));
	k_memcpy(path+k_strlen(modules_path,100)+k_strlen(name,100),".elf.tns",k_strlen(".elf.tns",100));
	
	DEBUGPRINTLN_1("loading module: %s",path);
	
	
	struct nuc_stat stat;
	if (nuc_stat(path,&stat) != 0)
	{
		DEBUGPRINTLN_1("could not stat");
		kfree(path);
		return;
	}
	NUC_FILE* f = nuc_fopen(path,"rb");
	if (f == NULL)
	{
		DEBUGPRINTLN_1("could not open");
		kfree(path);
		return;
	}
	kfree(path);
	
	struct elf_header h;
	elf_read_header(f,&h);
	if (! elf_check_header(&h))
	{
		DEBUGPRINTLN_1("invalid elf header");
		nuc_fclose(f);
		return;
	}
	
	struct elf_desc *elf = elf_load_file(f,stat.st_size);
	if (elf == NULL)
	{
		DEBUGPRINTLN_1("couldn't load elf");
		nuc_fclose(f);
		return;
	}
	nuc_fclose(f);
	
	elf_alloc_image(elf);
	if (elf->image == NULL)
	{
		DEBUGPRINTLN_1("could alloc image");
		elf_destroy(elf);
		return;
	}
	elf_fix_got(elf);
	elf_assemble_image(elf);
	
	void* entry = elf_entry(elf);
	if (entry == NULL)
	{
		DEBUGPRINTLN_1("couldn't get entry point");
		elf_destroy(elf);
		return;
	}
	struct module *newmod = kmalloc(sizeof(struct module));
	if (newmod == NULL)
	{
		DEBUGPRINTLN_1("couldn't alloc module");
		elf_destroy(elf);
		return;
	}
	
	char *name_copy = kmalloc(k_strlen(name,100)+2);
	if (name_copy == NULL)
	{
		DEBUGPRINTLN_1("couldn't alloc name");
		elf_destroy(elf);
		return;
	}
	k_memset(name_copy,'\0',k_strlen(name,100)+1);
	k_memcpy(name_copy,name,k_strlen(name,100));
	
	
	newmod->name = name_copy; // remember to free the name when uninstalling the module!
	newmod->start = elf->image;
	newmod->pages = elf->image_pages;
	newmod->next = NULL;
	
	
	void* (*module_entry)(void*,uint32_t,bool) = entry;
	newmod->module_end = NULL;
	newmod->module_end = module_entry(&module_search_function,OSEXT_VERSION,false);
	if (newmod->module_end == NULL)
	{
		DEBUGPRINTLN_1("couldn't get module end, uninstalling");
		kfree_hint(newmod,sizeof(struct module));
		kfree_hint(name_copy,k_strlen(name,100)+2);
		elf_destroy(elf);
		return;
	}
	
	// module loaded an initialized, now add it to the list
	
	
	if (modules == NULL)
	{
		modules = newmod;
	}
	else
	{
		newmod->next = modules;
		modules = newmod;
	}
	
	DEBUGPRINTLN_1("module installed");
	
	elf_destroy_without_image(elf);
}


void module_load_all()
{
	NUC_DIR* dir = nuc_opendir(modules_path);
	DEBUGPRINTLN_1("opening directory")
	if (dir != NULL)
	{
		struct nuc_dirent* dirent;
		while ((dirent = nuc_readdir(dir)) != NULL)
		{
			DEBUGPRINTLN_1("entry: %s",dirent->d_name)
			if (k_strlen(dirent->d_name,100) > 8)
			{
				uint32_t pathlen = k_strlen(dirent->d_name,100);
				if (k_strcmp(dirent->d_name+(pathlen-8),".elf.tns",100) == 0)
				{
					DEBUGPRINTLN_1("elf file extension",dirent->d_name)
					char buffer[105];
					k_memset(buffer,'\0',103);
					k_memcpy(buffer,dirent->d_name,pathlen);
					buffer[pathlen-8] = '\0'; // cut the .elf.tns off for module_install
					module_install(buffer);
				}
			}
		}
		nuc_closedir(dir);
	}
}



void* module_search_function(const char *name)
{
	DEBUGPRINTLN_1("requested function: %s",name);
	for (uint32_t i = 0;i<sizeof(modules_export)/4;i++)
	{
		//DEBUGPRINTLN_1("index: %d, name: %s",i,modules_export_names[i]);
		if (k_streq(name,modules_export_names[i],100))
		{
			DEBUGPRINTLN_1("referenced found: 0x%x",modules_export[i]);
			return modules_export[i];
		}
	}
	return NULL;
}





