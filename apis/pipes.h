#ifndef PIPES_H
#define PIPES_H
typedef struct{
	void* buffer;
	int buffsize;
} Pipe;
char popChar(Pipe* p);
int popInt(Pipe* p);
void putChar(Pipe* p,char c);
void putInt(Pipe* p,int i);

char popChar(Pipe* p)
{
	
}
int popInt(Pipe* p)
{
	
}
void putChar(Pipe* p,char c)
{
	
}
void putInt(Pipe* p,int i)
{
	
	
}

#endif