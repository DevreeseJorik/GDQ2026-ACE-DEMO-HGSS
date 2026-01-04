@ hook to be set
@ add hook to PokepicManager_BufferPlttData before handling palette data, to inject a custom one.

.arch armv5te
.text
.code	16
.thumb
.global start
_start:
bl 0x023c4500 @ custom palette payload

