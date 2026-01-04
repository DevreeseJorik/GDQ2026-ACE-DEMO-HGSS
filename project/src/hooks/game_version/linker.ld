MEMORY {
    RAM : ORIGIN = 0x20f566c, LENGTH = 60k
}

SECTIONS {
    . = 0x20f566c;

    .text : {
        _start = 0x20f566c;
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
