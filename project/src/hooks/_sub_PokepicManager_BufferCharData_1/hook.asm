@ hook to be set
@ overwrite PokepicManager_BufferCharData's call to to UnscanPokepic

.arch armv5te
.text
.code	16
.thumb
.global start
_start:
bl 0x023C4460 @ custom sprite payload
