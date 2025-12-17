@ hook to be set

.arch armv5te
.text
.code	32
.arm
.global start
_start:
bl 0x1ff82c8 @ Sys_ReloadMemory
bl 0x1ff81e8 @ Sys_ForceBoot


