MEMORY {
    RAM : ORIGIN = 0x0200bf8e, LENGTH = 60k
}

SECTIONS {
    . = 0x0200bf8e;

    .text : {
        _start = 0x0200bf8e;
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
