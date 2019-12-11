#ifndef DYNLINKER_H
#define DYNLINKER_H
#include <nucleus.h>
#include "../../zehn_loader.h"
#include "strings.h"




//#define LIBPATH_OVERRIDE


static char *stdlibpath = "/documents/ndless/";
static int syscall_offset = 5*20+10; // module id*20+10
static void* s_get_lib_function_pointers();

void addLibraryEntry(char *name, void *functiontable)
{
	uart_printf("adding entry for: %s with pointer 0x%x\n",name,functiontable);
	if (libs == NULL)
	{
		libs = malloc(sizeof(Library));
		libs->name = malloc(strlen(name)+1);
		memcpy(libs->name,name,strlen(name)+1);
		libs->functiontable = functiontable;
		libs->next = NULL;
	}
	Library *libp = libs;
	/*
	do
	{
		if (strcmp(name,libp->name) == 0)
		{
			return;
		}
		libp = libp->next;
	} while (libp != NULL);
	*/
	while (true)
	{
		if (strcmp(name,libp->name) == 0)
		{
			return;
		}
		if (libp->next == NULL)
			break;
		libp = libp->next;
	}
	libp->next = malloc(sizeof(Library));
	libp = libp->next;
	libp->name = malloc(strlen(name)+1);
	memcpy(libp->name,name,strlen(name)+1);
	libp->functiontable = functiontable;
	libp->next = NULL;
}


void initDynlinker()
{
	setSyscall(syscall_offset,(unsigned int) s_addLibrary);
	setSyscall(syscall_offset+1,(unsigned int) s_requestLibrary);
	setSyscall(syscall_offset+2,(unsigned int) s_get_lib_function_pointers);
	
	
	
	
}



Library* searchLibrary(char * name)
{
	uart_printf("searching %s\n",name);
	if (libs == NULL)
	{
		return NULL;
	}
	Library *libp = libs;
	do
	{
		uart_printf("current in list: %s\n",libp->name);
		if (strcmp(name,libp->name) == 0)
		{
			uart_printf("found: %s\n",libp->name);
			return libp;
		}
		libp = libp->next;
	} while (libp != NULL);
	return NULL;
}




bool loadLibrary(char *name)
{
	uart_printf("loading %s\n",name);
	char *libpathp;
	#ifndef LIBPATH_OVERRIDE
		int libpsetting = getSetting("libpath");
		if (libpsetting == -1)
		{
			libpathp = stdlibpath;
		}
		else
		{
			libpathp = StringSetting(libpsetting);
		}
	#else
		libpathp = stdlibpath;
	#endif
	int length = strlen(libpathp)+strlen(name)+4;
	char *s = malloc(length);
	for (int i = 0;i<length;i++)
	{
		s[i] = '\0';
	}
	
	strcat(s,libpathp);
	strcat(s,name);
	FILE *lfile = fopen(s,"r"); // does fopen even work in a syscall? in this case apparently yes
	if (lfile == NULL)
	{
		free(s);
		length = strlen(libpathp)+strlen(name)+10; //try again with .tns
		s = malloc(length);
		for (int i = 0;i<length;i++)
		{
			s[i] = '\0';
		}
		strcat(s,libpathp);
		strcat(s,name);
		strcat(s,".tns");
		lfile = fopen(s,"r");
		if (lfile == NULL)
		{
			free(s);
			return false;
		}
		fclose(lfile);
		if (fileExtensionTriggered(s) != 0xBEEF)
		{
			nl_exec(s,0,NULL);
		}
		free(s);
		return true;
	}
	fclose(lfile);
	if (fileExtensionTriggered(s) != 0xBEEF)
	{
		nl_exec(s,0,NULL);
	}
	free(s);
	return true;
}




bool s_addLibrary(char *name,void *function_table)
{
	uart_printf("adding %s\n",name);
	if (searchLibrary(name) == NULL)
	{
		addLibraryEntry(name,function_table);
		return true;
	}
	return false; // should not call nl_set_resident() when false is returned to save ram
}


void* s_requestLibrary(char *name)
{
	//bkpt();
	uart_printf("requesting %s\n",name);
	Library *l = searchLibrary(name);
	if (l == NULL)
	{
		if (loadLibrary(name))
		{
			Library *l2 = searchLibrary(name);
			if (l2 == NULL)
			{
				uart_printf("couldn't load library\n");
				return NULL;
			}
			else
			{
				uart_printf("found pointer: 0x%x\n",l2->functiontable);
				return l2->functiontable;
			}
		}
		else
		{
			uart_printf("couldn't load library\n");
			return NULL;
		}
	}
	else
	{
		uart_printf("found pointer: 0x%x\n",l->functiontable);
		return l->functiontable;
	}
}

static const char* lib_signature = "Libs:";




int resolveDependencies(NUC_FILE *file)
{
	uart_printf("resolving dependencies\n");
	char sig[8];
	nuc_fread(sig,1,5,file);
	sig[5] = '\0';
	if (strcmp(lib_signature,sig) != 0)
	{
		uart_printf("no lib signature\n");
		nuc_fseek(file,0,SEEK_SET);
		return 1;
	}
	char strings;
	nuc_fread(&strings,1,1,file);
	//uart_printf("strings: %d\n",strings);
	for (int i = 0;i<strings;i++)
	{
		char str[100];
		int index = 0;
		//uart_printf("string %d: ",i);
		while (true)
		{
			char c;
			nuc_fread(&c,1,1,file);
			//uart_printf("%c",c);
			if (index >= 90)
			{
				break;
			}
			if (c == '\0')
			{
				uart_printf("\n");
				break;
			}
			str[index] = c;
			index++;
		}
		str[index] = '\0';
		index++;
		uart_printf("Dependency: %s\n",str);
		if (searchLibrary(str) == NULL)
		{
			uart_printf("Dependency loaded: %s\n",str);
			loadLibrary(str);
		}
	}
	return 0;
}


int fileExtensionTriggered(char *path) // TODO add support for zehn files embedded in prg programms
{
	char *file = path;
	while (true) // remove the path and leave the filename
	{
		char *s = strchr(file,'/');
		if (s == NULL)
			break;
		file = s+1;
	}
	uart_printf("file: %s\n",file);
	
	char *ext = strchr(file,'.');
	if (ext == NULL) // no extension found
	{
		return 0xDEAD;
	}
	ext = ext+1;
	NUC_FILE *f = nuc_fopen(path,"rb");
	if (f == NULL)
		return 0xDEAD;
	
	
	
	if (strcmp(ext,"lib.tns") == 0) // will be saved as library, without the need of nl_set_resident
	{
		uart_printf(".lib file\n");
		void *base;
		bool hw_w;
		int (*entry)(int,char*[]);
		int ret = resolveDependencies(f);
		if (ret == 1)
		{
			if (zehn_load(f,&base,&entry,&hw_w) == 0)
			{
				nuc_fclose(f);
				entry(0,NULL);
				return 0xBEEF;
			}
			nuc_fclose(f);
			return 0xDEAD;
		}
		if (zehn_load(f,&base,&entry,&hw_w) == 0)
		{
			entry(0,NULL);
		}
		else
		{
			nuc_fclose(f);
			return 0xDEAD;
		}
	}
	/*
	if (strcmp(ext,"mod.tns") == 0) // osext module file for dynamic module loading
	{
		uart_printf(".mod file\n")
		void *base;
		bool hw_w;
		int (*entry)(int,char*[]);
		int ret = resolveDependencies(f);
		if (ret == 1)
		{
			if (zehn_load(f,&base,&entry,&hw_w) == 0)
			{
				nuc_fclose(f);
				entry(0,NULL);
				return 0xBEEF;
			}
			nuc_fclose(f);
			return 0xDEAD;
		}
		if (zehn_load(f,&base,&entry,&hw_w) == 0)
		{
			entry(0,NULL);
		}
		else
		{
			nuc_fclose(f);
			return 0xDEAD;
		}
	}
	*/
	if (strcmp(ext,"libp.tns") == 0) // will be unloaded after executing
	{
		uart_printf(".libp file\n");
		void *base;
		bool hw_w;
		int (*entry)(int,char*[]);
		int ret = resolveDependencies(f);
		if (ret == 1)
		{
			if (zehn_load(f,&base,&entry,&hw_w) == 0)
			{
				nuc_fclose(f);
				entry(0,NULL);
				return 0xBEEF;
			}
			nuc_fclose(f);
			return 0xDEAD;
		}
		if (zehn_load(f,&base,&entry,&hw_w) == 0)
		{
			entry(0,NULL);
		}
		else
		{
			nuc_fclose(f);
			return 0xDEAD;
		}
		free(base);
	}
	
	
	nuc_fclose(f);
	return 0xBEEF;
}



static unsigned int libftable[] = {(unsigned int) s_addLibrary,(unsigned int)  s_requestLibrary};
static void* s_get_lib_function_pointers()
{
	return libftable;
}











#endif