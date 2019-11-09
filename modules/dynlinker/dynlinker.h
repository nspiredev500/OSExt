#ifndef DYNLINKER_H
#define DYNLINKER_H

#define LIBPATH_OVERRIDE


static char *stdlibpath = "/documents/ndless/";
static int syscall_offset = 5*20+10; // module id*20+10


void addLibraryEntry(char *name, void *functiontable)
{
	if (libs == NULL)
	{
		libs = malloc(sizeof(Library));
		libs->name = malloc(strlen(name)+1);
		memcpy(libs->name,name,strlen(name)+1);
		libs->functiontable = functiontable;
		libs->next = NULL;
	}
	Library *libp = libs;
	do
	{
		if (strcmp(name,libp->name) == 0)
		{
			return;
		}
		libp = libp->next;
	} while (libp->next != NULL);
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
	
	
}


Library* searchLibrary(char * name)
{
	if (libs == NULL)
	{
		return NULL;
	}
	Library *libp = libs;
	do
	{
		if (strcmp(name,libp->name) == 0)
		{
			return libp;
		}
		libp = libp->next;
	} while (libp->next != NULL);
	return NULL;
}




bool loadLibrary(char *name)
{
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
		nl_exec(s,0,NULL);
		free(s);
		return true;
	}
	nl_exec(s,0,NULL);
	free(s);
	return true;
}

bool s_addLibrary(char *name,void *function_table)
{
	bkpt();
	if (searchLibrary(name) == NULL)
	{
		addLibraryEntry(name,function_table);
		return true;
	}
	return false; // should not call nl_set_resident() when false is returned to save ram
}


void* s_requestLibrary(char *name)
{
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
		return l->functiontable;
	}
}













#endif