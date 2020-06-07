#ifndef _SYS_SYSCALL_TEMPLATES
#define _SYS_SYSCALL_TEMPLATES 1



int syscall(unsigned int nr);
int syscall1(unsigned int nr,int p1);
int syscall2(unsigned int nr,int p1,int p2);
int syscall3(unsigned int nr,int p1,int p2,int p3);
int syscall4(unsigned int nr,int p1,int p2,int p3,int p4);
int syscall5(unsigned int nr,int p1,int p2,int p3,int p4,int p5);











#endif