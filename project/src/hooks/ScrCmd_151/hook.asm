@ hook to be set
@ overwrite unused script command ScrCmd_151 to jump to custom subroutine to set partysize to 0

.arch armv5te
.text
.code	16
.thumb
.global start
_start:
.word 0x023c45A1


