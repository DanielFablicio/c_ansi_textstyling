#include "style_print.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define isupper_s(ch) isupper((unsigned char)(ch))
#define toupper_s(ch) toupper((unsigned char)(ch))

#define ltonum(ch) ch - 'a'
#define utonum(ch) ch - 'A'

#define ESC "\033["

#define RESET_ALL "0m"
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


typedef struct {
    int setted_colors;
} ControlLimits;

static void hextorgb(const char *hex, char *out_rgb);
static bool is_valid_hex_color(const char *str);
static bool is_valid_color(const char *str);
static bool is_valid_style(char ch);
static int handle_styling_parse(const char **pts, char *stybuf,
                                 ControlLimits *cl);
static void handle_styling_reset(const char **pts);
static int parse_style(char ch, char *stybuf);
static void parse_basic_color(const char *slc, char *color_buf, char ground);
static void parse_rgb_color(const char *slc, char *color_buf, char ground);
static int parse_color(char *slc, char *stybuf, ControlLimits *cl);
static void skip_until_close(const char **pts);

static void style(const char **pts) {
    if (**pts == '}' || **pts == '_') {
        handle_styling_reset(pts);
        return;
    }
    char stybuf[BUF_SZ+1];

    ControlLimits cl = {
        .setted_colors = 0,
    };

    bool any_style_valid = false;
    bool auto_reset_syntax = false;

    while (**pts != '}' && **pts != '\0') {
        memset(stybuf, 0, BUF_SZ+1);
        int offset = handle_styling_parse(pts, stybuf, &cl);
        if (stybuf[0]) {
            if (!any_style_valid) {
                any_style_valid = true;
                printf(ESC);
            }
            printf("%s;", stybuf);
        }
        if (**pts == ':') {
            auto_reset_syntax = true;
            (*pts)++;
            break;
        }
        (*pts) += offset;
    }
    if (auto_reset_syntax) {
        skip_until_close(pts);
        handle_styling_reset(pts);
    }
    if (any_style_valid)
        putchar('m');
}



static int handle_styling_parse(const char **pts, char *stybuf,
                                 ControlLimits *cl)
{
    int offset = 1;
    if (is_valid_style(**pts)) {
        offset = parse_style(**pts, stybuf);
    }
    
    char color_slc[HEX_COLOR_MAX_SZ+1] = {0};
    if (is_valid_color(*pts)) {
        strncpy(color_slc, *pts, HEX_COLOR_MAX_SZ);
        offset = parse_color(color_slc, stybuf, cl);
    }
    return offset;
}

static void handle_styling_reset(const char **pts) {
    const char *reset = RESET_ALL;
    unsigned char ch;
    if (**pts == '_'){
        char next_ch = *++(*pts);

        if (next_ch == 'f') reset = RESET_FG;
        if (next_ch == 'b') reset = RESET_BG;
    }
    if (**pts != '\0') (*pts)++;
    printf(ESC"%s", reset);
}

static int parse_style(char ch, char *stybuf) {
    const int offset = 1;
    snprintf(stybuf, BUF_SZ, "%s", STYLES[utonum(ch)]);
    return offset;
}

static int parse_color(char *slc, char *stybuf, ControlLimits *cl) {
    if (cl->setted_colors >= 2) return 1;

    int offset = 1;
    char ground = cl->setted_colors == 0 ? 'f' : 'b';
    char color_buf[BUF_SZ+1] = {0}; // # -> \n

    unsigned char ch = slc[0];
    if (ch == '#') {
        offset = HEX_COLOR_MAX_SZ;
        parse_rgb_color(slc+1, color_buf, ground);
    } else {
        parse_basic_color(slc, color_buf, ground);
    }
    snprintf(stybuf, BUF_SZ-1, "%s", color_buf);
    cl->setted_colors++;
    return offset;
}

static void hextorgb(const char *hex, char *out_rgb) {
    
}

static void parse_basic_color(const char *slc, char *color_buf, char ground) {
    char hi_color = slc[0] == 'h' ? '1' : '0';
    char chosen_ground = ground == 'f'
                            ? (hi_color == '1' ? '9' : '3')
                            : (hi_color == '1' ? '0' : '4');
    char color = COLORS[ltonum(slc[hi_color == '1' ? 1 : 0])];

    color_buf[0] = hi_color;
    color_buf[1] = chosen_ground;
    color_buf[2] = color;
}

static void parse_rgb_color(const char *slc, char *color_buf, char ground) {
    
}

static void skip_until_close(const char **pts) {
    while(**pts != '\0' && **pts != '}')
        putchar(*((*pts)++));
}

static bool is_valid_hex_color(const char *str) {
    for (int i = 0; str[i] != '\0' && i < 6; i++) {
        unsigned char ch = toupper_s(str[i]);
        if ((ch < 'A' || ch > 'F') && (ch < '0' || ch > '9'))
            return false;
    }
    return true;
}

static bool is_valid_color(const char *str) {
    bool res = false;
    unsigned char ch = *str;
    
    if (islower(ch)) {
        if (ch == 'h')
            ch = *(str+1);
        if (COLORS[ltonum(ch)])
            res = true;
    }
    
    if (ch == '#') {
        res = is_valid_hex_color(str+1);
    }

    return res;
}

static bool is_valid_style(char ch) {
    return isupper_s(ch) && STYLES[utonum(ch)];
}
