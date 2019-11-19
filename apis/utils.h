#ifndef UTILS_H
#define UTILS_H
char signum(int a)
{
	if (a<0)
		return -1;
	if (a>0)
		return 1;
	return 0;
}
void intswap(int *a,int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}
int roundtoint(double a)
{
	if (a == 0)
		return 0;
	if (a < 0)
	{
		if (abs(a)-(int) abs(a) < 0.5)
		{
			return (int) a;
		}
		else
		{
			return (int) (a-1);
		}
	}
	else
	{
		if (a-(int)a < 0.5)
		{
			return (int) a;
		}
		else
		{
			return (int) (a+1);
		}
	}
}



#endif