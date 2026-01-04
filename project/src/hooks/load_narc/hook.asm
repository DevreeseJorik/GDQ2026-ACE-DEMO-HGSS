@ hook to be set

.arch armv5te
.text
.code	16
.thumb
.global start
_start:
bl 0x0 @ custom narc payload
