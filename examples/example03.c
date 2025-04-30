#include "../c_ansi_textstyling.h"
#include <stdio.h>

#define BREAK_LINE() putchar('\n')

int main() {
    BREAK_LINE();
    puts(BOLD("  ANSI 256 Colors:\n"));
    for (int i = 0; i < 256 / 8; i++) {
        printf("  ");
        for (int k = 0; k < 8; k++) {
            int clr = i + (k * 32);
            printf("%03d: " A256_BG(%d, "   ")" ", clr , clr);
        }
        BREAK_LINE();
    }
    BREAK_LINE();
}
