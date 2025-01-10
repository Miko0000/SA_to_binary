/*
 * Main code
 *
 *
 *
 *
 */
#define VERSION "1.2"
#define GITHUB_LINK "https:/""/github.com/KingWing34/SA_to_binary.git"
#define DISCORD_LINK "https:/""/discord.gg/fwjc86ERXq"
#include <iostream>
#include "argv.h"
#include "config.h"
#include <stdlib.h>

    //=====================================================
    // This function was moved under the main function to keep the
    // code neat, that is why it's declared here.

void print_help(char *argv[]);

    //=====================================================
    // Convert ASCII codes

int pre_convert(int c, int t, int r) {
            //(input, type, rotate)

    if(c == EOF) {
        return EOF;
    }

    if(c >= 48 && c <= 57) {
        c = c - 48;
    } else if(c >= 65 && c <= 86) {
        c = c - 55;
    } else {
        return -2;
    }
    if(c >= t) {
        return -2;
    }

    // Rotate by r if r is not 0

    if(r != 0) {
        c = c + r;
        if(c > (t - 1)) {
            c = c - (t - 1);
        }
    }

    // Return converter result
    return c;
}

    //=====================================================
    // This function gets the size of input bits

int get_size(char byte, int t) {

    if(t == 2) {
        return 1;
    }
    if(t >= 3 && t <= 4) {
        return 2;
    }
    if(t >= 5 && t <= 8) {
        return 3;
    }
    if(t >= 9 && t <= 16) {
        return 4;
    }
    if(t >= 17 && t <= 32) {
        return 5;
    }

    // previous code (may be useful later?)

    /*
    unsigned char i = 0b10000000;

    int size = 0;

    // If 'i' is not 0, figure out how many bits are used

    if(byte != 0) {
        while(((i >> size) | byte) > byte) size++;
        return 8 - size;
    } else {
        return 1;
    }
    */

    return 1;
}

    //=====================================================
    // Reverse the bit order

int Rbitorder(char bits) {

    unsigned char i = 0b10000000;

    int rbits = 0;

    int shift = 7;

    while(i > 0) {
    rbits = rbits | (((bits & i) << (7 - shift)) >> shift);
    shift = shift - 1;
    i = i >> 1;
    }
    return rbits;
}

    //=====================================================
    // Invert bits in a byte

int invert(char i) {
    i =~ i;
    return i;
}

    //=====================================================
    // This is where this program does its things

int check(int argc, char *argv[]) {

    // Set valid arguments

    argv_set(valid, 0, "-h", argc, argv);
    argv_set(valid, 1, "-c", argc, argv); // TODO
    argv_set(valid, 2, "-s", argc, argv);
    argv_set(valid, 3, "-i", argc, argv);
    argv_set(valid, 4, "-r", argc, argv); // TODO
    argv_set(valid, 5, "-in", argc, argv);
    argv_set(valid, 6, "-out", argc, argv);
    argv_set(valid, 7, "-t", argc, argv);
    argv_set(valid, 8, "-e", argc, argv);
    argv_set(valid, 9, "-table", argc, argv); //TODO

    // Print help and exit if -h argument given

    if(argv_get(valid, 0)[0] == '-') {
        print_help(argv);
        return 1;
    }

    // Manage file I/O

    FILE *fp_in, *fp_out;
    char *in_name = argv_get(valid, 5);
    char *out_name = argv_get(valid, 6);
    const char *default_name = "converter_out.bin";

    // Check if input file is readable

    if ((fp_in = fopen(in_name,"rb")) == NULL) {
		std::cout << "ERROR: cannot read file " << in_name << ".\n";
		return 1;
    }

    // Check if the -out option is given, then check if output
    // file is writable otherwise make output file using default name

    if(argv_get(valid, 6)[0]) {
        if ((fp_out = fopen(out_name,"w")) == NULL) {
		std::cout << "ERROR: cannot write file " << out_name << ".\n";
		return 1;
        }
    } else if ((fp_out = fopen(default_name,"w")) == NULL) {
        std::cout << "ERROR: cannot write file " << out_name << ".\n";
    }

    // Read -t (default is 2)

    int n;
    if(argv_get(valid, 7)[0]) {
        char *convertType = argv_get(valid, 7);
        n = strtol(argv_get(valid, 7), NULL, 10);
        std::cout << "Converting as: " << convertType << "\n";
        if(n < 2 || n > 32) {
            std::cout << "ERROR: Type should be 2-32\n";
            return 1;
        }
    } else {
        n = 2;
    }

    // read -s (default is 0)

    int s;
    if(argv_get(valid, 2)[0]) {
        char *shift = argv_get(valid, 2);
        s = strtol(argv_get(valid, 2), NULL, 10);
        std::cout << "Shift output by: " << shift << "\n";

        if (s < 0 || s > 7) {
            std::cout << "ERROR: Shift should be 1-7\n";
            return 1;
        }
    } else {
        s = 0;
    }

    // Variables used in the converter code below

    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int used = s;
    int r = 0;
    int rotate = 0;

    // Loop until all characters are converted

    while(a != EOF) {
        while(a != EOF && used < 8) {
            a = fgetc(fp_in);
            a = pre_convert(a,n,rotate);
            if(a < 0)
                continue;
            r = (r << get_size(a,n)) | a;
            used = used + get_size(a,n);
        }
        if(used > 8) {
            c = 0;
            while(d != (used - 8)) {
                c = (c << 1) + 1;
                d++;
            }
            b = r & c;
            r = r >> d;
            used = 8;
        }
        if(a == EOF) {
            r = r << (8 - used);
        }
        if((a == EOF && used != 0) || used == 8) {
            if(argv_get(valid, 8)[0] == '-' ) {
                r = Rbitorder(r);
            }
            if(argv_get(valid, 3)[0] == '-' ) {
                r = invert(r);
            }
            fputc((unsigned char) r, fp_out);
            r = 0;
            used = 0;
        }
        if(d != 0) {
            r = b;
            used = d;
            d = 0;
        }
    }


    // Close files and exit

    fclose(fp_in);
    fclose(fp_out);
    return 0;
}

    //=====================================================
    // Main function
char *argv_0 = NULL;
extern "C" int gui(int argc, char *argv[]);
int main(int argc, char *argv[]) {
    argv_0 = argv[0];

    // If too many arguments, tell user to check help!

    if ( argc > 12) {
        std::cout << "ERROR: Too many arguments!\n\nCheck help!\n";
        return 1;
    }

    // Assume that the right arguments were given if any

    if (argc > 1) {
        return check(argc, argv);
    }

    // No arguments, print help

    if ( argc == 1) {
        //print_help(argv);
        gui(argc, argv);
        return 1;
    }
}

    //=====================================================
    // This function prints help (btw, if there's a better way to do this, let me know!)

void print_help(char *argv[]) {
    const char *text1 =
    "SA to binary converter " VERSION "\n\n"
    "Usage:\n";
    std::cout << text1;

    std::cout << argv[0] << "   -in <file_name>\n";
    std::cout << argv[0] << "   -in <file_name> -out <file_name>\n";
    std::cout << argv[0] << "   [option1] ... [option_n] -in <file_name>\n";
    std::cout << argv[0] << "   [option1] ... [option_n] -in <file_name> -out <file_name>\n\n";

    const char *text2 =
    "Options:\n"
    " -in  <file_name>   File input\n"
    " -out <file_name>   File output\n"
    // " -table <file name> (see github for table format)\n" // TODO
    // " -c                 Try all combinations\n" // TODO
    " -e                 Reverse bit order\n"
    " -i                 Invert output\n"
    // " -r                 Rotate conversion table (creates more than one file)\n" //TODO
    " -s <n>             Shift bits (where n is 1-7)\n"
    " -t <n>             Type (where n is 2-32)\n"
    " -h                 Prints this help message\n";
    std::cout << text2;
    return ;
}

#include "gui.h"
