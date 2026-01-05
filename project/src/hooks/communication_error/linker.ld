MEMORY {
    RAM : ORIGIN = 0x02037d84, LENGTH = 60k
}

SECTIONS {
    . = 0x02037d84;

    .text : {
        _start = 0x02037d84;
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
