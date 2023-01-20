ENTRY(_start)

BASE_ADDRESS        =   0x80200000;

SECTIONS {
    . = BASE_ADDRESS;
    kernelStart = .;
    . = ALIGN(4096);
    .text : {
        *(.boot)
        *(.text .text.*)
        . = ALIGN(4096);
        _trampoline = .;
        *(.trampoline)
        . = ALIGN(8192);
        textEnd = .;
    }
    . = ALIGN(4096);
    .data : {
        *(.data .data.*)
        . = ALIGN(4096);
        _stack = .;
        *(.stack)
    }
    . = ALIGN(4096);
    bssStart = .;
    .bss : {
        *(.bss .bss.* .sbss .sbss.*)
        . = ALIGN(4096);
        _pgdir = .;
        *(.pgdir)
        . = ALIGN(4096);
        bssEnd = .;
    }
    . = ALIGN(4096);
    kernelEnd = .;
}