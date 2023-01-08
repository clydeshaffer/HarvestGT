.export _LD_enemy_index, _ST_enemy_index, _enemy_index_on_screen
.import _enemies, _tempEnemy, _camera_x, _camera_y

.segment "CODE"


;
;((enemy->x.b.msb + 1) > (camera_x.b.msb)
;                && enemy->x.b.msb < camRIGHT
;                && (enemy->y.b.msb + 1) > camera_y.b.msb
;                && enemy->y.b.msb < camBOTTOM))
;

.proc _enemy_index_on_screen: near
    PHY
    PHX
    TAY
    AND #$0F
    TAX
    TYA
    LDY table16, x
    CMP #16
    BCS on_screen_enemy_UpperHalf
    LDA _enemies+5, y
    CMP _camera_x+1
    BCC ret_enemy_off_screen_LOW
    CLC
    LDA _camera_x+1
    ADC #9
    CMP _enemies+5, y
    BCC ret_enemy_off_screen_LOW
    LDA _enemies+7, y
    CMP _camera_y+1
    BCC ret_enemy_off_screen_LOW
    CLC
    LDA _camera_y+1
    ADC #9
    CMP _enemies+7, y
    BCC ret_enemy_off_screen_LOW
    LDA #1
    STA _enemies+10, y
    PLX
    PLY
    RTS
on_screen_enemy_UpperHalf:
    LDA _enemies+261, y
    CMP _camera_x+1
    BCC ret_enemy_off_screen_HIGH
    CLC
    LDA _camera_x+1
    ADC #9
    CMP _enemies+261, y
    BCC ret_enemy_off_screen_HIGH
    LDA _enemies+263, y
    CMP _camera_y+1
    BCC ret_enemy_off_screen_HIGH
    CLC
    LDA _camera_y+1
    ADC #9
    CMP _enemies+263, y
    BCC ret_enemy_off_screen_HIGH
    LDA #1
    STA _enemies+266, y
    PLX
    PLY
    RTS
    ret_enemy_off_screen_HIGH:
    LDA #0
    STA _enemies+266, y
    PLX
    PLY
    RTS
    ret_enemy_off_screen_LOW:
    LDA #0
    STA _enemies+10, y
    PLX
    PLY
    RTS
    
.endproc

    table16:
    .byte 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240

; void LD_enemy_index(char i) take index i from enemies array and fill tempEnemy
; MobState struct is 16 bytes long and MAX_ENEMIES is 32
.proc _LD_enemy_index: near
    PHY
    PHX
    TAY
    AND #$0F
    TAX
    TYA
    LDY table16, x
    CMP #16
    BCS LD_Enemy_UpperHalf
    LDA _enemies+0, y
    STA _tempEnemy
    LDA _enemies+1, y
    STA _tempEnemy+1
    LDA _enemies+2, y
    STA _tempEnemy+2
    LDA _enemies+3, y
    STA _tempEnemy+3
    LDA _enemies+4, y
    STA _tempEnemy+4
    LDA _enemies+5, y
    STA _tempEnemy+5
    LDA _enemies+6, y
    STA _tempEnemy+6
    LDA _enemies+7, y
    STA _tempEnemy+7
    LDA _enemies+8, y
    STA _tempEnemy+8
    LDA _enemies+9, y
    STA _tempEnemy+9
    LDA _enemies+10, y
    STA _tempEnemy+10
    LDA _enemies+11, y
    STA _tempEnemy+11
    LDA _enemies+12, y
    STA _tempEnemy+12
    PLX
    PLY
    RTS
LD_Enemy_UpperHalf:
    LDA _enemies+256, y
    STA _tempEnemy
    LDA _enemies+257, y
    STA _tempEnemy+1
    LDA _enemies+258, y
    STA _tempEnemy+2
    LDA _enemies+259, y
    STA _tempEnemy+3
    LDA _enemies+260, y
    STA _tempEnemy+4
    LDA _enemies+261, y
    STA _tempEnemy+5
    LDA _enemies+262, y
    STA _tempEnemy+6
    LDA _enemies+263, y
    STA _tempEnemy+7
    LDA _enemies+264, y
    STA _tempEnemy+8
    LDA _enemies+265, y
    STA _tempEnemy+9
    LDA _enemies+266, y
    STA _tempEnemy+10
    LDA _enemies+267, y
    STA _tempEnemy+11
    LDA _enemies+268, y
    STA _tempEnemy+12
    PLX
    PLY
    RTS
.endproc

.proc _ST_enemy_index: near
    PHY
    PHX
    TAY
    AND #$0F
    TAX
    TYA
    LDY table16, x
    CMP #16
    BCS ST_Enemy_UpperHalf
    LDA _tempEnemy+0
    STA _enemies+0, y
    LDA _tempEnemy+1
    STA _enemies+1, y
    LDA _tempEnemy+2
    STA _enemies+2, y
    LDA _tempEnemy+3
    STA _enemies+3, y
    LDA _tempEnemy+4
    STA _enemies+4, y
    LDA _tempEnemy+5
    STA _enemies+5, y
    LDA _tempEnemy+6
    STA _enemies+6, y
    LDA _tempEnemy+7
    STA _enemies+7, y
    LDA _tempEnemy+8
    STA _enemies+8, y
    LDA _tempEnemy+9
    STA _enemies+9, y
    LDA _tempEnemy+10
    STA _enemies+10, y
    LDA _tempEnemy+11
    STA _enemies+11, y
    LDA _tempEnemy+12
    STA _enemies+12, y
    PLX
    PLY
    RTS
ST_Enemy_UpperHalf:
LDA _tempEnemy+0
    STA _enemies+256, y
    LDA _tempEnemy+1
    STA _enemies+257, y
    LDA _tempEnemy+2
    STA _enemies+258, y
    LDA _tempEnemy+3
    STA _enemies+259, y
    LDA _tempEnemy+4
    STA _enemies+260, y
    LDA _tempEnemy+5
    STA _enemies+261, y
    LDA _tempEnemy+6
    STA _enemies+262, y
    LDA _tempEnemy+7
    STA _enemies+263, y
    LDA _tempEnemy+8
    STA _enemies+264, y
    LDA _tempEnemy+9
    STA _enemies+265, y
    LDA _tempEnemy+10
    STA _enemies+266, y
    LDA _tempEnemy+11
    STA _enemies+267, y
    LDA _tempEnemy+12
    STA _enemies+268, y
    PLX
    PLY
    RTS

.endproc