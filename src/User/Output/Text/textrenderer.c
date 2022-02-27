#include "User/Output/Text/textrenderer.h"
#include "kernelpanic.h"


static bmpfont_t* font;

static uint32_t numColumns;
static uint32_t numRows;

static uint32_t foregroundColor;
static uint32_t backgroundColor;

static point_t startPosition;
static point_t endPosition;
static point_t curPosition;

static bool_t  trendrInitialized;


static void __initialize__(uint32_t __color, uint32_t __backcolor, uint64_t __xoff, uint64_t __yoff) {
    uint64_t _buff_w = kernel_kdd_fbo_get()->_Width;
    uint64_t _buff_h = kernel_kdd_fbo_get()->_Height;

    foregroundColor = __color;
    backgroundColor = __backcolor;
    
    startPosition = (point_t) {
        .x = __xoff,
        .y = __yoff
    };

    endPosition = (point_t) {
        .x = _buff_w - (_buff_w %  8),
        .y = _buff_h - (_buff_h % 16)
    };

    curPosition = startPosition;

    numColumns = endPosition.x / 8;
    numRows    = endPosition.y / 16;

    kernel_kdd_fbo_clear(backgroundColor);
}

static void __putchar__(char __char) {
    char* _font_ptr = font->_Buffer + (__char * font->_Header->_CharSize);

    uint64_t _start_x = curPosition.x;
    uint64_t _start_y = curPosition.y;

    uint64_t _end_x = _start_x + 8,
             _end_y = _start_y + 16;

    for (uint64_t _y = _start_y; _y < _end_y; _y++) {
        for (uint64_t _x = _start_x; _x < _end_x; _x++) {
            kernel_kdd_pxcolor_set(
                _x, _y, 
                ((*_font_ptr & ((1 << 7) >> (_x - _start_x))))
                    ? foregroundColor : backgroundColor
            );
        }

        _font_ptr++;
    }
}

static void __newline__() {
    curPosition.x  = startPosition.x;
    curPosition.y += 16;

    SOFTASSERT(!(curPosition.y < endPosition.y), RETVOID);

    kernel_text_scroll_up(1);
}

void kernel_text_scroll_up(uint32_t __lines) {
    SOFTASSERT(trendrInitialized, RETVOID);
    SOFTASSERT(__lines != 0, RETVOID);
    SOFTASSERT(__lines < numRows, RETVOID);
    
    for (uint64_t _y = __lines * 16; _y < endPosition.y; _y++) {
        for (uint64_t _x = startPosition.x; _x < kernel_kdd_fbo_get()->_PixelsPerScanLine; _x++) {
            kernel_kdd_pxcolor_set(
                _x, _y - __lines * 16,
                kernel_kdd_pxcolor_get(_x, _y)
            );
        }
    }

    for (uint64_t _l = 0; _l < __lines; _l++)
        kernel_text_line_clear(curPosition.y / 16 - _l);

    curPosition.y -= 16 * __lines;
}

void kernel_text_line_clear(uint32_t __line) {
    SOFTASSERT(trendrInitialized, RETVOID);

    for (uint64_t _y = __line * 16 - 16; _y < __line * 16; _y++) {
        for (uint64_t _x = startPosition.x; _x < endPosition.x; _x++) {
            kernel_kdd_pxcolor_set(
                _x, _y,
                backgroundColor
            );
        }
    }
}

void kernel_text_print_char(char __char) {
    SOFTASSERT(trendrInitialized, RETVOID);
    SOFTASSERT(__char != 0, RETVOID);

    switch (__char) {
        case '\n': __newline__(); return;
        case '\t': return;
    }

    if (curPosition.x == endPosition.x)
        __newline__();
    
    __putchar__(__char);
    curPosition.x += 8;
}

void kernel_text_print_str(char* __str) {
    SOFTASSERT(trendrInitialized, RETVOID);
    
    char* _chr = __str;

    while (*_chr != 0) {
        kernel_text_print_char(*_chr);
        *_chr++;
    }
}

void kernel_text_info_set(uint32_t __color, uint32_t __backcolor) {
    SOFTASSERT(trendrInitialized, RETVOID);
    
    foregroundColor = __color;
    backgroundColor = __backcolor; 
}

void kernel_text_initialize(uint32_t __color, uint32_t __backcolor, uint32_t __xoff, uint32_t __yoff, bmpfont_t* __font) {
    SOFTASSERT(!trendrInitialized, RETVOID);
    
    __initialize__(__color, __backcolor, __xoff, __yoff);
    font = __font;

    trendrInitialized = TRUE;
}

void kernel_text_reinitialize(uint32_t __color, uint32_t __backcolor, uint32_t __xoff, uint32_t __yoff) {
    SOFTASSERT(trendrInitialized, RETVOID);
    __initialize__(__color, __backcolor, __xoff, __yoff);
}

void kernel_text_info_get(uint32_t* __color, uint32_t* __backcolor, uint32_t* __xoff, uint32_t* __yoff, bmpfont_t** __font) {
    SOFTASSERT(trendrInitialized, RETVOID);
    
    ARGRET(__color, foregroundColor);
    ARGRET(__backcolor, backgroundColor);
    
    ARGRET(__xoff, curPosition.x);
    ARGRET(__yoff, curPosition.y);
    ARGRET(__font, font);
}
