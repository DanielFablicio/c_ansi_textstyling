#include <stdio.h>
#include "../c_ansi_textstyling.h"

#define string "Hello, World\n"

int main() {
    printf(
        BOLD("\n Styles---------------------------\n\n")
        " Normal:            " string "\n"
        " Bold:              " BOLD(string)
        " Dim:               " DIM(string)
        " Italic:            " ITALIC(string)
        " Underline:         " UNDERLINE(string)
        " Blinking:          " BLINKING(string)
        " Reverse:           " REVERSE(string)
        " Hidden:            " HIDDEN(string)
        " Strikethrough:     " STRIKETHROUGH(string)
        " Double Underline:  " DOUBLE_UNDERLINE(string)
        " Curly underline:   " CURLY_UNDERLINE(string)
        " Overline:          " OVERLINE(string)
        "\n"
    );
    return 0;
}
