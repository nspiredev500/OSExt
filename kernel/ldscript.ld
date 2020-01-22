
ENTRY(main);

/* linker variables get deformed by relocation, subtract _EXEC_START to get the real value
   maybe they don't work after self-relocation, so don't use them afterwards
*/
SECTIONS
{
	.text : {
		*(.text);
	}
	.data : {
		*(.data);
    }

	.bss : {
		*(.bss);
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
	_EXEC_SIZE = SIZEOF (.text) + SIZEOF (.data) + SIZEOF (.bss) + SIZEOF (.got);
	_GOT_START = ADDR (.got);
	_GOT_SIZE = SIZEOF (.got);
}
























