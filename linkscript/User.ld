ENTRY(_userStart)
SECTIONS {
    . = 0x40000000;
	.text : {
		*(.text)
	}
	. = ALIGN(4096);
	.data : {
		*(.data)
	}
    . = ALIGN(4096);
  	.bss : {
		*(.bss)
	}
  	. = ALIGN(4096);
	. = 0x040200000;
	.fsipcbuf : {
		*(.buffer)
		. = ALIGN(4096);
	}
	. = ALIGN(4096);
  	end = . ;
}