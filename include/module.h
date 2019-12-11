#ifndef MODULE_H
#define MODULE_H

bool (*addEarlyFunction)(unsigned int);
bool (*addFunction)(unsigned int);
bool (*addDrawFunction)(unsigned int);
bool (*addSyscallFunction)(unsigned int);
void (*removeEarlyFunction)(unsigned int);
void (*removeFunction)(unsigned int);
void (*removeDrawFunction)(unsigned int);
void (*removeSyscallFunction)(unsigned int);
bool (*shareFunction)(char *,unsigned int);
unsigned int (*searchFunction)(char *);



void initModule(char **argv) // not really char*, but it's passed like this to the module
{
	unsigned int *funcs = (unsigned int*) argv;
	addEarlyFunction = funcs[0];
	addFunction = funcs[1];
	addSyscallFunction = funcs[2];
	shareFunction = funcs[3];
	searchFunction = funcs[4];
	removeEarlyFunction = funcs[5];
	removeFunction = funcs[6];
	removeDrawFunction = funcs[7];
	removeSyscallFunction = funcs[8];
	addDrawFunction = funcs[9];
}








#endif