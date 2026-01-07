MEMORY {
    RAM : ORIGIN = 0x020faf5c, LENGTH = 60k
}

SECTIONS {
    . = 0x020faf5c;

    .text : {
        _start = 0x020faf5c;
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
