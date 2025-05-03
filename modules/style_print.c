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
            if (*(++str) == '{') {
                putchar('{');
                str++;
            } else
                style(&str);
            continue;
        }
        putchar(*str);
        str++;
        count++;
    }
    putchar('\n');
    return count;
}

static void start_color_buffer(char *color_buf, char prefix);
static void set_hi_color(char *color_buf);
static void try_parse_color(char *color_buf, char clr, bool *control_flag);
static void try_parse_style(const char ch, bool *control_flag);

static void style(const char **s) {
    if (**s == '}')
        printf(ESC "0m");
    
    bool any_valid = false;
    int setted_colors = 0;
    while(**s > 0 && **s != '}') {
        if (setted_colors++ < 2 && (**s == 'f' || **s == 'b')) {
            char color_buf[4];
            start_color_buffer(color_buf, **s);
            
            (*s)++;
            
            if (**s == 'h') {
                set_hi_color(color_buf);
                (*s)++;
            }
            
            try_parse_color(color_buf, **s, &any_valid);
        }
        if (**s > 'A' && **s < 'Z') {
            try_parse_style(**s, &any_valid);
        }
        if (**s == ':') {
            if (any_valid) {
                putchar('m');
            }
            while(*((*s)+1) != '}' && *((*s)+1) != '\0') {
                (*s)++;
                putchar(**s);
            }
            printf(ESC "0");
        }
        (*s)++;
    }
    if (any_valid)
        putchar('m');
    if (**s != 0)
        (*s)++;
}

static void start_color_buffer(char *color_buf, char prefix) {
    color_buf[0] = '0';
    color_buf[1] = (prefix == 'b') ? '4' : '3';
    color_buf[3] = '\0';
}

static void set_hi_color(char *color_buf) {
    if (color_buf[1] == '4') {
        color_buf[1] = '0';
        color_buf[0] = '1';
    } else
        color_buf[1] = '9';
}

static void try_parse_color(char *color_buf, char clr, bool *control_flag) {
    if (islower(clr) && COLORS[ltonum(clr)]) {
        if (!*control_flag) {
            *control_flag = true;
            printf(ESC);
        }
        color_buf[2] = COLORS[ltonum(clr)];
        printf(";%s", color_buf);
    }
}

static void try_parse_style(const char ch, bool *control_flag) {
    const char *sty;
    if ((sty = STYLES[utonum(ch)])) {
        if (!*control_flag) {
            *control_flag = true;
            printf(ESC);
        }
        printf(";%s", sty);
    }
}
