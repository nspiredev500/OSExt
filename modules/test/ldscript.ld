ENTRY(module_start);
SECTIONS
{
	. = 0x0;
	.text : {
		*(.text);
		*(.text.*)
	}
	.data : {
		*(.rodata*)
		*(.data*)
    }
	
	.got : {
		*(.got)
	}
	
	.bss : {
		*(.bss);
    }
	
	
}