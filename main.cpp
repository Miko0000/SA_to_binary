/*
 * Main code
 *
 *
 *
 *
 */
#include <iostream>
#include "argv.h"
#include <stdlib.h>

    //=====================================================
    // This function was moved under the main function to keep the
    // code neat, that is why it's declared here.

void print_help(char *argv[]);

    //=====================================================
    // Do actual converting stuffin finally!

int pre_convert(int c) {

    // Turn ASCII codes into the values that they should be converted into.


    if(c >= 48 && c < 58) {
        c = c - 48;
    } else if(c >= 65 && c < 91) {
        c = c - 65 + 10;
    } else {
        c = 0;
    }

    // Return converter result

    return c;
}

    //=====================================================
    // This function gets the size of input bits

int get_size(char byte) {
    unsigned char i = 0b10000000;

    int size = 0;

    // If 'i' is not 0, figure out how many bits are used

    if(byte != 0) {
        while(((i >> size) | byte) > byte) size++;
        return 8 - size;
    } else {
        return 1;
    }
}

    //=====================================================
    // This is where this program does its things

int check(int argc, char *argv[]) {

    // Set valid arguments

    argv_set(valid, 0, "-h", argc, argv); // works
    argv_set(valid, 1, "-p", argc, argv); // TODO
    argv_set(valid, 2, "-s", argc, argv); // TODO
    argv_set(valid, 3, "-i", argc, argv); // TODO
    argv_set(valid, 4, "-c", argc, argv); // TODO later
    argv_set(valid, 5, "-in", argc, argv); // works
    argv_set(valid, 6, "-out", argc, argv); // works
    argv_set(valid, 7, "-t", argc, argv); // TODO

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

    // Convert

    int a = 0;
    int b = 0;
    int c = 0;
    int nused;
    int abs = 0;
    int r = 0;


    a = fgetc(fp_in);
    a = pre_convert(a);

    while(a != EOF) {
        nused = 8;
        r = 0;
        while(nused > 0 && a != EOF) {
            r = (r << get_size(a)) | a;
            nused = nused - get_size(a);
            if(nused > 0) {
                a = fgetc(fp_in);
                if( a == EOF ) // add EOF detection (done)
                    break;
                a = pre_convert(a);
            }
        }
        if(nused < 0 && a != EOF) {
            abs = 0;
            while(nused != 0) {
                abs++;
                nused++;
            }
            b = 0;
            c = 0;
            while(b != abs) {
                c = (c << 1) + 1;
                b++;
            }
            a = r & b;
            r = r >> abs;
            // std::cout << r << "\n";
            fputc((unsigned char) r, fp_out);
            r = b;
            nused = 8 - abs;
            a = fgetc(fp_in);
            if( a == EOF ) // add EOF detection (done)
                break;
            a = pre_convert(a);
        }
        if(nused == 0 && a != EOF) {
            a = fgetc(fp_in);
            if( a == EOF ) // add EOF detection (done)
                break;
            a = pre_convert(a);
        }
        if(a == EOF){
            r = r << nused;
        }
        // std::cout << r << "\n";
        fputc((unsigned char) r, fp_out);
    }
    return 0;
}

    //=====================================================
    // Main function

int main(int argc, char *argv[]) {

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
        print_help(argv);
        return 1;
    }
}

    //=====================================================
    // This function prints help (btw, if there's a better way to do this, let me know!)

void print_help(char *argv[]) {
    const char *text1 =
    "SA to binary converter v1.0\n\n"
    "Usage:\n";
    std::cout << text1;

    std::cout << argv[0] << "   -in <file_name>\n";
    std::cout << argv[0] << "   -in <file_name> -out <file_name>\n";
    std::cout << argv[0] << "   [option1] ... [option_n] -in <file_name>\n";
    std::cout << argv[0] << "   [option1] ... [option_n] -in <file_name> -out <file_name>\n\n";

    const char *text2 =
    "Options:\n"
    " -in  <file_name>  File input\n"
    " -out <file_name>  File output\n"
   // " -t <n>            Type (where n is 2-32)\n"
   // " -p                Try all possibilities\n"
   // " -i                Invert output\n"
   // " -s <n>            Shift bits (where n is 1-7)\n"
   // " -c                crazy (check github for explanation)\n"
    " -h                Prints this help message\n";
    std::cout << text2;
    return ;
}