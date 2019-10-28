#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IntSetting(set) (*((int*)settings[(set)].data))
#define StringSetting(set) ((char*)settings[(set)].data)
#define DoubleSetting(set) (*((double*)settings[(set)].data))


typedef struct {
	char type; // 0 = int, 1 = string, 2 = double
	char *name;
	void *data;
	bool def;
} SettingsEntry;

static char *configpath = "/OSExt_config.conf.tns";

int settingssize = 0;
SettingsEntry *settings = NULL;
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
		return;
	}
	settingssize = length;
	settings = calloc(length,sizeof(SettingsEntry));
	for (int i = 0;i<length;i++)
	{
		char type = 0;
		matches = fscanf(settingsf,"%c ",&type);
		if (matches == 0)
		{
			free(settings);
			settings = NULL;
			fclose(settingsf);
			defaultSettings();
			return;
		}
		if (type > 2)
		{
			free(settings);
			settings = NULL;
			fclose(settingsf);
			defaultSettings();
			return;
		}
		char *name = calloc(11,sizeof(char));
		matches = fscanf(settingsf,"%10s ",name);
		if (matches == 0)
		{
			free(settings);
			settings = NULL;
			fclose(settingsf);
			defaultSettings();
			return;
		}
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
		return;
	}
	fprintf(settingsf,"length: %d\n",settingssize);
	for (int i = 0;i<settingssize;i++)
	{
		fprintf(settingsf,"%c ",settings[i].type);
		fprintf(settingsf,"%10s ",settings[i].name);
		if (settings[i].type == 0)
		{
			fprintf(settingsf,"%d\n",*((int*)settings[i].data));
		}
		if (settings[i].type == 1)
		{
			fprintf(settingsf,"%30s\n",((char*)settings[i].data));
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
	int length = 0;
	#ifdef MODULE_DESKTOP
	length += 3;
	#endif
	#ifdef MODULE_DISABLENAVNET
	length += 2;
	#endif
	
	
	
	settingssize = length;
	settings = calloc(length,sizeof(SettingsEntry));
	int index = 0;
	
	
	#ifdef MODULE_DESKTOP
	settings[index].type = 0;
	settings[index].name = "r";
	settings[index].def = true;
	settings[index].data = (void*) malloc(sizeof(int));
	*((int*)settings[index].data) = 0;
	index++;
	settings[index].type = 0;
	settings[index].name = "g";
	settings[index].def = true;
	settings[index].data = (void*) malloc(sizeof(int));
	*((int*)settings[index].data) = 255;
	index++;
	settings[index].type = 0;
	settings[index].name = "b";
	settings[index].def = true;
	settings[index].data = (void*) malloc(sizeof(int));
	*((int*)settings[index].data) = 255;
	index++;
	#endif
	#ifdef MODULE_DISABLENAVNET
	settings[index].type = 0;
	settings[index].name = "navnet";
	settings[index].def = true;
	settings[index].data = (void*) malloc(sizeof(int));
	*((int*)settings[index].data) = 0;
	index++;
	settings[index].type = 0;
	settings[index].name = "navnet_next";
	settings[index].def = true;
	settings[index].data = (void*) malloc(sizeof(int));
	*((int*)settings[index].data) = 0;
	index++;
	#endif
	
	
	
	
	
	
	
	
	
	
	
	
}



#endif