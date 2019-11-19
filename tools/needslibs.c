#include <stdio.h>
#include <stdlib.h>
const char* libstring = "Libs:";
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage: needslibs [the .tns file] [lib1] [lib2] [lib3] [...]\n");
		return 0;
	}
	int libs = argc-2;
	FILE* f = fopen(argv[1],"r");
	if (f == NULL)
	{
		printf("could not open %s\n",argv[1]);
		return 1;
	}
	fseek(f,0,SEEK_END);
	int size = ftell(f);
	rewind(f);
	char *file = malloc(size);
	fread(file,1,size,f);
	fclose(f);
	f = fopen(argv[1],"w");
	if (f == NULL)
	{
		printf("could not write to %s\n",argv[1]);
		return 1;
	}
	fprintf(f,"%s%c",libstring,libs);
	for (int i = 2;i<argc;i++)
	{
		printf("writing %s\n",argv[i]);
		fprintf(f,"%s",argv[i]);
		fputc('\0',f);
	}
	fwrite(file,1,size,f);
	fclose(f);
	free(file);
	return 0;
}