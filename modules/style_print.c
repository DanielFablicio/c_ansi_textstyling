#include "style_print.h"
#include <stdio.h> //NULL, printf, vprintf, sprintf, putchar, va_list
#include <stdarg.h> //va_start, va_end
#include <string.h> //memset, strncpy
#include <stdbool.h> //bool, false, true
#include <ctype.h> //isdigit, isupper, toupper, islower
#include <stdlib.h> //strtol

#define uint8 unsigned char

#define isdigit_s(ch) isdigit((unsigned char)(ch))
#define isupper_s(ch) isupper((unsigned char)(ch))
#define toupper_s(ch) toupper((unsigned char)(ch))

#define ltonum(ch) ch - 'a'
#define utonum(ch) ch - 'A'

#define ESC "\033["

#define RESET_ALL "0m" ESC "K"
#define RESET_FG  "39m"
#define RESET_BG  "49m" ESC "K"

#define BUF_SZ (sizeof("00;0;000;000;000")-1)
#define BSC_COLOR_MAX_SZ (sizeof("hc")-1)
#define A256_COLOR_MAX_SZ (sizeof("$000")-1)
#define HEX_COLOR_MAX_SZ (sizeof("#000000")-1)


enum Styles {
    BOLD          = utonum('B'),
    FAINT         = utonum('F'), //dim
    ITALIC        = utonum('I'),
    UNDERLINE     = utonum('U'),
    BLINKING      = utonum('K'),
    REVERSE       = utonum('R'),
    HIDDEN        = utonum('H'),
    STRIKETHROUGH = utonum('S'),
    D_UNDERLINE   = utonum('D'),
    C_UNDERLINE   = utonum('C'),
    OVERLINE      = utonum('O'),
};

enum Colors {
    BLACK   = ltonum('d'), //dark
    RED     = ltonum('r'),
    GREEN   = ltonum('g'),
    YELLOW  = ltonum('y'),
    BLUE    = ltonum('b'),
    MAGENTA = ltonum('m'),
    CYAN    = ltonum('c'),
    WHITE   = ltonum('w')
};

const char STYLES[26][4] = {
    [BOLD]          = "1",
    [FAINT]         = "2",
    [ITALIC]        = "3",
    [UNDERLINE]     = "4",
    [BLINKING]      = "5",
    [REVERSE]       = "7",
    [HIDDEN]        = "8",
    [STRIKETHROUGH] = "9",
    [D_UNDERLINE]   = "21",
    [C_UNDERLINE]   = "4:3",
    [OVERLINE]      = "53",
};

const char COLORS[26] = {
    [BLACK] = '0',
    [RED] = '1',
    [GREEN] = '2',
    [YELLOW] = '3',
    [BLUE] = '4',
    [MAGENTA] = '5',
    [CYAN] = '6',
    [WHITE] = '7',
};

static void style(const char **ptrs);

int stypf(const char *restrict str, ...) {
    int count = 0;
    va_list args;

    va_start(args, str);
    vprintf(str, args);

    va_end(args);
    return count;
}

int styps(const char *str) {
    int count = 1; //\n
    while(*str != '\0') {
        unsigned char ch = *str;
        if (ch == '{' || ch == '}') {
            if (*(++str) == ch) {
                putchar(ch);
                str++;
            } else if (ch == '}') {
                ;
            } else
                style(&str);
            continue;
        }
        putchar(ch);
        str++;
        count++;
    }
    putchar('\n');
    return count;
}

static void style(const char **ptrs) {
    unsigned char ch;
    while((ch = **ptrs) != '\0' && ch != '}') {
        
    }
}
