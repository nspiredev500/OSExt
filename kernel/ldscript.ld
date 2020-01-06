


SECTIONS
{
	
	.text : {
		_EXEC_START = .;
		*(.text);
	}
	.data : {
		*(.data);
    }

	.bss : {
		*(.bss);
		
    }
	_EXEC_END = .;
}

























