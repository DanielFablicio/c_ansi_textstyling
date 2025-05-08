#include "style_print.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define bool int
#define true 1
#define false 0

#define ltonum(ch) ch - 'a'
#define utonum(ch) ch - 'A'

#define islower(c) ((c) >= 'a' && (c) <= 'z')
#define isupper(c) ((c) >= 'A' && (c) <= 'Z')

#define ESC "\033["

#define RESET_ALL "0m"
#define RESET_FG  "39m"
#define RESET_BG  "49m" ESC "K"

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
    while(*str != '\0') {
        unsigned char ch = *str;
        if (ch == '{' && ch == '}') {
            if (*(++str) == ch) {
                putchar(ch);
                str++;
            } else if (ch == '}') {
                str++;
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

#define BUF_SZ sizeof("00;0;000;000;000")

typedef struct {
    int setted_colors;
} ControlLimits;

static void handle_styling_parse(const char **s, char stybuf[],
                                 ControlLimits *cl);
static void handle_styling_reset(const char **s);
static void parse_style(char ch, char stybuf[]);
static void parse_color(char slc[], char stybuf[], ControlLimits *cl);
static void skip_until_close(const char **s);

static void style(const char **s) {
    if (**s == '}' || **s == '_') {
        handle_styling_reset(s);
        return;
    }
    char stybuf[BUF_SZ] = {0};

    ControlLimits cl = {
        .setted_colors = 0,
    };

    bool any_style_valid = false;
    bool auto_reset_syntax = false;

    while(**s != '}' && **s != '\0') {
        stybuf[0] = '\0';
        handle_styling_parse(s, stybuf, &cl);
        if (stybuf[0]) {
            if (!any_style_valid) {
                any_style_valid = true;
                printf(ESC);
            }
            printf("%s;", stybuf);
        }
        if (**s == ':') {
            auto_reset_syntax = true;
            (*s)++;
            break;
        }
    }
    if (auto_reset_syntax) {
        skip_until_close(s);
        handle_styling_reset(s);
    }
    if (any_style_valid)
        putchar('m');
}

static bool is_valid_color(char slc[]) {
    return slc[0] >= 'A' && slc[0] <= 'Z' && STYLES[ltonum(slc[0])];
}
static bool is_valid_style(char ch) {
    return ch >= 'a' && ch <= 'z' && STYLES[utonum(ch)];
}

static void handle_styling_parse(const char **s, char stybuf[],
                                 ControlLimits *cl)
{
    #define SLC_MAX_SZ sizeof("#000000")
    char slc[SLC_MAX_SZ] = {**s};

    if (is_valid_style(slc[0])) {
        parse_style(slc[0], stybuf);
    }
    if (is_valid_color(slc)) {
        strncpy(slc, *s, SLC_MAX_SZ);
        parse_color(slc, stybuf, cl);
    }
}

static void handle_styling_reset(const char **s) {
    const char *reset = RESET_ALL;
    if (**s == '_'){
        char next_ch = *++(*s);
        if (next_ch == 'f')
            reset = RESET_FG;
        if (next_ch == 'b')
            reset = RESET_BG;
    }
    if (**s != '\0') (*s)++;
    printf(ESC"%s", reset);
}

static void parse_style(char ch, char stybuf[]) {
    snprintf(stybuf, BUF_SZ-1, "%s", STYLES[utonum(ch)]);
}

static void parse_color(char slc[], char stybuf[], ControlLimits *cl) {
    if (cl->setted_colors++ == 2) return;
    bool is_foreground = cl->setted_colors == 0;

    char hi_color = slc[0];
    char ground = is_foreground
                    ? (hi_color == '1' ? '9' : '3')
                    : (hi_color == '1' ? '0' : '4');

    char color = slc[2];

    char color_buf[3+1] = {
        hi_color,
        ground,
        color
    };
    snprintf(stybuf, BUF_SZ-1, "%s", color_buf);
}

static void skip_until_close(const char **s) {
    while(**s != '\0' && **s != '}')
        putchar(*((*s)++));
}
