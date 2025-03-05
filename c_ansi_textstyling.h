#ifndef C_ANSI_TEXT_STYLING_H
#define C_ANSI_TEXT_STYLING_H
/*
 * This header defines text style macros using ANSI escape codes with
 * SGR (Select Graphic Rendition) parameters.
 *
 * There are two ways to apply styles:
 *
 * 1. ESCAPE CODE MACROS (_E SUFFIX)
 *    Use macros ending in _E before the string and a reset macro afterward
 *    when necessary.
 *
 *    Example: printf(BOLD_E FG_RED_E "Hello" RESET);
 *
 * 2. FUNCTION MACROS (CONCISE)
 *    Use function-like macros (at the end of this file) to style text,
 *    which automatically apply and reset styles. [At the end of the file]
 *
 *    Example: printf(BOLD(FG_RED("Hello")));
 *
 * -----------------------------------------------------------------------------
 * NOTES
 *   1. Not all terminals support all styles.
 *   2. The bold and dim styles, as well as underline and double underline, are
 *      reset using the same escape code. Therefore, resetting one also resets
 *      the other.
 * -----------------------------------------------------------------------------
 *
 * More information and examples:
 * GitHub: https://github.com/DanielFablicio/c_ansi_text_styling
 */

//Reset all styles. Specific restarts are from line 89.
#define   RESET                   "\033[0m"

//Styles----------------------------------------------------------
#define   BOLD_E                  "\033[1m"
#define   DIM_E                   "\033[2m"
#define   ITALIC_E                "\033[3m"
#define   UNDERLINE_E             "\033[4m"
#define   BLINKING_E              "\033[5m"
#define   REVERSE_E               "\033[7m"
#define   HIDDEN_E                "\033[8m"
#define   STRIKETHROUGH_E         "\033[9m"
#define   DOUBLE_UNDERLINE_E      "\033[21m"
#define   CURLY_UNDERLINE_E       "\033[4:3m"
#define   OVERLINE_E              "\033[53m"
//----------------------------------------------------------------

//Text/Foreground Colors------------------------------------------
#define   FG_BLACK_E              "\033[30m"
#define   FG_RED_E                "\033[31m"
#define   FG_GREEN_E              "\033[32m"
#define   FG_YELLOW_E             "\033[33m"
#define   FG_BLUE_E               "\033[34m"
#define   FG_MAGENTA_E            "\033[35m"
#define   FG_CYAN_E               "\033[36m"
#define   FG_WHITE_E              "\033[37m"
//High Intensity / Bright-----------------------------------------
#define   FG_HI_BLACK_E           "\033[90m"
#define   FG_HI_RED_E             "\033[91m"
#define   FG_HI_GREEN_E           "\033[92m"
#define   FG_HI_YELLOW_E          "\033[93m"
#define   FG_HI_BLUE_E            "\033[94m"
#define   FG_HI_MAGENTA_E         "\033[95m"
#define   FG_HI_CYAN_E            "\033[96m"
#define   FG_HI_WHITE_E           "\033[97m"
//----------------------------------------------------------------

//Background Colors-----------------------------------------------
#define   BG_BLACK_E              "\033[40m"
#define   BG_RED_E                "\033[41m"
#define   BG_GREEN_E              "\033[42m"
#define   BG_YELLOW_E             "\033[43m"
#define   BG_BLUE_E               "\033[44m"
#define   BG_MAGENTA_E            "\033[45m"
#define   BG_CYAN_E               "\033[46m"
#define   BG_WHITE_E              "\033[47m"
//High Intensity / Bright-----------------------------------------
#define   BG_HI_BLACK_E           "\033[100m"
#define   BG_HI_RED_E             "\033[101m"
#define   BG_HI_GREEN_E           "\033[102m"
#define   BG_HI_YELLOW_E          "\033[103m"
#define   BG_HI_BLUE_E            "\033[104m"
#define   BG_HI_MAGENTA_E         "\033[105m"
#define   BG_HI_CYAN_E            "\033[106m"
#define   BG_HI_WHITE_E           "\033[107m"
//----------------------------------------------------------------

//ANSI 256 and RGB colors-----------------------------------------
#define   A256_FG_E(COLOR_ID)  "\033[38;5;" #COLOR_ID "m"
#define   A256_BG_E(COLOR_ID)  "\033[48;5;" #COLOR_ID "m"
#define   RGB_FG_E(R, G, B)    "\033[38;2;" #R ";" #G ";" #B "m"
#define   RGB_BG_E(R, G, B)    "\033[48;2;" #R ";" #G ";" #B "m"
//----------------------------------------------------------------

//Specific Resets-------------------------------------------------
#define   RESET_BOLD              "\033[22m"
#define   RESET_DIM               "\033[22m"
#define   RESET_ITALIC            "\033[23m"
#define   RESET_UNDERLINE         "\033[24m"
#define   RESET_BLINKING          "\033[25m"
#define   RESET_REVERSE           "\033[27m"
#define   RESET_HIDDEN            "\033[28m"
#define   RESET_STRIKETHROUGH     "\033[29m"
#define   RESET_DOUBLE_UNDERLINE  "\033[24m"
#define   RESET_CURLY_UNDERLINE   "\033[4:0m"
#define   RESET_OVERLINE          "\033[55m"

#define   RESET_FG_COLOR          "\033[39m"
#define   RESET_BG_COLOR          "\033[49m"
//----------------------------------------------------------------

//Function Macros-----------------------------------------------------------------------
#define   BOLD(str)               BOLD_E              str RESET_BOLD
#define   DIM(str)                DIM_E               str RESET_DIM
#define   ITALIC(str)             ITALIC_E            str RESET_ITALIC
#define   UNDERLINE(str)          UNDERLINE_E         str RESET_UNDERLINE
#define   BLINKING(str)           BLINKING_E          str RESET_BLINKING
#define   REVERSE(str)            REVERSE_E           str RESET_REVERSE
#define   HIDDEN(str)             HIDDEN_E            str RESET_HIDDEN
#define   STRIKETHROUGH(str)      STRIKETHROUGH_E     str RESET_STRIKETHROUGH
#define   DOUBLE_UNDERLINE(str)   DOUBLE_UNDERLINE_E  str RESET_DOUBLE_UNDERLINE
#define   CURLY_UNDERLINE(str)    CURLY_UNDERLINE_E   str RESET_CURLY_UNDERLINE
#define   OVERLINE(str)           OVERLINE_E          str RESET_OVERLINE

#define   FG_BLACK(str)           FG_BLACK_E          str RESET_FG_COLOR
#define   FG_RED(str)             FG_RED_E            str RESET_FG_COLOR
#define   FG_GREEN(str)           FG_GREEN_E          str RESET_FG_COLOR
#define   FG_YELLOW(str)          FG_YELLOW_E         str RESET_FG_COLOR
#define   FG_BLUE(str)            FG_BLUE_E           str RESET_FG_COLOR
#define   FG_MAGENTA(str)         FG_MAGENTA_E        str RESET_FG_COLOR
#define   FG_CYAN(str)            FG_CYAN_E           str RESET_FG_COLOR
#define   FG_WHITE(str)           FG_WHITE_E          str RESET_FG_COLOR

#define   FG_HI_BLACK(str)        FG_HI_BLACK_E       str RESET_FG_COLOR
#define   FG_HI_RED(str)          FG_HI_RED_E         str RESET_FG_COLOR
#define   FG_HI_GREEN(str)        FG_HI_GREEN_E       str RESET_FG_COLOR
#define   FG_HI_YELLOW(str)       FG_HI_YELLOW_E      str RESET_FG_COLOR
#define   FG_HI_BLUE(str)         FG_HI_BLUE_E        str RESET_FG_COLOR
#define   FG_HI_MAGENTA(str)      FG_HI_MAGENTA_E     str RESET_FG_COLOR
#define   FG_HI_CYAN(str)         FG_HI_CYAN_E        str RESET_FG_COLOR
#define   FG_HI_WHITE(str)        FG_HI_WHITE_E       str RESET_FG_COLOR

//Using the Erase in Line escape code (\033[K) to correct bugs with newline-terminated strings
//that only appear when resetting the background
#define   BG_BLACK(str)           BG_BLACK_E          str RESET_BG_COLOR "\033[K"
#define   BG_RED(str)             BG_RED_E            str RESET_BG_COLOR "\033[K"
#define   BG_GREEN(str)           BG_GREEN_E          str RESET_BG_COLOR "\033[K"
#define   BG_YELLOW(str)          BG_YELLOW_E         str RESET_BG_COLOR "\033[K"
#define   BG_BLUE(str)            BG_BLUE_E           str RESET_BG_COLOR "\033[K"
#define   BG_MAGENTA(str)         BG_MAGENTA_E        str RESET_BG_COLOR "\033[K"
#define   BG_CYAN(str)            BG_CYAN_E           str RESET_BG_COLOR "\033[K"
#define   BG_WHITE(str)           BG_WHITE_E          str RESET_BG_COLOR "\033[K"

#define   BG_HI_BLACK(str)        BG_HI_BLACK_E       str RESET_BG_COLOR "\033[K"
#define   BG_HI_RED(str)          BG_HI_RED_E         str RESET_BG_COLOR "\033[K"
#define   BG_HI_GREEN(str)        BG_HI_GREEN_E       str RESET_BG_COLOR "\033[K"
#define   BG_HI_YELLOW(str)       BG_HI_YELLOW_E      str RESET_BG_COLOR "\033[K"
#define   BG_HI_BLUE(str)         BG_HI_BLUE_E        str RESET_BG_COLOR "\033[K"
#define   BG_HI_MAGENTA(str)      BG_HI_MAGENTA_E     str RESET_BG_COLOR "\033[K"
#define   BG_HI_CYAN(str)         BG_HI_CYAN_E        str RESET_BG_COLOR "\033[K"
#define   BG_HI_WHITE(str)        BG_HI_WHITE_E       str RESET_BG_COLOR "\033[K"

#define   A256_FG(COLOR_ID, str)  "\033[38;5;" #COLOR_ID "m" str RESET_FG_COLOR
#define   A256_BG(COLOR_ID, str)  "\033[48;5;" #COLOR_ID "m" str RESET_BG_COLOR "\033[K"
#define   RGB_FG(R, G, B, str)    "\033[38;2;" #R ";" #G ";" #B "m" str RESET_FG_COLOR
#define   RGB_BG(R, G, B, str)    "\033[48;2;" #R ";" #G ";" #B "m" str RESET_BG_COLOR "\033[K"

#endif
