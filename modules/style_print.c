#include "style_print.h"
#include <stdio.h> //NULL, printf, vprintf, sprintf, putchar, va_list
#include <stdarg.h> //va_start, va_end
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

#define BSC_COLOR_MAX_SZ (sizeof("hc")-1)
#define A256_COLOR_MAX_SZ (sizeof("$255")-1)
#define RGB_COLOR_MAX_SZ (sizeof("#FFFFFF")-1)

//parsed
#define PRS_BSC_MASX_SZ (sizeof("107")-1)
#define PRS_A256_MAX_SZ (sizeof("38;5;255")-1)
#define PRS_RGB_MAX_SZ (sizeof("38;2;255;255;255")-1)

#define SOURCE_STYLE_MAX_SZ RGB_COLOR_MAX_SZ
#define PARSED_STYLE_MAX_SZ PRS_RGB_MAX_SZ

#define FOREGROUND '3'
#define BACKGROUND '4'
#define HI_FOREGROUND '9'
#define HI_BACKGROUND '0'

#define HI_COLOR_PREFIX 'h'

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

#define STYLE_MAX_SZ (sizeof(STYLES[0])-1)

enum States {
    READING,
    READING_BASIC_COLOR,
    READING_RGB_COLOR,
    READING_A256_COLOR,
    READING_STYLE,
    SKIP_UNTIL_CLOSE,
    SKIP_PRINT_UNTIL_CLOSE,
    WRITE_STYLING,
};

typedef enum States State;

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

typedef struct {
    State state;
    char srcbuf[SOURCE_STYLE_MAX_SZ+1];
    char prsbuf[PARSED_STYLE_MAX_SZ+1];
    bool first_style;
    int counter;
    int setted_colors;
} ContextVariables;

static bool is_valid_style(char ch);
static bool is_valid_basic_color(char ch);
static bool is_valid_a256_color(char ch);
static bool is_valid_rgb_color(char ch);

static void parse_style(const char *srcbuf, char *prsbuf);
static void parse_basic_color(const char *srcbuf, char *prsbuf, char ground);
static void parse_a256_color(const char *srcbuf, char *prsbuf, char ground);
static void parse_rgb_color(const char *srcbuf, char *prsbuf, char ground);

static void hand_parse_style(char ch, ContextVariables *ctx);
static void hand_parse_basic_color(char ch, ContextVariables *ctx);
static void hand_parse_a256_color(char ch, ContextVariables *ctx);
static void hand_parse_rgb_color(char ch, ContextVariables *ctx);

static void write_styling(const char *prsbuf, bool *fst_style_flag);

static void style(const char **ptrs) {
    ContextVariables ctx = {
        .state = READING,
        .srcbuf = "\0",
        .prsbuf = "\0",
        .first_style = true,
        .counter = 0,
        .setted_colors = 0,
    };

    unsigned char ch;
    while ((ch = **ptrs) != '}' && ch != '\0') {
        switch (ctx.state) {
            case READING:
                if (isupper(ch))
                    ctx.state = READING_STYLE;
                if (ctx.setted_colors < 2) {
                    if (islower(ch))
                        ctx.state = READING_BASIC_COLOR;
                    if (ch == '$')
                        ctx.state = READING_A256_COLOR;
                    if (ch == '#')
                        ctx.state = READING_RGB_COLOR;
                    ctx.setted_colors++;
                }
                if (ch == '_')
                    ctx.state = SKIP_UNTIL_CLOSE;
                if (ch == ':')
                    ctx.state = SKIP_PRINT_UNTIL_CLOSE;
                break;
            case READING_STYLE:
                hand_parse_style(ch, &ctx);
                break;
            case READING_BASIC_COLOR:
                hand_parse_basic_color(ch, &ctx);
                break;
            case READING_A256_COLOR:
                hand_parse_a256_color(ch, &ctx);
                break;
            case READING_RGB_COLOR:
                hand_parse_rgb_color(ch, &ctx);
                break;
            case SKIP_UNTIL_CLOSE:
                //holds the state here
                break;
            case SKIP_PRINT_UNTIL_CLOSE:
                putchar(ch);
                break;
            case WRITE_STYLING:
                write_styling(ctx.srcbuf, &ctx.first_style);
                ctx.state = READING;
                break;
        }

        if (ctx.state == READING) {
            ctx.counter = 0;
        }
        
        if (ctx.state != WRITE_STYLING) {
            (*ptrs)++;
        }
    }

    if (!ctx.first_style)
        putchar('m');
}

inline bool is_valid_style(char ch) {
    return STYLES[utonum(ch)];
}

inline bool is_valid_basic_color(char ch) {
    return COLORS[ltonum(ch)];
}

inline bool is_valid_rgb_color(char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F');
}

inline bool is_valid_a256_color(char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F');
}

void parse_style(const char *srcbuf, char *prsbuf) {
    const char *sty = STYLES[utonum(srcbuf[0])];
    snprintf(prsbuf, STYLE_MAX_SZ + 1, "%s", sty);
}

void parse_basic_color(const char *srcbuf, char *prsbuf, char ground) {
    uint8 clr = ground == FOREGROUND ? 30 : 40;
    int count = 0;
    if (srcbuf[0] == HI_COLOR_PREFIX) {
        clr += 60;
        count++;
    }
    clr += COLORS[ltonum(srcbuf[count])] - '0';
    snprintf(prsbuf, PRS_BSC_MASX_SZ + 1,"%d", clr);
}

void parse_a256_color(const char *srcbuf, char *prsbuf, char ground) {
    uint8 clr = strtol(srcbuf, NULL, 10);
    snprintf(prsbuf, PRS_A256_MAX_SZ + 1, "%c8;5;%d", ground, clr);
}

void parse_rgb_color(const char *srcbuf, char *prsbuf, char ground) {
    char cr[3] = {srcbuf[0], srcbuf[1]};
    char cg[3] = {srcbuf[2], srcbuf[3]};
    char cb[3] = {srcbuf[4], srcbuf[5]};

    uint8 r = strtol(cr, NULL, 16);
    uint8 g = strtol(cg, NULL, 16);
    uint8 b = strtol(cb, NULL, 16);

    snprintf(prsbuf, PRS_RGB_MAX_SZ + 1, "%c8;2;%d;%d;%d", ground, r, g, b);
}

void hand_parse_style(char ch, ContextVariables *ctx) {
    if (!is_valid_style(ch)) {
        ctx->state = READING;
        return;
    }
    
    ctx->srcbuf[0] = ch;
    ctx->srcbuf[1] = '\0';
    parse_style(ctx->srcbuf, ctx->prsbuf);
    ctx->state = WRITE_STYLING;
}

void hand_parse_basic_color(char ch, ContextVariables *ctx) {
    if (ch != HI_COLOR_PREFIX && !is_valid_basic_color(ch)) {
        ctx->state = READING;
        return;
    }
    
    ctx->srcbuf[ctx->counter++] = ch;
    
    if (ch == HI_COLOR_PREFIX)
        return;
    
    ctx->state = WRITE_STYLING;
    ctx->srcbuf[ctx->counter] = '\0';
    char ground = !ctx->setted_colors ? FOREGROUND : BACKGROUND;
    parse_basic_color(ctx->srcbuf, ctx->prsbuf, ground);
}

void hand_parse_a256_color(char ch, ContextVariables *ctx) {
    if (!isdigit((unsigned)ch)) {
        if (ctx->counter == 0 && is_valid_a256_color(ch))
            ;
        //is_valid_a256_color(srcbuf)
    }

    ctx->state = WRITE_STYLING;
    ctx->srcbuf[ctx->counter] = '\0';
    char ground = !ctx->setted_colors ? FOREGROUND : BACKGROUND;
    parse_a256_color(ctx->srcbuf, ctx->prsbuf, ground);
}

void hand_parse_rgb_color(char ch, ContextVariables *ctx) {
    if (!is_valid_rgb_color(ch)) {
        ctx->state = READING;
        return;
    }

    ctx->srcbuf[ctx->counter++] = ch;
    if (ctx->counter != RGB_COLOR_MAX_SZ-1) //-#
        return;

    ctx->state = WRITE_STYLING;
    ctx->srcbuf[ctx->counter] = '\0';
    char ground = !ctx->setted_colors ? FOREGROUND : BACKGROUND;
    parse_rgb_color(ctx->srcbuf, ctx->prsbuf, ground);
}

void write_styling(const char *prsbuf, bool *fst_style_flag) {
    if (*fst_style_flag) {
        *fst_style_flag = false;
        printf(ESC);
    }
    printf("%s;", prsbuf);
}
