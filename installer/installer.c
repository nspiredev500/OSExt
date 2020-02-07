#include <stdlib.h>
#include <string.h>
#include <os.h>
#include <libndls.h>
#include <dirent.h>



bool copy_folder(char *source,char *dest);

int main(int argsn, char **argc)
{
	if (argsn == 0)
		return 0;
	
	
	if (strlen(argc[0]) >= 80)
	{
		show_msgbox("OSExt Installer","execution path too long, maybe put the folder directly into documents");
		return 0;
	}
	//printf("%s \n",argc[0]);
	char path[100];
	memset(path,'\0',90);
	{
		char *c = argc[0]+strlen(argc[0]);
		while (*c != '/')
		{
			c--;
		}
		c++;
		size_t size = (size_t) (c-argc[0]);
		memcpy(path,argc[0],size); // get the path up to the first / from the right
	}
	printf("%s \n",path);
	
	
	// deleting folders only works if the folder is empty
	// mkdir("/documents/test",0777);
	// rmdir("/documents/test");
	// unlink("/documents/test");
	
	
	
	
	
	// make a small linux filesystem hierarchy
	mkdir("/etc",0777);
	mkdir("/boot",0777);
	// /dev and /proc will be caught in the syscalls so they appear to be filled with the right files
	mkdir("/usr",0777);
	mkdir("/usr/include",0777);
	mkdir("/usr/lib",0777);
	mkdir("/usr/bin",0777);
	
	char buff[200];
	
	
	memset(buff,'\0',195);
	strcat(buff,path);
	strcat(buff,"etc");
	copy_folder(buff,"/etc");
	
	
	memset(buff,'\0',195);
	strcat(buff,path);
	strcat(buff,"boot");
	copy_folder(buff,"/boot");
	
	
	memset(buff,'\0',195);
	strcat(buff,path);
	strcat(buff,"usr");
	copy_folder(buff,"/usr");
	
	
	
	refresh_osscr();
	return 0;
}

bool copy_folder(char *source,char *dest)
{
	printf("source: %s \n",source);
	printf("destination: %s \n",dest);
	DIR *d = opendir(source);
	if (d == NULL)
	{
		return false;
	}
	struct dirent *file;
	while ((file = readdir(d)) != NULL)
	{
		if (strcmp(file->d_name,".") == 0)
			continue;
		if (strcmp(file->d_name,"..") == 0)
			continue;
		char dest_path[100];
		memset(dest_path,'\0',98);
		if (strlen(dest)+strlen(file->d_name) > 95)
			return false;
		strcat(dest_path,dest);
		strcat(dest_path,"/");
		strcat(dest_path,file->d_name);
		
		
		char source_path[100];
		memset(source_path,'\0',98);
		if (strlen(source)+strlen(file->d_name) > 95)
			return false;
		strcat(source_path,source);
		strcat(source_path,"/");
		strcat(source_path,file->d_name);
		
		
		if (! copy_folder(source_path,dest_path))
		{
			printf("copying: %s \n",file->d_name);
			FILE *f = fopen(source_path,"rb");
			if (f == NULL)
			{
				continue;
			}
			FILE *f2 = fopen(dest_path,"wb");
			if (f2 == NULL)
			{
				fclose(f);
				continue;
			}
			char buff[1024];
			size_t read;
			while ((read = fread(buff,1,1000,f)) != 0)
			{
				printf("read: %d \n",read);
				fwrite(buff,1,read,f2);
			}
			printf("read: %d \n",read);
			fclose(f);
			fclose(f2);
		}
	}
	closedir(d);
	return true;
}





































