#ifndef _STDIO_H
#define _STDIO_H 1

#include <stddef.h>
#include <stdint.h>
#include <sys/types>
#include <stdarg.h>


#define BUFSIZ 256
#define L_ctermid 50
#define L_tmpnam 50


#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define SEEK_CUR 0
#define SEEK_END 1
#define SEEK_SET 2


#define FILENAME_MAX 100
#define FOPEN_MAX (2^32)
#define TMP_MAX 10000

#define EOF -1



#define p_tmpdir "/tmp/"



typedef uint32_t fpos_t;
typedef struct FILE {
	uint32_t fd;
	fpos_t pos;
	char buffer[BUFSIZ];
};


FILE* stdin = &((FILE){0,0});
FILE* stdout = &((FILE){1,0});
FILE* stderr = &((FILE){2,0});







void clearerr(FILE*);
int ferror(FILE*);
int fflush(FILE*);
void rewind(FILE*);
int feof(FILE*);
int fclose(FILE*);
FILE* fopen(const char*,const char*);
int fprintf(FILE*,const char*,...);
int vfprintf(FILE*, const char*, va_list);
int printf(const char*,...);
int vprintf(const char*, va_list);
int fscanf(FILE*,const char*,...);
int fseek(FILE*,long,int);
long ftell(FILE*);
size_t fwrite(const char*,size_t,size_t,FILE*);
size_t fread(void*,size_t,size_t,FILE*);
int remove(const char*);
int rename(const char*, const char*);

/*
char* ctermid(char*);
int dfprintf(int,const char*,...);
FILE* fdopen(int, const char*);
int fgetc(FILE*);
int fgetpos(FILE*, fpos_t*);
char* fgets(char*,int,FILE*);
int fileno(FILE*);
void flockfile(FILE*);
FILE* fmemopen(void*,size_t,const char*);
int fputc(int,FILE*);
int fputs(const char*, FILE*);
FILE* freopen(const char*, const char*, FILE*);
off_t ftello(FILE*);
int ftrylockfile(FILE*);
void funlockfile(FILE*);
int getc(FILE*);
int getchar(void);
int getc_unlocked(FILE*);
int getchar_unlocked(void);
ssize_t getdelim(char**,size_t*,int,FILE*);
ssize_t getline(char**,size_t*,FILE*);
char* gets(char*);
FILE* open_memstream(char**,size_t*);
int pclose(FILE*);
void perror(const char*);
FILE* popen(const char*, const char*);
int putc(int, FILE*);
int putchar(int);
int putc_unlocked(int,FILE*);
int puts(const char*);
int renameat(int,const char*,int, const char*);


int scanf(const char*,...);
void setbuf(FILE*, char*);
int setvbuf(FILE*,char*,int, size_t);
int snprintf(char*,size_t,const char*,...);
int sprintf(char*,const char*,...);
int sscanf(const char*, const char*,...);


char* tempnam(const char*, const char*);
FILE* tmpfile(void);
char* tmpnam(char*);
int ungetc(int,FILE*);

int vdprintf(int,const char,va_list);
int vfscanf(FILE*, const char*,va_list);
int vscanf(const char*, va_list);
int vsnprintf(const char*,size_t,const char*, va_list);
int vsprintf(char*,size_t,const char*,va_list);
int vsscanf(const char*, const char*,va_list);
*/

#endif