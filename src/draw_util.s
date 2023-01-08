.export _pushRect, _rect, _queue_flags_param
.importzp sp, ptr1
.import incsp2, pushax
.import _banksMirror
.importzp _queue_start, _queue_end, _queue_count, _queue_pending

BankReg = $2005
DMAFlags = $2007

Q_DMAFlags = $0200
Q_VX       = $0300
Q_VY       = $0400
Q_GX       = $0500
Q_GY       = $0600
Q_WIDTH    = $0700
Q_HEIGHT   = $0800
Q_Color    = $0900
Q_BankReg  = $0A00

_queue_flags_param = $3200
_rect = $3201 ;borrow unbanked Audio RAM memory

; ---------------------------------------------------------------
; void __near__ pushRect ()
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_pushRect: near

    ;jsr     pushax


    ldy     _queue_end ;4
    inc     _queue_end ;6
    inc     _queue_count ;6

    lda     _banksMirror ;get current banks reg 4
    tax ;4
    ora     #$40 ;make BANK1 version of same 2
    sta     BankReg ;switch to BANK1 4


    lda     _rect+7 ;bank 4
    sta     Q_BankReg, y ;5

    lda     _rect+0 ;x
    sta     Q_VX, y

    lda     _rect+1 ;y
    sta     Q_VY, y

    lda     _rect+4 ;gx
    sta     Q_GX, y

    lda     _rect+5 ;gy
    sta     Q_GY, y

    lda     _rect+2 ;w
    sta     Q_WIDTH, y

    lda     _rect+3 ;h
    sta     Q_HEIGHT, y

    lda     _rect+6 ;color
    sta     Q_Color, y

    lda     _queue_flags_param ;flags
    sta     Q_DMAFlags, y

    ;now finally switch back to BANK0

    stx     BankReg


    rts 
    ;jmp     incsp2

.endproc