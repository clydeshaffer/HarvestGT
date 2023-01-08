.import _tiles
.export _asmTileCopy

.segment "ZEROPAGE"
srcPtr:
	.res 2

.segment "CODE"


.proc _asmTileCopy: near
    STA srcPtr
    STX srcPtr+1

    LDY #0
:
    LDA (srcPtr), y
    STA _tiles, y
    INY
    BNE :-
    INC srcPtr+1

    LDY #0
:
    LDA (srcPtr), y
    STA _tiles+256, y
    INY
    BNE :-
    INC srcPtr+1

    LDY #0
:
    LDA (srcPtr), y
    STA _tiles+512, y
    INY
    BNE :-
    INC srcPtr+1

    LDY #0
:
    LDA (srcPtr), y
    STA _tiles+768, y
    INY
    BNE :-
    INC srcPtr+1


    RTS
.endproc