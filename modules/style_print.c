#include "style_print.h"
#include <stdio.h>
#include <stdarg.h>

#define ESC "\033["

#define bool int
#define true 1
#define false 0

#define ltonum(ch) ch - 'a'
#define utonum(ch) ch - 'A'

#define islower(c) ((c) >= 'a' && (c) <= 'z')
#define isupper(c) ((c) >= 'A' && (c) <= 'Z')

enum Styles {
    BOLD          = utonum('B'),
    DIM           = utonum('D'),
    ITALIC        = utonum('I'),
    UNDERLINE     = utonum('U'),
    BLINKING      = utonum('K'),
    REVERSE       = utonum('R'),
    HIDDEN        = utonum('H'),
    STRIKETHROUGH = utonum('S'),
    D_UNDERLINE   = utonum('P'), //paired underline
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
    [DIM]           = "2",
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

static void style(const char **s);

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
    while(*str > 0) {
        if (*str == '{') {
            str++;
            if (*str == '{') {
                putchar('{');
                str++;
            }
            style(&str);
        }
        if (!*str)
            break;
        putchar(*str);
        str++;
        count++;
    }
    putchar('\n');
    return count;
}

static void style(const char **s) {
    if (**s == '}')
        printf(ESC "0m");
    
    int seted_color = 0;
    bool any_valid = false;
    char buf[4];
    buf[3] = '\0';
    while(**s > 0 && **s != '}') {
        if (seted_color < 2 && (**s == 'f' || **s == 'b')) {
            buf[0] = '0';
            buf[1] = (**s == 'b') ? '4' : '3';
            
            (*s)++;
            
            if (**s == 'h') {
                if (buf[1] == '4') {
                    buf[1] = '0';
                    buf[0] = '1';
                }
                buf[1] = (buf[1] == '4') ? '0' : '9';
                (*s)++;
            }
            
            char clr = islower(**s) ? COLORS[ltonum(**s)] : 0;
            
            if (clr) {
                if (!any_valid) {
                    any_valid = true;
                    printf(ESC);
                }
                buf[2] = clr;
                printf(";%s", buf);
            }
            seted_color++;
        }
        if (**s > 'A' && **s < 'Z') {
            const char *sty = STYLES[utonum(**s)];
            if (*sty) {
                if (!any_valid) {
                    any_valid = true;
                    printf(ESC);
                }
                printf(";%s", sty);
            }
        }
        (*s)++;
    }
    if (any_valid)
        putchar('m');
    if (**s != 0)
        (*s)++;
}
