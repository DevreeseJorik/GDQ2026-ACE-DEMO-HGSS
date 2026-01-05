@ hook to be set

.arch armv5te
.text
.code	16
.thumb
.global start
_start:
mov r0, #0x14
pop {r3, pc}


