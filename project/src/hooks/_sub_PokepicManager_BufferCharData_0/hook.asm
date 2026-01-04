@ hook to be set
@ overwrite PokepicManager_BufferCharData's call to NNS_G2dGetUnpackedCharacterData

.arch armv5te
.text
.code	16
.thumb
.global start
_start:
bl 0x0 @ _call_NNS_G2dGetUnpackedCharacterData
