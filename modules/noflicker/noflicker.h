#ifndef NOFLICKER_H
#define NOFLICKER_H

// arm nop: 0xe1a00000





#define SLEDS 1
static unsigned int sled_start[SLEDS] = {
	0x100237c8
};
static unsigned int sled_end[SLEDS] = {
	0x100237d4
};
bool sleds_installed = false;
unsigned int *sleds[SLEDS];

void initSleds()
{
	for (int i = 0;i<SLEDS;i++)
	{
		int length = (sled_end[i]-sled_start[i])/8;
		sleds[i] = calloc(sizeof(int),length);
	}
}


void installSleds()
{
	if (sleds_installed)
		return;
	for (int i = 0;i<SLEDS;i++)
	{
		int length = (sled_end[i]-sled_start[i])/8;
		unsigned int *sledstart = (unsigned int*) sled_start[i];
		for (int a = 0;a<length;a++)
		{
			sleds[i][a] = *(sledstart+a);
			*(sledstart+a) = 0xe1a00000;
		}
	}
	sleds_installed = true;
}
void uninstallSleds()
{
	if (! sleds_installed)
		return;
	for (int i = 0;i<SLEDS;i++)
	{
		int length = (sled_end[i]-sled_start[i])/8;
		unsigned int *sledstart = (unsigned int*) sled_start[i];
		for (int a = 0;a<length;a++)
		{
			*(sledstart+a) = sleds[i][a];
		}
	}
}




#endif