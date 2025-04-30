#include "../c_ansi_textstyling.h"
#include <stdio.h>
#include <math.h>

#define BREAK_LINE() putchar('\n')

int main() {
    BREAK_LINE();
    puts(BOLD("  RGB Colors: \n"));
    for (int k = 0, c = 0; k < 4; k++) {
        int r = 0, g = 0, b = 0;
        if (k == 0 || k == 3)
            r = 1;
        if (k == 1 || k == 3)
            g = 1;
        if (k == 2 || k == 3)
            b = 1;

        printf("  ");
        for (int c = 0; c <= 85; c++) {
            int clr = c*3; //
            printf(RGB_BG(%d, %d, %d, " "), clr * r, clr * g, clr * b);
        }
        BREAK_LINE();
    }
    BREAK_LINE();
    printf("  ");

    // Being honest... I had this idea below, but ChatGPT that did it,
    // because I couldn't -_-
    #define PI 3.141592653589323
    for (int c = 0; c <= 85; c++) {
        double t = (c / 85.0) * 2 * PI;
        int r, g, b;

        r = (int)((sin(t)*0.5+0.5)*255);
        g = (int)((sin(t + 2*PI / 3)*0.5+0.5)*255);
        b = (int)((sin(t + 4*PI / 3)*0.5+0.5)*255);

        printf(RGB_BG(%d, %d, %d, " "), r, g, b);
    }
    BREAK_LINE();
    BREAK_LINE();
}
