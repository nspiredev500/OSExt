
ENTRY(__entry);
/* linker variables get deformed by relocation, subtract _EXEC_START to get the real value
   maybe they don't work after self-relocation, so don't use them afterwards
*/
SECTIONS
{
	.text 0x0 : {
		_start = .;
		*(.text);
		*(.text.*)
	}
	.data : {
		*(.rodata*)
		*(.data*)
		KEEP(*(.genzehn))
    }
	.got : {
		*(.got.plt*)
		*(.got)
		/* from ndless linker script
		*/
		LONG(0xFFFFFFFF)
	}
	/* from ndless linker script
	*/
	.eh_frame : {
		KEEP(*(.eh_frame))
	}
	
	/* from ndless linker script
	*/
	.ARM.extab   : { KEEP(*(.ARM.extab* .gnu.linkonce.armextab.*)) }
	PROVIDE_HIDDEN (__exidx_start = .);
	.ARM.exidx   : { KEEP(*(.ARM.exidx* .gnu.linkonce.armexidx.*)) }
	PROVIDE_HIDDEN (__exidx_end = .);
	
	
	
	.bss : {
		PROVIDE(__bss_start = .);
		*(.bss);
		PROVIDE(_end = .);
    }
	
	
	
	
	
	
	_EXEC_START = ADDR(.text);
	/* _EXEC_END doesn't take the GOT into account
	*/
	_TEXT_START = ADDR(.text);
	_DATA_START = ADDR(.data);
	_BSS_START = ADDR(.bss);
	_TEXT_SIZE = SIZEOF (.text);
	_DATA_SIZE = SIZEOF (.data);
	_BSS_SIZE = SIZEOF (.bss);
	_EXEC_SIZE = SIZEOF (.text) + SIZEOF (.data) + SIZEOF (.bss) + SIZEOF (.got) + SIZEOF (.ARM.extab) + SIZEOF (.ARM.exidx);
	_GOT_START = ADDR (.got);
	_GOT_SIZE = SIZEOF (.got);
}
























