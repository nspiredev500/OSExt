#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>





int main(int argc, char** argv)
{
	
	printf("\n---  OSExt configuration  ---\n");
	
	printf("do you want do overwrite the current config file? [Y/n]\n");
	if (fgetc(stdin) != 'Y')
	{
		printf("aborting\n");
		return 0;
	}
	
	
	FILE* f = fopen("../config.h","w");
	if (f == NULL)
	{
		printf("could not create or open the config file!\n");
		return -1;
	}
	
	FILE* optsf = fopen("options.txt","r");
	if (optsf == NULL)
	{
		fclose(f);
		printf("could not open options.txt!\n");
		return -1;
	}
	
	fprintf(f,"#ifndef __CONFIG_H\n#define __CONFIG_H\n");
	
	
	
	char buffer[100];
	memset(buffer,'\0',100);
	char c;
	while (fscanf(optsf,"%99s %c",buffer,&c) == 2)
	{
		if (c == 'b') // boolean value
		{
			printf("%s (true/false): ",buffer);
			char buffer2[100];
			memset(buffer2,'\0',100);
			
			if (scanf("%s",buffer2) != 1)
			{
				printf("defaulting to false\n");
			}
			if ((strlen(buffer2) != strlen("false") || strcmp("false",buffer2) != 0) && (strlen(buffer2) != strlen("true") || strcmp("true",buffer2) != 0))
			{
				printf("defaulting to false\n");
				strcpy(buffer2,"false");
			}
			while ((getchar()) != '\n');
			fprintf(f,"#define %s %s\n",buffer,buffer2);
			
			memset(buffer,'\0',100);
			continue;
		}
		if (c == 'i') // integer value
		{
			printf("%s (number): ",buffer);
			int i;
			if (scanf("%d",&i) != 1)
			{
				printf("defaulting to 0\n");
				i = 0;
			}
			while ((getchar()) != '\n');
			fprintf(f,"#define %s %d\n",buffer,i);
			
			memset(buffer,'\0',100);
			continue;
		}
		printf("unknown option type\n");
	}
	fprintf(f,"#endif");
	
	printf("\n---  configuration finished  ---\n");
	
	fclose(optsf);
	fclose(f);
	return 0;
}






















