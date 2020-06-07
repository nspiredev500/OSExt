#ifndef _ASSERT_H
#define _ASSERT_H 1

void assert(long long exp,char* exprstring, char* file, char *line, char* function);


#ifndef NDEBUG
#define assert(X)
#else
#define assert(X) do { if ( ((long long) X) == 0) { assert(X,#X,__FILE__,__LINE__,__func__);} } while (0)
#endif




#endif