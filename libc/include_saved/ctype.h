#ifndef _CTYPE_H
#define _CTYPE_H 1


#include <libctypes/locale_t.h>


int isalnum(int i);
int isalnum_l(int i, locale_t l);

int isalpha(int i);
int isalpha_l(int i, locale_t l);


int isascii(int i);
int isascii_l(int i, locale_t l);


int isblank(int i);
int isblank_l(int i, locale_t l);


int iscntrl(int i);
int iscntrl_l(int i, locale_t l);


int isdigit(int i);
int isdigit_l(int i, locale_t l);


int islower(int i);
int islower_l(int i, locale_t l);


int isprint(int i);
int isprint_l(int i, locale_t l);


int ispunct(int i);
int ispunct_l(int i, locale_t l);


int isspace(int i);
int isspace_l(int i, locale_t l);


int isupper(int i);
int isupper_l(int i, locale_t l);


int isxdigit(int i);
int isxdigit_l(int i, locale_t l);

int toascii(int i);

int tolower(int i);
int tolower_l(int i, locale_t l);


int toupper(int i);
int toupper_l(int i, locale_t l);



#define _tolower(c) tolower(c)
#define _toupper(c) toupper(c)



#endif