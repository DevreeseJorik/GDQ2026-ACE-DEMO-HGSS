MEMORY {
    RAM : ORIGIN = 0x1ff81d4, LENGTH = 60k
}

SECTIONS {
    . = 0x1ff81d4;

    .text : {
        _start = 0x1ff81d4;
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
