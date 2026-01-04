@ hook to be set

.arch armv5te
.text
.code	16
.thumb
.global start
_start:
bl 0x023c4500 @ custom palette payload

