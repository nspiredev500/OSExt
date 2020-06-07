#include "../../kernel.h"






void exit(uint32_t *regs)
{
	struct process* p = scheduler_running_process();
	if (p == NULL)
	{
		panic("exit called without a running process!\n");
	}
	scheduler_remove_process(p);
}



