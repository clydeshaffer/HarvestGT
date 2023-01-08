    .export _SineTable
    .export _Tileset_00, _FontSprites, _FontFrames, _BattleBG
    .export _HeroSprites, _HeroFrames, _DynaWave
    .export _MainMusic, _BattleMusic
    .export _EnemyFrames_VEGGIES, _EnemySprites_VEGGIES
    .export _HomesteadMap, _Forest0Map, _MazeMap, _DesertMap, _BeachMap, _ClearingMap, _RoadMap
    .segment "COMMON"

_SineTable:
	.byte 128,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173
	.byte 176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215
	.byte 218,220,222,224,226,228,230,232,234,235,237,238,240,241,243,244
	.byte 245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255
	.byte 255,255,255,255,254,254,254,253,253,252,251,250,250,249,248,246
	.byte 245,244,243,241,240,238,237,235,234,232,230,228,226,224,222,220
	.byte 218,215,213,211,208,206,203,201,198,196,193,190,188,185,182,179
	.byte 176,173,170,167,165,162,158,155,152,149,146,143,140,137,134,131
	.byte 128,124,121,118,115,112,109,106,103,100,97,93,90,88,85,82
	.byte 79,76,73,70,67,65,62,59,57,54,52,49,47,44,42,40
	.byte 37,35,33,31,29,27,25,23,21,20,18,17,15,14,12,11
	.byte 10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0 
	.byte 0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9
	.byte 10,11,12,14,15,17,18,20,21,23,25,27,29,31,33,35
	.byte 37,40,42,44,47,49,52,54,57,59,62,65,67,70,73,76
	.byte 79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124

_MainMusic:
    .incbin "homestead.gtm2"

_BattleMusic:
    .incbin "battle.gtm2"

_HeroFrames:
    .incbin "farmer.gsi"

_FontFrames:
    .incbin "font.gsi"

_EnemyFrames_VEGGIES:
    .incbin "vegemons.gsi"

    .segment "LOADONCE"

_DynaWave:
    .incbin "audio_fw.bin.deflate"

_HeroSprites:
    .incbin "farmer.gtg.deflate"

_FontSprites:
    .incbin "font.gtg.deflate"

_BattleBG:
    .incbin "battle_bg.gtg.deflate"

    .segment "TILES"

_Tileset_00:
    .incbin "tiles.gtg.deflate"

_HomesteadMap:
    .incbin "homestead.map"

_Forest0Map:
    .incbin "forest0.map"

_MazeMap:
    .incbin "maze.map"

_DesertMap:
    .incbin "desert.map"
_BeachMap:
    .incbin "beach.map"
_ClearingMap:
    .incbin "clearing.map"
_RoadMap:
    .incbin "road.map"

    .segment "MONSTERS"
    
_EnemySprites_VEGGIES:
    .incbin "vegemons.gtg.deflate"