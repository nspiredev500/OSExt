#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IntSetting(set) (*((int*)settings[(set)].data))
#define StringSetting(set) ((char*)settings[(set)].data)
#define DoubleSetting(set) (*((double*)settings[(set)].data))


typedef struct {
	int type; // 0 = int, 1 = string, 2 = double
	char *name;
	void *data;
	bool def;
} SettingsEntry;

static char *configpath = "/OSExt_config.conf.tns";

int settingssize = 0;
SettingsEntry *settings = NULL;
void deleteSettings()
{
	remove(configpath);
}
void changeSetting(char * name,void *data)
{
	if (settings == NULL)
	{
		return;
	}
	for (int i = 0;i<settingssize;i++)
	{
		if (strcmp(name,settings[i].name) == 0)
		{
			if (settings[i].def)
			{
				if (settings[i].type != 1)
				{
					free(settings[i].data);
				}
				settings[i].data = data;
				settings[i].def = false;
			}
			else
			{
				free(settings[i].data);
				settings[i].data = data;
			}
			return;
		}
	}
}
void reloadSettings() // there should be no memory leak if I'm right 
{
	for (int i = 0;i<settingssize;i++)
	{
		if (settings[i].def)
		{
			if (settings[i].type != 1)
			{
				free(settings[i].data);
			}
		}
		else
		{
			free(settings[i].data);
			free(settings[i].name);
		}
	}
	free(settings);
	loadSettings();
}
int getSetting(char *name)
{
	if (settings == NULL)
	{
		return -1;
	}
	for (int i = 0;i<settingssize;i++)
	{
		if (strcmp(name,settings[i].name) == 0)
		{
			return i;
		}
	}
	return -1;
}
void loadSettings()
{
	FILE *settingsf = fopen(configpath,"r");
	if (settingsf == NULL)
	{
		printf("could not open config");
		defaultSettings();
		return;
	}
	int matches;
	int length;
	matches = fscanf(settingsf,"length: %d\n",&length);
	if (matches == 0)
	{
		fclose(settingsf);
		defaultSettings();
		printf("no length found");
		return;
	}
	
	settingssize = length;
	settings = calloc(length,sizeof(SettingsEntry));
	for (int i = 0;i<length;i++)
	{
		int type = 0;
		matches = fscanf(settingsf,"%d ",&type);
		if (matches == 0)
		{
			free(settings);
			settings = NULL;
			fclose(settingsf);
			defaultSettings();
			printf("not enough entries found");
			return;
		}
		if (type > 2)
		{
			free(settings);
			settings = NULL;
			fclose(settingsf);
			defaultSettings();
			printf("invalid type");
			return;
		}
		char *name = calloc(31,sizeof(char));
		matches = fscanf(settingsf,"%30s ",name);
		if (matches == 0)
		{
			free(settings);
			settings = NULL;
			fclose(settingsf);
			defaultSettings();
			printf("no name found");
			return;
		}
		printf("\nname: %s\n",name);
		if (type == 0)
		{
			int *data = calloc(1,sizeof(int));
			matches = fscanf(settingsf,"%d\n",data);
			if (matches == 0)
			{
				free(data);
				free(settings);
				settings = NULL;
				fclose(settingsf);
				defaultSettings();
				printf("no int data found: %d",i);
				return;
			}
			settings[i].name = name;
			settings[i].type = type;
			settings[i].data = data;
		}
		if (type == 1)
		{
			char *data = calloc(31,sizeof(char));
			matches = fscanf(settingsf,"%30s\n",data);
			if (matches == 0)
			{
				free(data);
				free(settings);
				settings = NULL;
				fclose(settingsf);
				defaultSettings();
				printf("no string data found");
				return;
			}
			settings[i].name = name;
			settings[i].type = type;
			settings[i].data = data;
		}
		if (type == 2)
		{
			double *data = calloc(1,sizeof(double));
			matches = fscanf(settingsf,"%lf\n",data);
			if (matches == 0)
			{
				free(data);
				free(settings);
				settings = NULL;
				fclose(settingsf);
				defaultSettings();
				printf("no float data found");
				return;
			}
			settings[i].name = name;
			settings[i].type = type;
			settings[i].data = data;
		}
		settings[i].def = false;
		
	}
	fclose(settingsf);
}
void saveSettings()
{
	FILE *settingsf = fopen(configpath,"w");
	if (settingsf == NULL)
	{
		printf("could not write the config");
		return;
	}
	
	fprintf(settingsf,"length: %d\n",settingssize);
	for (int i = 0;i<settingssize;i++)
	{
		fprintf(settingsf,"%d ",settings[i].type);
		fprintf(settingsf,"%s ",settings[i].name);
		if (settings[i].type == 0)
		{
			fprintf(settingsf,"%d\n",*((int*)settings[i].data));
		}
		if (settings[i].type == 1)
		{
			fprintf(settingsf,"%s\n",((char*)settings[i].data));
		}
		if (settings[i].type == 2)
		{
			fprintf(settingsf,"%lf\n",*((double*)settings[i].data));
		}
		
	}
	fclose(settingsf);
	
}
void defaultSettings()
{
	printf("could not read the config");
	int length = 0;
	#ifdef MODULE_DESKTOP
		length += 3;
	#endif
	#ifdef MODULE_DISABLENAVNET
		length += 2;
	#endif
	#ifdef MODULE_SECURITY
		length += 2;
	#endif
	#ifdef MODULE_DYNLINKER
		length += 1;
	#endif
	#ifdef MODULE_CURSOR
		length += 3;
	#endif
	
	settingssize = length;
	settings = calloc(length,sizeof(SettingsEntry));
	int index = 0;
	
	
	#ifdef MODULE_DESKTOP
		settings[index].type = 0;
		settings[index].name = "r";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 102;
		index++;
		settings[index].type = 0;
		settings[index].name = "g";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 205;
		index++;
		settings[index].type = 0;
		settings[index].name = "b";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 170;
		index++;
	#endif
	#ifdef MODULE_DISABLENAVNET
		settings[index].type = 0;
		settings[index].name = "navnet";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 1;
		index++;
		settings[index].type = 0;
		settings[index].name = "navnet_next";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 0;
		index++;
	#endif
	#ifdef MODULE_SECURITY
		settings[index].type = 1;
		settings[index].name = "password";
		settings[index].def = true;
		settings[index].data = "";
		index++;
		settings[index].type = 0;
		settings[index].name = "locktime";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 120;
		index++;
	#endif
	#ifdef MODULE_DYNLINKER
		settings[index].type = 1;
		settings[index].name = "libpath";
		settings[index].def = true;
		settings[index].data = "/documents/native libs";
		index++;
	#endif
	#ifdef MODULE_CURSOR
		settings[index].type = 0;
		settings[index].name = "cursorx";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 25;
		index++;
		settings[index].type = 0;
		settings[index].name = "cursory";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 25;
		index++;
		settings[index].type = 0;
		settings[index].name = "lastpress";
		settings[index].def = true;
		settings[index].data = (void*) malloc(sizeof(int));
		*((int*)settings[index].data) = 10;
		index++;
	#endif
	
	
	
	
	
}



#endif