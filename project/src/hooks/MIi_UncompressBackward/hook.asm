@ hook to be set

.arch armv5te
.text
.code	32
.arm
.global start
_start:
@ make sure to branch without link register, as this function never pushes lr and just uses bx lr at the end.
@ meaning if we jump to our payload we still need to know what the link register state was to return gracefully.
b 0x23C4020
