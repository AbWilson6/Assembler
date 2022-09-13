//
// Created by Abigail Wilson on 3/2/22.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "format_text.h"
#include "map.h"
#include "translate.h"
#define MAX_LEN 200
#define MAX_RANGE 24576


int main (int argc, char *argv[]) {
    // while loop that goes through whole infile
    openFile(argv[1], argv[2]);
    char line[MAX_LEN];
    char newline[MAX_LEN];
    char* command;
    // create new map with pre-existing values
    make_map();
    // create a new map with comp and jump values
    insert_hack_map();
    // read first line
    readLine(line);
    while (!endof()) {
        // clean and check lines
        clean(line, newline); // @R0
        if (newline[0] != '\0') {
            // for each line, call l_map_var on it
            l_map_var(newline); // just increases the line_counter by 1 so it now equals 2
            a_map_var(newline); // doesn't do anything because R0 is already in the map
            command = check(newline); // ACOMMAND
            //printf(command);
            //char x[20];
            //sprintf(x, "%d", strcmp(command, "C_COMMAND\n"));
            //printf(x);
            if (strcmp(command, "C_COMMAND\n") == 0) {
                char c_bin[17];
                convert_c(newline, c_bin);
                writeLine(c_bin);
                writeLine("\n");
            }
            else if (strcmp(command, "A_COMMAND\n") == 0) { // goes to here
                char a_binary[17];
                convert_a(newline, a_binary); // puts the binary value into a_binary
                writeLine(a_binary); // puts a_binary into the outfile
                writeLine("\n"); // writes new line
            }
            else if (strcmp(command, "L_COMMAND\n") == 0) {
                char l_binary[17];
                convert_l(newline, l_binary);
                writeLine(l_binary);
                writeLine("\n");
            }

        }
        readLine(line); // reads next line

    }
    // close files
    close();
    freeM();
}