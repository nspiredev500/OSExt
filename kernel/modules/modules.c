#include "../kernel.h"

/*
	To initialize the module system, we load the symbol table and string table of OSExt into memory.
	Whenever a module searches for a function, the symbol table gets searched for a function with that name, and it's address is returned.
	
	
	The entry point of modules is a module_init function.
	The argument they take is a pointer to the search function.
	This enables modules to find all other function in the kernel.
	
	
	modules also have to provide a module_end function, that is called when the kernel uninstalls this module.
	A pointer to module_end should be returned by module_start.
	
*/

//static const char* symtab_path = "/documents/symtab.tns";
//static const char* strtab_path = "/documents/strtab.tns";
static const char* modules_path = "/documents/";

#define EXPORTED_NUM 3
static void* modules_export[EXPORTED_NUM];
static const char* modules_export_names[EXPORTED_NUM];


//static char* osext_strtab = NULL;
//static void* osext_symtab = NULL;
//static uint32_t strtab_pages = 0;
//static uint32_t symtab_pages = 0;

struct module *modules = NULL;


void module_system_init()
{
	modules_export[0] = kmalloc;
	modules_export[1] = kfree;
	modules_export[2] = uart_printf;
	
	modules_export_names[0] = "kmalloc";
	modules_export_names[1] = "kfree";
	modules_export_names[2] = "uart_printf";
}

// only works in kernel space
void module_uninstall(const char *name)
{
	struct module *modprev = NULL;
	struct module *mod = modules;
	while (mod != NULL)
	{
		if (k_strlen(name,100) == k_strlen(mod->name,100) && k_strcmp(name,mod->name,100) == 0)
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
	
	
	void* (*module_entry)(void*) = entry;
	newmod->module_end = NULL;
	newmod->module_end = module_entry(&module_search_function);
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



void* module_search_function(const char *name)
{
	/*
	if (osext_strtab == NULL || osext_symtab == NULL)
	{
		return NULL;
	}
	struct elf_symtab_entry sym;
	// (index+1)*sizeof(struct elf_symtab_entry), because +sizeof(struct elf_symtab_entry) because we will access the structure at that point
	for (uint32_t index = 0;(index+1)*sizeof(struct elf_symtab_entry) < symtab_pages*SMALL_PAGE_SIZE;index++)
	{
		k_memcpy(&sym,osext_symtab+index*sizeof(struct elf_symtab_entry),sizeof(struct elf_symtab_entry));
		if ((sym.info & 0xf) == 2 && (sym.info >> 4) == 1) // function and global
		{
			if (sym.name <= strtab_pages*SMALL_PAGE_SIZE)
			{
				uint32_t maxlen = strtab_pages*SMALL_PAGE_SIZE - sym.name;
				if (k_strlen(osext_strtab+sym.name,maxlen) == k_strlen(name,100) && k_strcmp(osext_strtab+sym.name,name,100) == 0) // is the name right?
				{
					uint32_t kernel_size = (uint32_t) (&_EXEC_SIZE-&_EXEC_START);
					if (sym.value > kernel_size)
					{
						return NULL;
					}
					return (&_TEXT_START)+sym.value;
				}
			}
		}
	}
	return NULL;
	*/
	DEBUGPRINTLN_1("requested function: %s",name);
	for (uint32_t i = 0;i<sizeof(modules_export)/4;i++)
	{
		DEBUGPRINTLN_1("index: %d, name: %s",i,modules_export_names[i]);
		if (k_strlen(modules_export_names[i],100) == k_strlen(name,100) && k_strcmp(name,modules_export_names[i],100) == 0)
		{
			DEBUGPRINTLN_1("referenced found: 0x%x",modules_export[i]);
			return modules_export[i];
		}
	}
	return NULL;
}


































