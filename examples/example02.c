#include <stdio.h>
#include "../c_ansi_textstyling.h"

#define string "Hello, World"

int main() {
    printf(
        BOLD("\n Foreg./Backg. Colors---------------------\n\n")
        " Black:      " FG_BLACK(string)    " | " BG_BLACK(string)    "\n"
        " Red:        " FG_RED(string)      " | " BG_RED(string)      "\n"
        " Green:      " FG_GREEN(string)    " | " BG_GREEN(string)    "\n"
        " Yellow:     " FG_YELLOW(string)   " | " BG_YELLOW(string)   "\n"
        " Blue:       " FG_BLUE(string)     " | " BG_BLUE(string)     "\n"
        " Magenta:    " FG_MAGENTA(string)  " | " BG_MAGENTA(string)  "\n"
        " Cyan:       " FG_CYAN(string)     " | " BG_CYAN(string)     "\n"
        " White:      " FG_WHITE(string)    " | " BG_WHITE(string)    "\n"
    );
    printf(
        BOLD("\n Foreg./Backg. HI Colors------------------\n\n")
        " HI Black:   " FG_HI_BLACK(string)    " | " BG_HI_BLACK(string)    "\n"
        " HI Red:     " FG_HI_RED(string)      " | " BG_HI_RED(string)      "\n"
        " HI Green:   " FG_HI_GREEN(string)    " | " BG_HI_GREEN(string)    "\n"
        " HI Yellow:  " FG_HI_YELLOW(string)   " | " BG_HI_YELLOW(string)   "\n"
        " HI Blue:    " FG_HI_BLUE(string)     " | " BG_HI_BLUE(string)     "\n"
        " HI Magenta: " FG_HI_MAGENTA(string)  " | " BG_HI_MAGENTA(string)  "\n"
        " HI Cyan:    " FG_HI_CYAN(string)     " | " BG_HI_CYAN(string)     "\n"
        " HI White:   " FG_HI_WHITE(string)    " | " BG_HI_WHITE(string)    "\n"
        "\n"
    );
    return 0;
}
