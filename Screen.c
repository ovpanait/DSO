#include	"Common.h"
#include	"Board.h"
#include	"Screen.h"
#include 	"scope.h"
#include 	"string.h"
#include 	"stdlib.h"

extern struct scope dso_scope;
extern struct waveform wave;

// ==========================================================
//	File Scope Variables
// ==========================================================
//

static U8 buf[10];
static U8 freq_delay = 15;

#define	ASC8X16_Use_Display_Char_Only
const U8 Font_ASC8X16[256*16] = {
#ifndef ASC8X16_Use_Display_Char_Only
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x00
        0x00,0x00,0x7E,0x81,0xA5,0x81,0x81,0xBD,0x99,0x81,0x81,0x7E,0x00,0x00,0x00,0x00,       //0x01
        0x00,0x00,0x7E,0xFF,0xDB,0xFF,0xFF,0xC3,0xE7,0xFF,0xFF,0x7E,0x00,0x00,0x00,0x00,       //0x02
        0x00,0x00,0x00,0x00,0x6C,0xFE,0xFE,0xFE,0xFE,0x7C,0x38,0x10,0x00,0x00,0x00,0x00,       //0x03
        0x00,0x00,0x00,0x00,0x10,0x38,0x7C,0xFE,0x7C,0x38,0x10,0x00,0x00,0x00,0x00,0x00,       //0x04
        0x00,0x00,0x00,0x18,0x3C,0x3C,0xE7,0xE7,0xE7,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x05
        0x00,0x00,0x00,0x18,0x3C,0x7E,0xFF,0xFF,0x7E,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x06
        0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x3C,0x3C,0x18,0x00,0x00,0x00,0x00,0x00,0x00,       //0x07
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE7,0xC3,0xC3,0xE7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,       //0x08
        0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x42,0x42,0x66,0x3C,0x00,0x00,0x00,0x00,0x00,       //0x09
        0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,0x99,0xBD,0xBD,0x99,0xC3,0xFF,0xFF,0xFF,0xFF,0xFF,       //0x0A
        0x00,0x00,0x1E,0x0E,0x1A,0x32,0x78,0xCC,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00,       //0x0B
        0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x3C,0x18,0x7E,0x18,0x18,0x00,0x00,0x00,0x00,       //0x0C
        0x00,0x00,0x3F,0x33,0x3F,0x30,0x30,0x30,0x30,0x70,0xF0,0xE0,0x00,0x00,0x00,0x00,       //0x0D
        0x00,0x00,0x7F,0x63,0x7F,0x63,0x63,0x63,0x63,0x67,0xE7,0xE6,0xC0,0x00,0x00,0x00,       //0x0E
        0x00,0x00,0x00,0x18,0x18,0xDB,0x3C,0xE7,0x3C,0xDB,0x18,0x18,0x00,0x00,0x00,0x00,       //0x0F
        0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFE,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,       //0x10
        0x00,0x02,0x06,0x0E,0x1E,0x3E,0xFE,0x3E,0x1E,0x0E,0x06,0x02,0x00,0x00,0x00,0x00,       //0x11
        0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x7E,0x3C,0x18,0x00,0x00,0x00,0x00,0x00,       //0x12
        0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x66,0x66,0x00,0x00,0x00,0x00,       //0x13
        0x00,0x00,0x7F,0xDB,0xDB,0xDB,0x7B,0x1B,0x1B,0x1B,0x1B,0x1B,0x00,0x00,0x00,0x00,       //0x14
        0x00,0x7C,0xC6,0x60,0x38,0x6C,0xC6,0xC6,0x6C,0x38,0x0C,0xC6,0x7C,0x00,0x00,0x00,       //0x15
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0xFE,0x00,0x00,0x00,0x00,       //0x16
        0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x7E,0x3C,0x18,0x7E,0x00,0x00,0x00,0x00,       //0x17
        0x00,0x00,0x18,0x3C,0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,       //0x18
        0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x3C,0x18,0x00,0x00,0x00,0x00,       //0x19
        0x00,0x00,0x00,0x00,0x00,0x18,0x0C,0xFE,0x0C,0x18,0x00,0x00,0x00,0x00,0x00,0x00,       //0x1A
        0x00,0x00,0x00,0x00,0x00,0x30,0x60,0xFE,0x60,0x30,0x00,0x00,0x00,0x00,0x00,0x00,       //0x1B
        0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,       //0x1C
        0x00,0x00,0x00,0x00,0x00,0x28,0x6C,0xFE,0x6C,0x28,0x00,0x00,0x00,0x00,0x00,0x00,       //0x1D
        0x00,0x00,0x00,0x00,0x10,0x38,0x38,0x7C,0x7C,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,       //0x1E
        0x00,0x00,0x00,0x00,0xFE,0xFE,0x7C,0x7C,0x38,0x38,0x10,0x00,0x00,0x00,0x00,0x00,       //0x1F
#endif
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x20' '
        0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,       //0x21'!'
        0x00,0x66,0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x22'"'
        0x00,0x00,0x00,0x6C,0x6C,0xFE,0x6C,0x6C,0x6C,0xFE,0x6C,0x6C,0x00,0x00,0x00,0x00,       //0x23'#'
        0x18,0x18,0x7C,0xC6,0xC2,0xC0,0x7C,0x06,0x06,0x86,0xC6,0x7C,0x18,0x18,0x00,0x00,       //0x24'$'
        0x00,0x00,0x00,0x00,0xC2,0xC6,0x0C,0x18,0x30,0x60,0xC6,0x86,0x00,0x00,0x00,0x00,       //0x25'%'
        0x00,0x00,0x38,0x6C,0x6C,0x38,0x76,0xDC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x26'&'
        0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x27'''
        0x00,0x00,0x0C,0x18,0x30,0x30,0x30,0x30,0x30,0x30,0x18,0x0C,0x00,0x00,0x00,0x00,       //0x28'('
        0x00,0x00,0x30,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x30,0x00,0x00,0x00,0x00,       //0x29')'
        0x00,0x00,0x00,0x00,0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00,0x00,0x00,0x00,0x00,       //0x2A'*'
        0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,       //0x2B'+'
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00,0x00,       //0x2C','
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x2D'-'
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,       //0x2E'.'
        0x00,0x00,0x00,0x00,0x02,0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00,0x00,0x00,0x00,       //0x2F'/'
        0x00,0x00,0x38,0x6C,0xC6,0xC6,0xD6,0xD6,0xC6,0xC6,0x6C,0x38,0x00,0x00,0x00,0x00,       //0x30'0'
        0x00,0x00,0x18,0x38,0x78,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00,0x00,0x00,       //0x31'1'
        0x00,0x00,0x7C,0xC6,0x06,0x0C,0x18,0x30,0x60,0xC0,0xC6,0xFE,0x00,0x00,0x00,0x00,       //0x32'2'
        0x00,0x00,0x7C,0xC6,0x06,0x06,0x3C,0x06,0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x33'3'
        0x00,0x00,0x0C,0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00,       //0x34'4'
        0x00,0x00,0xFE,0xC0,0xC0,0xC0,0xFC,0x06,0x06,0x06,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x35'5'
        0x00,0x00,0x38,0x60,0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x36'6'
        0x00,0x00,0xFE,0xC6,0x06,0x06,0x0C,0x18,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,       //0x37'7'
        0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x38'8'
        0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7E,0x06,0x06,0x06,0x0C,0x78,0x00,0x00,0x00,0x00,       //0x39'9'
        0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,       //0x3A':'
        0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x30,0x00,0x00,0x00,0x00,       //0x3B';'
        0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00,       //0x3C'<'
        0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x3D'='
        0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00,       //0x3E'>'
        0x00,0x00,0x7C,0xC6,0xC6,0x0C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,       //0x3F'?'
        0x00,0x00,0x00,0x7C,0xC6,0xC6,0xDE,0xDE,0xDE,0xDC,0xC0,0x7C,0x00,0x00,0x00,0x00,       //0x40'@'
        0x00,0x00,0x10,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0x41'A'
        0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x66,0x66,0x66,0x66,0xFC,0x00,0x00,0x00,0x00,       //0x42'B'
        0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xC0,0xC0,0xC2,0x66,0x3C,0x00,0x00,0x00,0x00,       //0x43'C'
        0x00,0x00,0xF8,0x6C,0x66,0x66,0x66,0x66,0x66,0x66,0x6C,0xF8,0x00,0x00,0x00,0x00,       //0x44'D'
        0x00,0x00,0xFE,0x66,0x62,0x68,0x78,0x68,0x60,0x62,0x66,0xFE,0x00,0x00,0x00,0x00,       //0x45'E'
        0x00,0x00,0xFE,0x66,0x62,0x68,0x78,0x68,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00,       //0x46'F'
        0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xDE,0xC6,0xC6,0x66,0x3A,0x00,0x00,0x00,0x00,       //0x47'G'
        0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0x48'H'
        0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x49'I'
        0x00,0x00,0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00,       //0x4A'J'
        0x00,0x00,0xE6,0x66,0x66,0x6C,0x78,0x78,0x6C,0x66,0x66,0xE6,0x00,0x00,0x00,0x00,       //0x4B'K'
        0x00,0x00,0xF0,0x60,0x60,0x60,0x60,0x60,0x60,0x62,0x66,0xFE,0x00,0x00,0x00,0x00,       //0x4C'L'
        0x00,0x00,0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0x4D'M'
        0x00,0x00,0xC6,0xE6,0xF6,0xFE,0xDE,0xCE,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0x4E'N'
        0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x4F'O'
        0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x60,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00,       //0x50'P'
        0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xD6,0xDE,0x7C,0x0C,0x0E,0x00,0x00,       //0x51'Q'
        0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x6C,0x66,0x66,0x66,0xE6,0x00,0x00,0x00,0x00,       //0x52'R'
        0x00,0x00,0x7C,0xC6,0xC6,0x60,0x38,0x0C,0x06,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x53'S'
        0x00,0x00,0x7E,0x7E,0x5A,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x54'T'
        0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x55'U'
        0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00,0x00,0x00,0x00,       //0x56'V'
        0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xD6,0xD6,0xD6,0xFE,0xEE,0x6C,0x00,0x00,0x00,0x00,       //0x57'W'
        0x00,0x00,0xC6,0xC6,0x6C,0x7C,0x38,0x38,0x7C,0x6C,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0x58'X'
        0x00,0x00,0x66,0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x59'Y'
        0x00,0x00,0xFE,0xC6,0x86,0x0C,0x18,0x30,0x60,0xC2,0xC6,0xFE,0x00,0x00,0x00,0x00,       //0x5A'Z'
        0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,0x00,0x00,0x00,       //0x5B'['
        0x00,0x00,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x06,0x02,0x00,0x00,0x00,0x00,       //0x5C'\'
        0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00,0x00,0x00,       //0x5D']'
        0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x5E'^'
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,       //0x5F'_'
        0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x60'`'
        0x00,0x00,0x00,0x00,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x61'a'
        0x00,0x00,0xE0,0x60,0x60,0x78,0x6C,0x66,0x66,0x66,0x66,0x7C,0x00,0x00,0x00,0x00,       //0x62'b'
        0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC0,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x63'c'
        0x00,0x00,0x1C,0x0C,0x0C,0x3C,0x6C,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x64'd'
        0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xFE,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x65'e'
        0x00,0x00,0x38,0x6C,0x64,0x60,0xF0,0x60,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00,       //0x66'f'
        0x00,0x00,0x00,0x00,0x00,0x76,0xCC,0xCC,0xCC,0xCC,0xCC,0x7C,0x0C,0xCC,0x78,0x00,       //0x67'g'
        0x00,0x00,0xE0,0x60,0x60,0x6C,0x76,0x66,0x66,0x66,0x66,0xE6,0x00,0x00,0x00,0x00,       //0x68'h'
        0x00,0x00,0x18,0x18,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x69'i'
        0x00,0x00,0x06,0x06,0x00,0x0E,0x06,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,       //0x6A'j'
        0x00,0x00,0xE0,0x60,0x60,0x66,0x6C,0x78,0x78,0x6C,0x66,0xE6,0x00,0x00,0x00,0x00,       //0x6B'k'
        0x00,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x6C'l'
        0x00,0x00,0x00,0x00,0x00,0xEC,0xFE,0xD6,0xD6,0xD6,0xD6,0xC6,0x00,0x00,0x00,0x00,       //0x6D'm'
        0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00,       //0x6E'n'
        0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x6F'o'
        0x00,0x00,0x00,0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00,       //0x70'p'
        0x00,0x00,0x00,0x00,0x00,0x76,0xCC,0xCC,0xCC,0xCC,0xCC,0x7C,0x0C,0x0C,0x1E,0x00,       //0x71'q'
        0x00,0x00,0x00,0x00,0x00,0xDC,0x76,0x66,0x60,0x60,0x60,0xF0,0x00,0x00,0x00,0x00,       //0x72'r'
        0x00,0x00,0x00,0x00,0x00,0x7C,0xC6,0x60,0x38,0x0C,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x73's'
        0x00,0x00,0x10,0x30,0x30,0xFC,0x30,0x30,0x30,0x30,0x36,0x1C,0x00,0x00,0x00,0x00,       //0x74't'
        0x00,0x00,0x00,0x00,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x75'u'
        0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00,0x00,0x00,0x00,       //0x76'v'
        0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xD6,0xD6,0xD6,0xFE,0x6C,0x00,0x00,0x00,0x00,       //0x77'w'
        0x00,0x00,0x00,0x00,0x00,0xC6,0x6C,0x38,0x38,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00,       //0x78'x'
        0x00,0x00,0x00,0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7E,0x06,0x0C,0xF8,0x00,       //0x79'y'
        0x00,0x00,0x00,0x00,0x00,0xFE,0xCC,0x18,0x30,0x60,0xC6,0xFE,0x00,0x00,0x00,0x00,       //0x7A'z'
        0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00,       //0x7B''
        0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,       //0x7C'|'
        0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,0x18,0x18,0x18,0x70,0x00,0x00,0x00,0x00,       //0x7D''
        0x00,0x00,0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0x7E'~'
//        0x00,0x00,0x00,0x00,0x10,0x38,0x6C,0xC6,0xC6,0xC6,0xFE,0x00,0x00,0x00,0x00,0x00,       //0x7F''
	0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x7E,0x62,0x60,0x60,0xC0,
//	0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x7C,0x60,0x60,0xC0,0x00,0x00,0x00,       //0xE6		-- Greek (u)
#ifdef ASC8X16_USE_EXTEND_CHARACTER_SET
        0x00,0x00,0x3C,0x66,0xC2,0xC0,0xC0,0xC0,0xC2,0x66,0x3C,0x0C,0x06,0x7C,0x00,0x00,       //0x80
        0x00,0x00,0xCC,0x00,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x81
        0x00,0x0C,0x18,0x30,0x00,0x7C,0xC6,0xFE,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x82
        0x00,0x10,0x38,0x6C,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x83
        0x00,0x00,0xCC,0x00,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x84
        0x00,0x60,0x30,0x18,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x85
        0x00,0x38,0x6C,0x38,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x86
        0x00,0x00,0x00,0x00,0x3C,0x66,0x60,0x60,0x66,0x3C,0x0C,0x06,0x3C,0x00,0x00,0x00,       //0x87
        0x00,0x10,0x38,0x6C,0x00,0x7C,0xC6,0xFE,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x88
        0x00,0x00,0xC6,0x00,0x00,0x7C,0xC6,0xFE,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x89
        0x00,0x60,0x30,0x18,0x00,0x7C,0xC6,0xFE,0xC0,0xC0,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x8A
        0x00,0x00,0x66,0x00,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x8B
        0x00,0x18,0x3C,0x66,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x8C
        0x00,0x60,0x30,0x18,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0x8D
        0x00,0xC6,0x00,0x10,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0x8E
        0x38,0x6C,0x38,0x00,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0x8F
        0x18,0x30,0x60,0x00,0xFE,0x66,0x60,0x7C,0x60,0x60,0x66,0xFE,0x00,0x00,0x00,0x00,       //0x90
        0x00,0x00,0x00,0x00,0x00,0xCC,0x76,0x36,0x7E,0xD8,0xD8,0x6E,0x00,0x00,0x00,0x00,       //0x91
        0x00,0x00,0x3E,0x6C,0xCC,0xCC,0xFE,0xCC,0xCC,0xCC,0xCC,0xCE,0x00,0x00,0x00,0x00,       //0x92
        0x00,0x10,0x38,0x6C,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x93
        0x00,0x00,0xC6,0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x94
        0x00,0x60,0x30,0x18,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x95
        0x00,0x30,0x78,0xCC,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x96
        0x00,0x60,0x30,0x18,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0x97
        0x00,0x00,0xC6,0x00,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00,       //0x98
        0x00,0xC6,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x99
        0x00,0xC6,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0x9A
        0x00,0x18,0x18,0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x18,0x18,0x00,0x00,0x00,0x00,       //0x9B
        0x00,0x38,0x6C,0x64,0x60,0xF0,0x60,0x60,0x60,0x60,0xE6,0xFC,0x00,0x00,0x00,0x00,       //0x9C
        0x00,0x00,0x66,0x66,0x3C,0x18,0x7E,0x18,0x7E,0x18,0x18,0x18,0x00,0x00,0x00,0x00,       //0x9D
        0x00,0xF8,0xCC,0xCC,0xF8,0xC4,0xCC,0xDE,0xCC,0xCC,0xCC,0xC6,0x00,0x00,0x00,0x00,       //0x9E
        0x00,0x0E,0x1B,0x18,0x18,0x18,0x7E,0x18,0x18,0x18,0x18,0x18,0xD8,0x70,0x00,0x00,       //0x9F
        0x00,0x18,0x30,0x60,0x00,0x78,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0xA0
        0x00,0x0C,0x18,0x30,0x00,0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00,       //0xA1
        0x00,0x18,0x30,0x60,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0xA2
        0x00,0x18,0x30,0x60,0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x76,0x00,0x00,0x00,0x00,       //0xA3
        0x00,0x00,0x76,0xDC,0x00,0xDC,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00,       //0xA4
        0x76,0xDC,0x00,0xC6,0xE6,0xF6,0xFE,0xDE,0xCE,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0xA5
        0x00,0x3C,0x6C,0x6C,0x3E,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xA6
        0x00,0x38,0x6C,0x6C,0x38,0x00,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xA7
        0x00,0x00,0x30,0x30,0x00,0x30,0x30,0x60,0xC0,0xC6,0xC6,0x7C,0x00,0x00,0x00,0x00,       //0xA8
        0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xC0,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,       //0xA9
        0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x06,0x06,0x06,0x06,0x00,0x00,0x00,0x00,0x00,       //0xAA
        0x00,0xC0,0xC0,0xC2,0xC6,0xCC,0x18,0x30,0x60,0xDC,0x86,0x0C,0x18,0x3E,0x00,0x00,       //0xAB
        0x00,0xC0,0xC0,0xC2,0xC6,0xCC,0x18,0x30,0x66,0xCE,0x9E,0x3E,0x06,0x06,0x00,0x00,       //0xAC
        0x00,0x00,0x18,0x18,0x00,0x18,0x18,0x18,0x3C,0x3C,0x3C,0x18,0x00,0x00,0x00,0x00,       //0xAD
        0x00,0x00,0x00,0x00,0x00,0x36,0x6C,0xD8,0x6C,0x36,0x00,0x00,0x00,0x00,0x00,0x00,       //0xAE
        0x00,0x00,0x00,0x00,0x00,0xD8,0x6C,0x36,0x6C,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,       //0xAF
        0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44,       //0xB0
        0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,       //0xB1
        0xDD,0x77,0xDD,0x77,0xDD,0x77,0xDD,0x77,0xDD,0x77,0xDD,0x77,0xDD,0x77,0xDD,0x77,       //0xB2
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xB3
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xB4
        0x18,0x18,0x18,0x18,0x18,0xF8,0x18,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xB5
        0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xF6,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xB6
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xB7
        0x00,0x00,0x00,0x00,0x00,0xF8,0x18,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xB8
        0x36,0x36,0x36,0x36,0x36,0xF6,0x06,0xF6,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xB9
        0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xBA
        0x00,0x00,0x00,0x00,0x00,0xFE,0x06,0xF6,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xBB
        0x36,0x36,0x36,0x36,0x36,0xF6,0x06,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xBC
        0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xBD
        0x18,0x18,0x18,0x18,0x18,0xF8,0x18,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xBE
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xBF
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xC0
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xC1
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xC2
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xC3
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xC4
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xFF,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xC5
        0x18,0x18,0x18,0x18,0x18,0x1F,0x18,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xC6
        0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x37,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xC7
        0x36,0x36,0x36,0x36,0x36,0x37,0x30,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xC8
        0x00,0x00,0x00,0x00,0x00,0x3F,0x30,0x37,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xC9
        0x36,0x36,0x36,0x36,0x36,0xF7,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xCA
        0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xF7,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xCB
        0x36,0x36,0x36,0x36,0x36,0x37,0x30,0x37,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xCC
        0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xCD
        0x36,0x36,0x36,0x36,0x36,0xF7,0x00,0xF7,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xCE
        0x18,0x18,0x18,0x18,0x18,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xCF
        0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xD0
        0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0xFF,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xD1
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xD2
        0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xD3
        0x18,0x18,0x18,0x18,0x18,0x1F,0x18,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xD4
        0x00,0x00,0x00,0x00,0x00,0x1F,0x18,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xD5
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xD6
        0x36,0x36,0x36,0x36,0x36,0x36,0x36,0xFF,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,       //0xD7
        0x18,0x18,0x18,0x18,0x18,0xFF,0x18,0xFF,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xD8
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xD9
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xDA
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,       //0xDB
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,       //0xDC
        0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,       //0xDD
        0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,       //0xDE
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xDF
        0x00,0x00,0x00,0x00,0x00,0x76,0xDC,0xD8,0xD8,0xD8,0xDC,0x76,0x00,0x00,0x00,0x00,       //0xE0
        0x00,0x00,0x78,0xCC,0xCC,0xCC,0xD8,0xCC,0xC6,0xC6,0xC6,0xCC,0x00,0x00,0x00,0x00,       //0xE1
        0x00,0x00,0xFE,0xC6,0xC6,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,       //0xE2
        0x00,0x00,0x00,0x00,0xFE,0x6C,0x6C,0x6C,0x6C,0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,       //0xE3
        0x00,0x00,0x00,0xFE,0xC6,0x60,0x30,0x18,0x30,0x60,0xC6,0xFE,0x00,0x00,0x00,0x00,       //0xE4
        0x00,0x00,0x00,0x00,0x00,0x7E,0xD8,0xD8,0xD8,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00,       //0xE5
        0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x7C,0x60,0x60,0xC0,0x00,0x00,0x00,       //0xE6	-- (u) micro
        0x00,0x00,0x00,0x00,0x76,0xDC,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00,       //0xE7
        0x00,0x00,0x00,0x7E,0x18,0x3C,0x66,0x66,0x66,0x3C,0x18,0x7E,0x00,0x00,0x00,0x00,       //0xE8
        0x00,0x00,0x00,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0x6C,0x38,0x00,0x00,0x00,0x00,       //0xE9
        0x00,0x00,0x38,0x6C,0xC6,0xC6,0xC6,0x6C,0x6C,0x6C,0x6C,0xEE,0x00,0x00,0x00,0x00,       //0xEA
        0x00,0x00,0x1E,0x30,0x18,0x0C,0x3E,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00,       //0xEB
        0x00,0x00,0x00,0x00,0x00,0x7E,0xDB,0xDB,0xDB,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,       //0xEC
        0x00,0x00,0x00,0x03,0x06,0x7E,0xDB,0xDB,0xF3,0x7E,0x60,0xC0,0x00,0x00,0x00,0x00,       //0xED
        0x00,0x00,0x1C,0x30,0x60,0x60,0x7C,0x60,0x60,0x60,0x30,0x1C,0x00,0x00,0x00,0x00,       //0xEE
        0x00,0x00,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x00,0x00,0x00,0x00,       //0xEF
        0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,       //0xF0
        0x00,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,       //0xF1
        0x00,0x00,0x00,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00,0x7E,0x00,0x00,0x00,0x00,       //0xF2
        0x00,0x00,0x00,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00,0x7E,0x00,0x00,0x00,0x00,       //0xF3
        0x00,0x00,0x0E,0x1B,0x1B,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,       //0xF4
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xD8,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00,       //0xF5
        0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x7E,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,       //0xF6
        0x00,0x00,0x00,0x00,0x00,0x76,0xDC,0x00,0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00,       //0xF7
        0x00,0x38,0x6C,0x6C,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xF8
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xF9
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xFA
        0x00,0x0F,0x0C,0x0C,0x0C,0x0C,0x0C,0xEC,0x6C,0x6C,0x3C,0x1C,0x00,0x00,0x00,0x00,       //0xFB
        0x00,0xD8,0x6C,0x6C,0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xFC
        0x00,0x70,0xD8,0x30,0x60,0xC8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,       //0xFD
        0x00,0x00,0x00,0x00,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x00,0x00,0x00,0x00,0x00,       //0xFE
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00        //0xFF
#endif
};

FONT ASC8X16 = {
	Font_ASC8X16,
	8,
	16,
	8,
	16,
	0x20
	};

// ===== Function Definitions ==================
//
// ==========================================
// Clear screen
void clr_screen(void)
{
 	FillRect(ScreenX0, ScreenY0, ScreenXsize, ScreenYsize, BG_CL);

}
void clr_blk(S16 x, S16 y, S16 sizex, S16 sizey)
{
	FillRect(x, y, sizex, sizey, BG_CL);
}
	
void clr_square_blk(S16 x, S16 y, S16 size){
	FillRect(x, y, size, size, BG_CL);
}

void SetWindow(U16 x, U16 xsize, U16 y, U16 ysize)
{
 write_comm(0x2A);

 write_data(x >> 8);
 write_data(x);
 x = x + xsize - 1;
 write_data(x >> 8);
 write_data(x);

 
 write_comm(0x2B);
 write_data(y >> 8);
 write_data(y);
 y = y + ysize - 1;
 write_data(y >> 8);
 write_data(y);
}



// ==========================================
// Fill rectangle area with given color
void FillRect(S16 x, S16 y, S16 xsize, S16 ysize, U16 color)
{
 U32 tmp; 

 SetWindow(x, xsize, y, ysize);
 
// Delay(200);

 SetToLow(TFT_nCS_Port, (1 << TFT_nCS_Bit));

 SetToLow(TFT_RS_Port, (1 << TFT_RS_Bit));
// Delay(2);

 TFT_Port = (TFT_Port & 0xFF00) | 0x2C;
 SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));

 tmp = (U32)xsize * (U32)ysize;
 // Set up to access Index Register (RS == 1)
 SetToHigh(TFT_RS_Port, (1 << TFT_RS_Bit));

 while(tmp) {
	TFT_Port = (TFT_Port & 0xFF00) | (color >> 8);
 	SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 	SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 	TFT_Port = (TFT_Port & 0xFF00) | color;
 	SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 	SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));
	tmp--;
 	}

 // Set TFT_nCS high
 SetToHigh(TFT_nCS_Port, (1 << TFT_nCS_Bit)); 
}

// Put at (x, y) a 15X16 character [ch] with [f_color] and [b_color]
//
void PutcGenic(U16 x, U16 y, U8 ch, U16 fgcolor, U16 bgcolor, FONT *font)
{
 U8 tmp, tmp2;
 U16 tmp1, tmp3;
 U8 *ptmp;

 // Font address
 ptmp = (U8 *)font->Array + (ch - font->IndexOfs) * ((font->Xsize + 7)/8) * font->Ysize;

 SetWindow(x, font->Xsize, y, font->Ysize);

 SetToLow(TFT_nCS_Port, (1 << TFT_nCS_Bit));

 SetToLow(TFT_RS_Port, (1 << TFT_RS_Bit));
// Delay(2);

 TFT_Port = (TFT_Port & 0xFF00) | 0x2C;
 SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));

 SetToHigh(TFT_RS_Port, (1 << TFT_RS_Bit));

 tmp1 = (font->Xsize * font->Ysize)/8;
 while(tmp1) {
 	tmp = *ptmp;
	tmp2 = 8;
	while(tmp2) {
		tmp3 = (tmp & 0x80) ? fgcolor : bgcolor;

 		TFT_Port = (TFT_Port & 0xFF00) | (tmp3 >> 8);
 		SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 		SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 		TFT_Port = (TFT_Port & 0xFF00) | tmp3;
 		SetToLow(TFT_nWR_Port, (1 << TFT_nWR_Bit));
 		SetToHigh(TFT_nWR_Port, (1 << TFT_nWR_Bit));

		tmp <<= 1;
		tmp2--;
		}
	ptmp++;
	tmp1--;	
 	}

 SetToHigh(TFT_nCS_Port, (1 << TFT_nCS_Bit)); 
}

void PutsGenic(U16 x, U16 y, U8 *str, U16 fgcolor, U16 bgcolor, FONT *font)
{
 U8 tmp;
 
 while((tmp = *str++)) {
	PutcGenic(x, y, tmp, fgcolor, bgcolor, font);
	x += font->CharPitch;
	if(x >= ScreenXsize) {
		y += font->LinePitch;
		x = 0;
		}
 	}
}

/* Display functions */
void grid_display(void)
{
	U8 i;
	U16 gridx, gridy;
	U16 xblk, yblk;
	
	xblk = (WD_WIDTH / GRID_DIST);
	yblk = (WD_HEIGHT / GRID_DIST);

	gridx = WD_OFFSETX;
	gridy = WD_OFFSETY;
	
	for(i = 0; i <  xblk + 1; ++i) {
		if(i == xblk/2)
			FillRect(gridx, gridy, GRID_CENTER_WIDTH, WD_HEIGHT, GRID_CENTER_CL);
		else
			FillRect(gridx, gridy, GRID_WIDTH, WD_HEIGHT, GRID_CL);
		gridx += GRID_DIST;
	}

	gridx = WD_OFFSETX;
	gridy = WD_OFFSETY;
	for(i = 0; i < yblk + 1;++i) {
		if(i == yblk/2)
			FillRect(gridx, gridy, WD_WIDTH, GRID_CENTER_WIDTH, GRID_CENTER_CL);
		else 
			FillRect(gridx, gridy, WD_WIDTH, GRID_WIDTH, GRID_CL);
		gridy += GRID_DIST;
	}
}

/* Display coursor */
void cursor_display(U16 posx, U16 posy, U8 cursor_type, U16 cursor_cl)
{
	clr_blk(posx, posy - 10, 8, 16 + 20);
	PutcGenic(posx, posy, cursor_type, cursor_cl, BG_CL, &ASC8X16);
}

/* Display timebase */
void timebase_display(U16 flag)
{
	if(!flag)
		return;
	
	U16 cl = (dso_scope.btn_selected == tb) ? SELECTED_CL : clGreen;
	BitClr(dso_scope.btns_flags, (1 << TB_BIT));

	clr_blk(TIMEBASE_OFFSETX, TIMEBASE_OFFSETY, 5 * 8, 12);
	if(dso_scope.timebase >= 1000){
		PutsGenic(TIMEBASE_OFFSETX + 8, TIMEBASE_OFFSETY, (U8 *)" ms", cl, clBlack, &ASC8X16);
		PutsGenic(TIMEBASE_OFFSETX, TIMEBASE_OFFSETY, (U8 *)itoa(dso_scope.timebase / 1000, buf, 10), cl, clBlack, &ASC8X16);
	} else {
		PutsGenic(TIMEBASE_OFFSETX + 17, TIMEBASE_OFFSETY, (U8 *)" us", cl, clBlack, &ASC8X16);
		PutsGenic(TIMEBASE_OFFSETX, TIMEBASE_OFFSETY, (U8 *)itoa(dso_scope.timebase, buf, 10), cl, clBlack, &ASC8X16);	
	}

	BitClr(dso_scope.btns_flags, (1 << PLUS_BTN_BIT));
	BitClr(dso_scope.btns_flags, (1 << MINUS_BTN_BIT));
	
}

/* Display any voltage */
void voltage_display(U16 posx, U16 posy, U8 *label, U16 adc_val, U16 text_clr, U16 bg_clr)
{
	U16 voltage = ( adc_val * 0.8);
	U16 label_s = strlen(label);
	U8 *ptr = buf;

	U8 v_buf[6] = {'0', '.', '0', '0', 'V', '\0'};

	if(voltage > 0) {
		itoa(voltage / 1000, buf, 10);
		v_buf[0] = buf[0];

		if(voltage > 1000)
			voltage %= 1000;
		voltage /= 10;

		/* Fractional part */
		itoa(voltage, buf, 10);
		ASSIGN_POS(v_buf[2], buf[0], ptr++);
		ASSIGN_POS(v_buf[3], buf[1], ptr);
	}

	/* Display */
	PutsGenic(posx, posy, label, text_clr, bg_clr, &ASC8X16);
	PutsGenic(posx + label_s * CHAR_WID, posy, v_buf, text_clr, bg_clr, &ASC8X16);
}

/* Display the previously calculated frequency */
void freq_display(double freq)
{
	clr_blk(FREQ_OFFSETX, FREQ_OFFSETY, FREQ_SIZE, 16);
	PutsGenic(FREQ_OFFSETX, FREQ_OFFSETY, (U8 *)"Freq:", TEXT_CL, BG_CL, &ASC8X16);
	if(!freq){
		PutcGenic(FREQ_OFFSETX + 5 * CHAR_WID, FREQ_OFFSETY, '-', TEXT_CL, BG_CL, &ASC8X16);	
		return;
	}
	U32 frq_int = freq * 100; /* Precision of two */

	U8 dig_buf[7];
	get_digits(frq_int, dig_buf);
	S8 dig_ind = strlen(dig_buf);

	U8 f_buf[9] = { '0', '0', '.', '0', '0', 'K', 'H', 'z', '\0'};

	U8 *ptr = f_buf;
	ASSIGN_POS(f_buf[4], dig_buf[0], dig_ind);
	ASSIGN_POS(f_buf[3], dig_buf[1], --dig_ind);
	ASSIGN_POS(f_buf[1], dig_buf[2], --dig_ind);
	ASSIGN_POS(f_buf[0], dig_buf[3], --dig_ind);

	PutsGenic(FREQ_OFFSETX + 5 * CHAR_WID, FREQ_OFFSETY, ptr, TEXT_CL, BG_CL, &ASC8X16);
}

void info_display(void)
{
	timebase_display(BitTest(dso_scope.btns_flags, (1 << TB_BIT)));
	/* Update peak-to-peak voltage */
	voltage_display(PPV_OFFSETX, PPV_OFFSETY, (U8 *)"Vpp:", (wave.max - wave.min + NOISE_MARGIN), TEXT_CL, BG_CL);
	/* Update max voltage */
	voltage_display(MAXV_OFFSETX, MAXV_OFFSETY, (U8 *)"Vmax:", (wave.max + NOISE_MARGIN), TEXT_CL, BG_CL);

	/* Display cursors */
	if(BitClrIfSet(dso_scope.btns_flags, (1 << LCURSOR_BIT)) || 
			BitClrIfSet(dso_scope.btns_flags, (1 << RCURSOR_BIT))) {
		cursor_display(CURSOR_LEFTX, wave.midpoint - 7, '>', 
			(dso_scope.btn_selected == l_cursor) ? SELECTED_CL : CURSOR_LEFT_CL);

		cursor_display(CURSOR_RIGHTX, wave.midpoint - GET_SAMPLE(dso_scope.trig_lvl_adc) - 6, '<', 
			(dso_scope.btn_selected == r_cursor) ? SELECTED_CL : CURSOR_RIGHT_CL);
	}

	/* Display frequency */
	if(!(--freq_delay)) {
		freq_display(wave.frequency);
		freq_delay = FREQ_DELAY;
	}
}

