MEMORY {
    RAM : ORIGIN = 0x022530e2, LENGTH = 60k
}

SECTIONS {
    . = 0x022530e2;

    .text : {
        _start = 0x022530e2;
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
