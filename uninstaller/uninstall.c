






int main(int argsn,char **argc)
{
	
	register uint32_t control_reg asm("r0");
	asm volatile("mrc p15, 0, r0, c1, c0, 0":"=r" (control_reg)::);
	if ((control_reg & (1 << 13)) == (1 << 13))
	{
		register uint32_t tt_base asm("r0");
		asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
		
		tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
		uint32_t *tt = (uint32_t*) tt_base;
		if (tt[((uint32_t) virtual_base_address) >> 20] != 0)
		{
			// kernel base address already used and interrupt vector high probably means osext is already installed
			// so uninstall it
			asm volatile("swi 0xf80000":"=r" (control_reg)::); // use the uninstall syscall
			if (control_reg != 0)
			{
				// clean the virtual address space
				for (uint32_t i = (uint32_t) virtual_base_address;i<0xfff00000;i+=SECTION_SIZE)
				{
					tt[i >> 20] = 0;
				}
				// free the old kernel
				ti_free((void*) control_reg);
			}
			else
			{
				// uninstallation could not be done
				return 0xDEAD;
			}
		}
	}
	return 0;
}










