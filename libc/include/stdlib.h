#ifndef _STDLIB_H
#define _STDLIB_H 1


#include <stddef.h>
#include <stdint.h>
#include <sys/types>
#include <stdarg.h>


#define EXIT_FAILURE -1
#define EXIT_SUCCESS 0

#define RAND_MAX 32767






void _Exit(int);
void _exit(int);
void exit(int);
void abort(void);
int abs(int);
int atexit(void (*)(void));
double atof(const char *);
int atoi(const char *);
long atol(const char *);
long long atoll(const char *);



void* calloc(size_t, size_t);
void* malloc(size_t);
void free(void *);
void* realloc(void *, size_t);









/*
long          a64l(const char *);
[Option End]
void         *bsearch(const void *, const void *, size_t, size_t,
                  int (*)(const void *, const void *));
div_t         div(int, int);
[XSI][Option Start]
double        drand48(void);
double        erand48(unsigned short [3]);
[Option End]
char         *getenv(const char *);
[CX][Option Start]
int           getsubopt(char **, char *const *, char **);
[Option End]
[XSI][Option Start]
int           grantpt(int);
char         *initstate(unsigned, char *, size_t);
long          jrand48(unsigned short [3]);
char         *l64a(long);
[Option End]
long          labs(long);
[XSI][Option Start]
void          lcong48(unsigned short [7]);
[Option End]
ldiv_t        ldiv(long, long);
long long     llabs(long long);
lldiv_t       lldiv(long long, long long);
[XSI][Option Start]
long          lrand48(void);
[Option End]
int           mblen(const char *, size_t);
size_t        mbstowcs(wchar_t *restrict, const char *restrict, size_t);
int           mbtowc(wchar_t *restrict, const char *restrict, size_t);
[CX][Option Start]
char         *mkdtemp(char *);
int           mkstemp(char *);
[Option End]
[XSI][Option Start]
long          mrand48(void);
long          nrand48(unsigned short [3]);
[Option End]
[ADV][Option Start]
int           posix_memalign(void **, size_t, size_t);
[Option End]
[XSI][Option Start]
int           posix_openpt(int);
char         *ptsname(int);
int           putenv(char *);
[Option End]
void          qsort(void *, size_t, size_t, int (*)(const void *,
                  const void *));
int           rand(void);
[OB CX][Option Start]
int           rand_r(unsigned *);
[Option End]
[XSI][Option Start]
long          random(void);
[Option End]
[XSI][Option Start]
char         *realpath(const char *restrict, char *restrict);
unsigned short *seed48(unsigned short [3]);
[Option End]
[CX][Option Start]
int           setenv(const char *, const char *, int);
[Option End]
[XSI][Option Start]
void          setkey(const char *);
char         *setstate(char *);
[Option End]
void          srand(unsigned);
[XSI][Option Start]
void          srand48(long);
void          srandom(unsigned);
[Option End]
double        strtod(const char *restrict, char **restrict);
float         strtof(const char *restrict, char **restrict);
long          strtol(const char *restrict, char **restrict, int);
long double   strtold(const char *restrict, char **restrict);
long long     strtoll(const char *restrict, char **restrict, int);
unsigned long strtoul(const char *restrict, char **restrict, int);
unsigned long long
              strtoull(const char *restrict, char **restrict, int);
int           system(const char *);
[XSI][Option Start]
int           unlockpt(int);
[Option End]
[CX][Option Start]
int           unsetenv(const char *);
[Option End]
size_t        wcstombs(char *restrict, const wchar_t *restrict, size_t);
int           wctomb(char *, wchar_t);

*/








#endif