/**
 * @file main.c
 * @author Piotr UjemnyGH Plombon (piotrplombon@gmail.com)
 * @brief Program that convert image to ASCII art
 * @version 0.1
 * @date 2023-03-25
 * 
 * @copyright Copyleft (c) 2023
 * 
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/**
 * @brief Least common divisor, I mean that`s literaly this function name
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int least_common_divisor(int a, int b) {
    while(b != 0) {
        int e = a % b;
        a = b;
        b = e;
    }

    return a;
}

int main(int argc, char *argv[]) {
    // Default chharacter set (gray map) defined by me
    unsigned char default_set[] = {' ', '.', ',', ':', '!', '[', '?', '#'};

    // Character set
    unsigned char set[256];
    // If set to 1 (-b or --better-ratio option) show what could work better for final image, although it doesn`t work perfectly
    int show_hint = 0;
    // Image data
    int x, y, nr;
    // Width and height defined by user, if 0 use image data instead
    int user_defined_x = 0, user_defined_y = 0;
    // Amount of characters per pixel width
    int chars_per_pixel = 2;

    if(argc < 2) {
        printf("Usage: %s [options], --help for more info!\n", argv[0]);
        return 0;
    }

    // Path to image
    char path[1024];
    // Path to output file
    char output_file[1024] = "output.txt";
    // Length of character set used for memory allocation
    int set_length = sizeof(default_set);

    // Data setup

    strcpy(set, default_set);

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--image") == 0) {
            strcpy(path, argv[i + 1]);
        }
        
        if(strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0) {
            user_defined_x = atoi(argv[i + 1]);
        }
        
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--height") == 0) {
            user_defined_y = atoi(argv[i + 1]);
        }
        
        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            strcpy(output_file, argv[i + 1]);
        }

        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--pixel-width") == 0) {
            chars_per_pixel = atoi(argv[i + 1]);
        }

        if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--charset") == 0) {
            set_length = strlen(argv[i + 1]);

            strcpy(set, argv[i + 1]);
        }

        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--better-ratio") == 0) {
            show_hint = 1;
        }
        
        if(strcmp(argv[i], "--help") == 0) {
            printf(
                "\tUsage: %s [options]\n"
                "\tOptions:\n"
                "\t\t-i | --image\t\t-\tpath to image\n"
                "\t\t-o | --output\t\t-\tpath to output text file\n"
                "\t\t-w | --width\t\t-\timage width, optional\n"
                "\t\t-h | --height\t\t-\timage height, optional\n"
                "\t\t-c | --charset\t\t-\tset custom charset\n"
                "\t\t-p | --pixels-width\t-\tamount of chars per pixel width (default 2)\n"
                "\t\t-b | --better-ratio\t-\tshow better ratio hints\n"
                "\t\t--help\t\t\t-\tshows this prompt\n"
                , argv[0]);

            break;
        }
    }

    printf("Info:\n");
    printf("\tOutput: %s\n\tImage path: %s\n", output_file, path);

    unsigned char* pixels = stbi_load(path, &x, &y, &nr, STBI_grey);

    printf("\tOriginal ratio: %dx%d\n", x, y);

    int ratio = least_common_divisor(x, y);

    if(user_defined_x == 0) {
        printf("\tUsing default width: %d\n", x);

        user_defined_x = x;
    }

    if(user_defined_y == 0) {
        printf("\tUsing default height: %d\n", y);

        user_defined_y = y;
    }

    // Calculating pixel offsets for lower (user defined) resolution images
    int x_offset = x / user_defined_x;
    int y_offset = y / user_defined_y;

    printf("\tGenerating file %dx%d px\n", user_defined_x, user_defined_y);

    if(user_defined_x % (x / ratio) != 0 && show_hint) {
        printf("This width could cutoff some pixels from final image, use better width!\n");
        printf("Try: %dx%d\n", user_defined_x + (user_defined_x % (x / ratio)), user_defined_y);
    }

    if(user_defined_y % (y / ratio) != 0 && show_hint) {
        printf("This height could cutoff some pixels from final image, use better height!\n");
        printf("Try: %dx%d\n", user_defined_x, user_defined_y + (user_defined_y % (y / ratio)));
    }

    FILE* file = fopen(output_file, "wb");

    // Creating ASCII image based on default charset (gray map)
    for(int v = 0; v < user_defined_y; v++) {
        for(int u = 0; u < user_defined_x; u++) {
            for(int i = 0; i < chars_per_pixel; i++) {
                fprintf(file, "%c", set[(pixels[((v * y_offset) * x) + (u * x_offset)] / (255 / set_length)) % set_length]);
            }
        }

        fprintf(file, "\n");
    }

    fclose(file);
    stbi_image_free(pixels);

    return 0;
}