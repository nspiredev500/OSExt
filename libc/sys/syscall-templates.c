


inline int syscall(unsigned int nr)
{
	register int r0 asm("r0");
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr)
		: "memory");
	return r0;
}
inline int syscall1(unsigned int nr,int p1)
{
	register int r0 asm("r0") = p1;
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr)
		: "memory");
	return r0;
}
inline int syscall2(unsigned int nr,int p1,int p2)
{
	register int r0 asm("r0") = p1;
	register int r1 asm("r1") = p2;
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r1)
		: "memory");
	return r0;
}
inline int syscall3(unsigned int nr,int p1,int p2,int p3)
{
	register int r0 asm("r0") = p1;
	register int r1 asm("r1") = p2;
	register int r2 asm("r2") = p3;
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r1), "r" (r2)
		: "memory");
	return r0;
}
inline int syscall4(unsigned int nr,int p1,int p2,int p3,int p4)
{
	register int r0 asm("r0") = p1;
	register int r1 asm("r1") = p2;
	register int r2 asm("r2") = p3;
	register int r3 asm("r3") = p4;
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r1), "r" (r2), "r" (r3)
		: "memory");
	return r0;
}
inline int syscall5(unsigned int nr,int p1,int p2,int p3,int p4,int p5)
{
	register int r0 asm("r0") = p1;
	register int r1 asm("r1") = p2;
	register int r2 asm("r2") = p3;
	register int r3 asm("r3") = p4;
	register int r4 asm("r4") = p5;
	asm volatile(
		"swi %[nr]\n"
		: "=r" (r0)
		: [nr] "i" (nr), "r" (r1), "r" (r2), "r" (r3), "r" (r4)
		: "memory");
	return r0;
}




















