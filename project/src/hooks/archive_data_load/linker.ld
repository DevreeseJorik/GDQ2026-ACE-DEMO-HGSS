MEMORY {
    RAM : ORIGIN = 0x0200750a, LENGTH = 60k
}

SECTIONS {
    . = 0x0200750a;

    .text : {
        _start = 0x0200750a;
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
