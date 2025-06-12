#include "style_print.h"
#include <stdio.h> //NULL, printf, vprintf, sprintf, putchar, va_list
#include <stdarg.h> //va_start, va_end
#include <stdbool.h> //bool, false, true
#include <ctype.h> //isdigit, isupper, toupper, islower
#include <stdlib.h> //strtol
#include <string.h>

#ifdef _WIN32
    #include <io.h>
    #define isatty _isatty
    #define fileno _fileno
#else
    #include <unistd.h>
#endif

#define uint8 unsigned char

#define isdigit_s(ch) isdigit((unsigned char)(ch))
#define isupper_s(ch) isupper((unsigned char)(ch))
#define toupper_s(ch) toupper((unsigned char)(ch))
#define isalpha_s(ch) isalpha((unsigned char)(ch))

#define ltonum(ch) ch - 'a'
#define utonum(ch) ch - 'A'

#define ESC "\033["
// ESC K insure that background in strings with '\n' are propperly reseted
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
#define PRS_RESET_MAZ_SZ (sizeof("4:0")-1)

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
    WHITE   = ltonum('w'),

    //'z' == no_color. Can't be implemented
};

enum Resets {
    RESET_BOLD          = BOLD,
    RESET_FAINT         = FAINT, //dim
    RESET_ITALIC        = ITALIC,
    RESET_UNDERLINE     = UNDERLINE,
    RESET_BLINKING      = BLINKING,
    RESET_REVERSE       = REVERSE,
    RESET_HIDDEN        = HIDDEN,
    RESET_STRIKETHROUGH = STRIKETHROUGH,
    RESET_D_UNDERLINE   = D_UNDERLINE,
    RESET_C_UNDERLINE   = C_UNDERLINE,
    RESET_OVERLINE      = OVERLINE,
    
    RESET_FG_COLOR      = ltonum('f'),
    RESET_BG_COLOR      = ltonum('b'),
};

const char STYLES[26][3] = {
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

//only the style resets to reduce the size of the lookup array
const char STYLE_RESETS[26][3] = {
    [RESET_BOLD]          = "22",
    [RESET_FAINT]         = "22",
    [RESET_ITALIC]        = "23",
    [RESET_UNDERLINE]     = "24",
    [RESET_BLINKING]      = "25",
    [RESET_REVERSE]       = "27",
    [RESET_HIDDEN]        = "28",
    [RESET_STRIKETHROUGH] = "29",
    [RESET_D_UNDERLINE]   = "24",
    [RESET_C_UNDERLINE]   = "4:0",
    [RESET_OVERLINE]      = "55",
};

#define STYLE_MAX_SZ sizeof(STYLES[0])

enum States {
    READING,
    READING_BASIC_COLOR,
    READING_RGB_COLOR,
    READING_A256_COLOR,
    READING_STYLE,
    READING_RESETS,
    DISPATCH_COLOR,
    SKIP_STYLING,
    WRITE_STYLING,
    CHANGE_COLOR_GROUND,
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
    while (*str != '\0') {
        unsigned char ch = *str;
        if (ch == '{' || ch == '}') {
            str++;
            if (*str == ch) {
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

typedef struct StyleContext {
    State state;
    char srcbuf[SOURCE_STYLE_MAX_SZ+1];
    char prsbuf[PARSED_STYLE_MAX_SZ+1];
    int counter;
    bool first_style;
    int setted_colors;
    char color_ground;
    void (*write_callback)(struct StyleContext*);
    void (*end_style_callback)(struct StyleContext*);
} StyleContext;

static bool is_valid_style(char ch);
static bool is_valid_basic_color(char ch);
static bool is_valid_rgb_color(char ch);

static bool is_valid_a256_color(const char *srcbuf);

static void parse_style(const char *srcbuf, char *prsbuf);
static void parse_basic_color(const char *srcbuf, char *prsbuf, char ground);
static void parse_a256_color(const char *srcbuf, char *prsbuf, char ground);
static void parse_rgb_color(const char *srcbuf, char *prsbuf, char ground);

static void hand_parse_style(char ch, StyleContext *ctx);
static void hand_parse_basic_color(char ch, StyleContext *ctx);
static void hand_parse_a256_color(char ch, StyleContext *ctx);
static void hand_parse_rgb_color(char ch, StyleContext *ctx);

static void parse_reset(const char *srcbuf, char *prsbuf);
static void hand_parse_reset(char ch, StyleContext *ctx);

static void write_styling(const char *prsbuf, bool *fst_style_flag);
static void hand_write_styling(StyleContext *ctx);

static bool dispatch_color(State dispatch_to, StyleContext *ctx);
static void reset_context_aux_variables(StyleContext *ctx);

//Callback Functions
static void clbk_increment_setted_colors(StyleContext *ctx);
static void clbk_change_state_to_reading_resets(StyleContext *ctx);
static void clbk_finish_reset_with_erase_in_line(StyleContext *ctx);

static void reset_context_aux_variables(StyleContext *ctx) {
    ctx->counter = 0;
    ctx->write_callback = NULL;
}

static void clbk_increment_setted_colors(StyleContext *ctx) {
    ctx->setted_colors++;
    ctx->color_ground = BACKGROUND;
}

static void clbk_change_state_to_reading_resets(StyleContext *ctx) {
    ctx->state = READING_RESETS;
}

static void clbk_finish_reset_with_erase_in_line(StyleContext *ctx) {
    printf("\033[K");
}

#define DEBUG

static void style(const char **ptrs) {
    bool is_atty = isatty(fileno(stdout));
#ifdef DEBUG
    is_atty = true;
#endif

    if (**ptrs == '}' && is_atty) {
        printf(ESC RESET_ALL);
        return;
    }
    
    StyleContext ctx = {
        .state = is_atty ? READING : SKIP_STYLING,
        .srcbuf = "",
        .prsbuf = "",
        .counter = 0,
        .first_style = true,
        .setted_colors = 0,
        .color_ground = FOREGROUND,
        .write_callback = NULL,
        .end_style_callback = NULL,
    };

    State dispatch_to;
    
    unsigned char ch;
    while ((ch = **ptrs) != '}' && ch != '\0') {
        switch (ctx.state) {
            case READING:
                reset_context_aux_variables(&ctx);

                bool skip_char = true; //skip the symbol of special stylings
                switch (ch) {
                    case '$':
                        dispatch_to = READING_A256_COLOR;
                        ctx.state = DISPATCH_COLOR;
                        break;
                    case '#':
                        dispatch_to = READING_RGB_COLOR;
                        ctx.state = DISPATCH_COLOR;
                        break;
                    case '-':
                        ctx.state = READING_RESETS;
                        break;
                    default:
                        skip_char = false;

                        if (isupper(ch)) {
                            ctx.state = READING_STYLE;
                            break;
                        }

                        if (ch == '_') {
                            ctx.state = CHANGE_COLOR_GROUND;
                            break;
                        }

                        if (islower(ch)) {
                            dispatch_to = READING_BASIC_COLOR;
                            ctx.state = DISPATCH_COLOR;
                        }
                }

                if (ctx.state != READING && !skip_char)
                    continue;
                break;
            case READING_STYLE:
                hand_parse_style(ch, &ctx);
                break;
            case DISPATCH_COLOR:
                if (!dispatch_color(dispatch_to, &ctx))
                    break;
                continue;
            case READING_BASIC_COLOR:
                hand_parse_basic_color(ch, &ctx);
                break;
            case READING_A256_COLOR:
                hand_parse_a256_color(ch, &ctx);
                break;
            case READING_RGB_COLOR:
                hand_parse_rgb_color(ch, &ctx);
                break;
            case READING_RESETS:
                hand_parse_reset(ch, &ctx);
                break;
            case WRITE_STYLING:
                hand_write_styling(&ctx);
                break;
            case SKIP_STYLING:
                break;
            case CHANGE_COLOR_GROUND:
                clbk_increment_setted_colors(&ctx);
                ctx.state = READING;
                break;
        }

        if (ctx.state != WRITE_STYLING)
            (*ptrs)++;
    }

    if (!ctx.first_style) {
        putchar('m');
    }

    if (ctx.end_style_callback) {
        ctx.end_style_callback(&ctx);
    }
}

static bool dispatch_color(State dispatch_to, StyleContext *ctx) {
    if (ctx->setted_colors == 2) {
        ctx->state = READING;
        return false;
    }

    ctx->write_callback = clbk_increment_setted_colors;
    ctx->state = dispatch_to;
    return true;
}

inline bool is_valid_style(char ch) {
    return STYLES[utonum(ch)];
}

inline bool is_valid_basic_color(char ch) {
    return COLORS[ltonum(ch)];
}

inline bool is_valid_rgb_color(char ch) {
    ch = toupper_s(ch);
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F');
}

inline bool is_valid_a256_color(const char *srcbuf) {
    bool only_zeros = strcmp(srcbuf, "000") == 0;
    return (strtol(srcbuf, NULL, 10) <= 255) || only_zeros;
}

inline bool is_valid_reset(char ch) {
    return (isupper_s(ch) && STYLE_RESETS[utonum(ch)]) || ch == 'f' || ch == 'b';
}

void parse_style(const char *srcbuf, char *prsbuf) {
    const char *sty = STYLES[utonum(srcbuf[0])];
    snprintf(prsbuf, STYLE_MAX_SZ + 1, "%.3s", sty);
}

void parse_basic_color(const char *srcbuf, char *prsbuf, char ground) {
    uint8 clr = (ground == FOREGROUND) ? 30 : 40;
    int count = 0;
    if (srcbuf[0] == HI_COLOR_PREFIX) {
        clr += 60;
        count++;
    }
    clr += COLORS[ltonum(srcbuf[count])] - '0';
    snprintf(prsbuf, PRS_BSC_MASX_SZ + 1, "%d", clr);
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

void parse_reset(const char *srcbuf, char *prsbuf) {
    char ch = srcbuf[0];
    char sty[PRS_RESET_MAZ_SZ] = {0};
    
    switch (ch) {
        case 'f':
            strcpy(sty, "39");
            break;
        case 'b':
            strcpy(sty, "49");
            break;
        default:
            strcpy(sty, STYLE_RESETS[utonum(ch)]);
    }

    snprintf(prsbuf, PRS_RESET_MAZ_SZ + 1, "%.3s", sty);
}

void hand_parse_style(char ch, StyleContext *ctx) {
    if (!is_valid_style(ch)) {
        ctx->state = READING;
        return;
    }
    
    ctx->state = WRITE_STYLING;
    
    ctx->srcbuf[0] = ch;
    ctx->srcbuf[1] = '\0';

    parse_style(ctx->srcbuf, ctx->prsbuf);
}

void hand_parse_basic_color(char ch, StyleContext *ctx) {
    if (ch != HI_COLOR_PREFIX || ctx->counter == 1) {
        if (!is_valid_basic_color(ch)) {
            ctx->state = READING;
            return;
        }
    }
    
    ctx->srcbuf[ctx->counter++] = ch;
    
    if (ch == HI_COLOR_PREFIX) {
        return;
    }
    
    ctx->state = WRITE_STYLING;
    
    ctx->srcbuf[ctx->counter] = '\0';
    parse_basic_color(ctx->srcbuf, ctx->prsbuf, ctx->color_ground);
}

void hand_parse_a256_color(char ch, StyleContext *ctx) {
    if (!isdigit_s(ch)) {
        ctx->state = READING;
        return;
    }

    ctx->srcbuf[ctx->counter++] = ch;
    if (ctx->counter != A256_COLOR_MAX_SZ - 1) {
        return;
    }

    ctx->srcbuf[ctx->counter] = '\0';
    
    if (!is_valid_a256_color(ctx->srcbuf)) {
        ctx->state = READING;
        return;
    }

    ctx->state = WRITE_STYLING;

    parse_a256_color(ctx->srcbuf, ctx->prsbuf, ctx->color_ground);
}

void hand_parse_rgb_color(char ch, StyleContext *ctx) {
    if (!is_valid_rgb_color(ch)) {
        ctx->state = READING;
        return;
    }

    ctx->srcbuf[ctx->counter++] = ch;
    if (ctx->counter != RGB_COLOR_MAX_SZ-1) //-#
        return;

    ctx->state = WRITE_STYLING;
    
    ctx->srcbuf[ctx->counter] = '\0';
    parse_rgb_color(ctx->srcbuf, ctx->prsbuf, ctx->color_ground);
}

void hand_parse_reset(char ch, StyleContext *ctx) {
    if (!is_valid_reset(ch)) {
        return; // purposely mantains the READING_RESETS state
    }

    ctx->state = WRITE_STYLING;
    ctx->write_callback = clbk_change_state_to_reading_resets;

    if (ch == 'b') {
        ctx->end_style_callback = clbk_finish_reset_with_erase_in_line;
    }
    
    ctx->srcbuf[0] = ch;
    ctx->srcbuf[1] = '\0';

    parse_reset(ctx->srcbuf, ctx->prsbuf);
}

void write_styling(const char *prsbuf, bool *fst_style_flag) {
    if (*fst_style_flag) {
        *fst_style_flag = false;
        printf(ESC);
    }
    printf("%s;", prsbuf);
}

void hand_write_styling(StyleContext *ctx) {
    write_styling(ctx->prsbuf, &ctx->first_style);
    ctx->state = READING;
    
    if (ctx->write_callback != NULL) {
        ctx->write_callback(ctx);
    }
}
