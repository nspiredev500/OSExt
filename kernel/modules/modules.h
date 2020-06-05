#ifndef MODULES_H
#define MODULES_H


struct module;
struct module {
	char *name; // name of the module file
	void (*module_end)();
	void* start;
	uint32_t pages;
	struct module *next; // modules are arranged in a linked list
};



void module_system_init();

// only works in kernel space
void module_uninstall(const char *name);
void module_install(const char *name);
void module_load_all();



void* module_search_function(const char *name);














#endif