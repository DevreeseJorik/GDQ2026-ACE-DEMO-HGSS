MEMORY {
    RAM : ORIGIN = 0x0200999c, LENGTH = 60k
}

SECTIONS {
    . = 0x0200999c;

    .text : {
        _start = 0x0200999c;
        *(.text*)
        . = ALIGN(4);
    } > RAM

    .rodata : {
        *(.rodata*)
        . = ALIGN(4);
    } > RAM

    .data : {
        *(.data*)
        . = ALIGN(4);
    } > RAM

    .got : {
        *(.got*)
        . = ALIGN(4);
    } > RAM
}
