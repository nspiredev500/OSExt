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

static const char* symtab_path = "/documents/modules/symtab.elf.tns";
static const char* modules_path = "/documents/modules";



static char* osext_strtab = NULL;
static void* osext_symtab = NULL;
static uint32_t strtab_pages = 0;
static uint32_t symtab_pages = 0;

struct module *modules = NULL;


void module_system_init()
{
	struct nuc_stat stat;
	if (nuc_stat(symtab_path,&stat) != 0)
	{
		return;
	}
	NUC_FILE* f = nuc_fopen(symtab_path,"rb");
	if (f == NULL)
	{
		return;
	}
	struct elf_header h;
	elf_read_header(f,&h);
	if (! elf_check_header(&h))
	{
		nuc_fclose(f);
		return;
	}
	
	struct elf_desc elf = elf_load_file(f,stat.st_size);
	if (elf_desc == NULL)
	{
		nuc_fclose(f);
		return;
	}
	nuc_fclose(f);
	
	osext_strtab = elf_copy_section(elf,".strtab",&strtab_pages);
	if (osext_strtab == NULL)
	{
		elf_destroy(elf);
		return;
	}
	osext_symtab = elf_copy_section(elf,".symtab",&symtab_pages);
	if (osext_strtab == NULL)
	{
		elf_destroy(elf);
		return;
	}
	elf_destroy(elf);
	
	
	
	
	
}

// only works in kernel space
void module_uninstall(const char *name)
{
	struct module *modprev = NULL;
	struct module *mod = modules;
	while (mod != NULL)
	{
		if (k_strlen(name,100) == k_strlen(mod->name,100) && k_strcmp(name,mod->name) == 0)
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
	struct nuc_stat stat;
	if (nuc_stat(name,&stat) != 0)
	{
		return;
	}
	NUC_FILE* f = nuc_fopen(name,"rb");
	if (f == NULL)
	{
		return;
	}
	struct elf_header h;
	elf_read_header(f,&h);
	if (! elf_check_header(&h))
	{
		nuc_fclose(f);
		return;
	}
	
	struct elf_desc elf = elf_load_file(f,stat.st_size);
	if (elf_desc == NULL)
	{
		nuc_fclose(f);
		return;
	}
	nuc_fclose(f);
	
	elf_alloc_image(elf);
	if (elf->image == NULL)
	{
		elf_destroy(elf);
		return;
	}
	elf_fix_got(elf);
	elf_assemble_image(elf);
	
	void* entry = elf_entry(elf);
	if (entry == NULL)
	{
		elf_destroy(elf);
		return;
	}
	struct module *newmod = kmalloc(sizeof(struct module));
	if (newmod == NULL)
	{
		elf_destroy(elf);
		return;
	}
	
	char *name_copy = kmalloc(k_strlen(name,100)+2);
	if (name_copy == NULL)
	{
		elf_destroy(elf);
		return;
	}
	k_memset(name_copy,'\0',k_strlen(name,100)+1);
	k_memcpy(name_copy,name,k_strlen(name,100));
	
	
	newmod->name = name_copy; // remember to free the name when uninstalling the module!
	newmod->start = elf.image;
	newmod->pages = elf.image_pages;
	newmod->next = NULL;
	
	
	(void (*)()) (*module_entry)(void* (*)()) = entry;
	newmod->module_end = NULL;
	newmod->module_end = module_entry(&module_search_function);
	if (newmod->module_end == NULL)
	{
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
	
	
	
	elf_destroy_without_image(elf);
}



void* module_search_function(const char *name)
{
	
	
	
	return NULL;
}


































