MEMORY {
    RAM : ORIGIN = 0x020dd638, LENGTH = 60k
}

SECTIONS {
    . = 0x020dd638;

    .text : {
        _start = 0x020dd638;
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
