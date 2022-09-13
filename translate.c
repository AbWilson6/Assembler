//
// Created by Abigail Wilson on 3/20/22.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "map.h"
#include "format_text.h"
#define MAX_LEN 200
#define MAX_RANGE 24576
int current_ram = 16;
char str[17];
int line_counter = 1;
map store_values = NULL;
map hack_map = NULL;

// creates a map with pre-existing labels and their values
void make_map() {
    store_values = createMap(MAX_RANGE);

    // add pre-existing values to map (0-16)
    insertKey(store_values, "SP", "0");
    insertKey(store_values, "LCL", "1");
    insertKey(store_values, "ARG", "2");
    insertKey(store_values, "THIS", "3");
    insertKey(store_values, "THAT", "4");
    insertKey(store_values, "R0", "0");
    insertKey(store_values, "R1", "1");
    insertKey(store_values, "R2", "2");
    insertKey(store_values, "R3", "3");
    insertKey(store_values, "R4", "4");
    insertKey(store_values, "R5", "5");
    insertKey(store_values, "R6", "6");
    insertKey(store_values, "R7", "7");
    insertKey(store_values, "R8", "8");
    insertKey(store_values, "R9", "9");
    insertKey(store_values, "R10", "10");
    insertKey(store_values, "R11", "11");
    insertKey(store_values, "R12", "12");
    insertKey(store_values, "R13", "13");
    insertKey(store_values, "R14", "14");
    insertKey(store_values, "R15", "15");
    insertKey(store_values, "SCREEN", "16384");
    insertKey(store_values, "KBD", "24576");
}

// creates a second map that contains binary values for comp and jump
void insert_hack_map() { // in main, would create a new map and call this function on it
    // would result in map specifically for comp and jump values
    // insert comp and jump into a different map tan store_values
    hack_map = createMap(MAX_RANGE);
    // comp
    insertKey(hack_map, "0", "0101010");
    insertKey(hack_map, "1", "0111111");
    insertKey(hack_map, "-1", "0111010");
    insertKey(hack_map, "D", "0101010");
    insertKey(hack_map, "A", "0110000");
    insertKey(hack_map, "M", "1110000");
    insertKey(hack_map, "!D", "0001101");
    insertKey(hack_map, "!A", "0110001");
    insertKey(hack_map, "!M", "1110001");
    insertKey(hack_map, "-D", "0001111");
    insertKey(hack_map, "-A", "0110011");
    insertKey(hack_map, "-M", "1110011");
    insertKey(hack_map, "D+1", "0011111");
    insertKey(hack_map, "A+1", "0110111");
    insertKey(hack_map, "M+1", "1110111");
    insertKey(hack_map, "D-1", "0001110");
    insertKey(hack_map, "A-1", "0110010");
    insertKey(hack_map, "M-1", "1110010");
    insertKey(hack_map, "D+A", "0000010");
    insertKey(hack_map, "D+M", "1000010");
    insertKey(hack_map, "D-A", "0010011");
    insertKey(hack_map, "D-M", "1010011");
    insertKey(hack_map, "A-D", "0000111");
    insertKey(hack_map, "M-D", "1000111");
    insertKey(hack_map, "D&A", "0000000");
    insertKey(hack_map, "D&M", "1000000");
    insertKey(hack_map, "D|A", "0010101");
    insertKey(hack_map, "D|M", "1010101");

    // jump
    insertKey(hack_map, "null", "000");
    insertKey(hack_map, "JGT", "001");
    insertKey(hack_map, "JEQ", "010");
    insertKey(hack_map, "JGE", "011");
    insertKey(hack_map, "JLT", "100");
    insertKey(hack_map, "JNE", "101");
    insertKey(hack_map, "JLE", "110");
    insertKey(hack_map, "JMP", "111");

}

// adds the L_COMMAND labels to the map
void l_map_var(char* line) { // this returns a map that has the l commands in it (CALL FIRST WHEN MAKING A MAP IN MAIN)
    // pass #1 --> go through each line and if A or C, increase line number
    // if L, check if key in map, if not, add it with proper line number
    char *command_output = check(line);
    if (*command_output == 'A') {
        line_counter++;
    }
    if (*command_output == 'C') {
        line_counter++;
    }
    if (*command_output == 'L') {
        char s[2] = ")";
        char *first = strtok(line, s);
        char t[2] = "(";
        char *l_first = strtok(first, t);
        char *l_variable = strtok(first, t);
        sprintf(str, "%d", line_counter);
        insertKey(store_values, l_variable, str);
        line_counter++;
    }
}

// checks if a string of characters are all digits
// just for use inside this c file
int isnum(const char* a_variable) { // returns 1 if it is a num
    for (int i = 0; a_variable[i] != '\0'; i++) {
        if (isdigit(a_variable[i]) == 0) { // if ever not a digit, return 0
            return 0;
        }
    }
    return 1;
}

// adds any A_COMMAND variables to the map (ignores integers)
void a_map_var(const char* line) { // the argument for put in map should be a cleaned line
    // returns a map with all the a command values in it (CALL AFTER CALLING THE L MAP ONE)
    // pass #2 --> add a variables into the map
    // for memory address when inserting key
    char *command_output = check(line);
    // insert a command key into map
    if (*command_output == 'A') {
        char s[2] = "@";
        char *a_first = strtok(line, s);
        char *a_value = strtok(line, s);
        if (containsKey(store_values, a_value) == -1) {
            // if a variable is not a number, add it with the right ram address
            if (isnum(a_value) != 1) {
                // if the key is not already in the map, put it in with value of proper ram address
                sprintf(str, "%d", current_ram);
                insertKey(store_values, a_value, str);
                current_ram++;
            }
        }
    }
}

// converts the dest=comp;jump variables to their binary equivalents
void convert_c(char *line, char c_str[17]) {
    char *command_output = check(line);

    // if the command_output is C, check if there is a semicolon and if there is an equal sign
    // if there is no semicolon, then first part of = is the dest and second part is comp, and jump is null
    // if there is no equal sign, then the first part is comp and the second part is jump, dest is null
    if (*command_output == 'C') {
        strcpy(c_str, "111");
        // if there is no semicolon
        if (strchr(line, ';') == NULL) {
            const char s[2] = "=";
            char *dest = strtok(line, s);
            char *dest_val = NULL;
            if (dest == NULL) {
                dest_val = "000";
            }
            else if (strcmp(dest, "M") != 0) {
                dest_val = "001";
            }
            else if (strcmp(dest, "D") != 0) {
                dest_val = "010";
            }
            else if (strcmp(dest, "MD") != 0) {
                dest_val = "011";
            }
            else if (strcmp(dest, "A") != 0) {
                dest_val = "100";
            }
            else if (strcmp(dest, "AM") != 0) {
                dest_val = "101";
            }
            else if (strcmp(dest, "AD") != 0) {
                dest_val = "110";
            }
            else if (strcmp(dest, "AMD") != 0) {
                dest_val = "111";
            }

            char *comp = "null";
            char *Fcomp = strchr(line, '=');
            if (Fcomp != NULL) {
                strcpy(comp,Fcomp);
            }


            // use lookupKey (returns the associated value for that key - parameters = map and key)
            if (lookupKey(hack_map, comp) != NULL) {
                char *comp_val = lookupKey(hack_map, comp);
                strcat(c_str, comp_val);
                strcat(c_str, dest_val);
            }
            char *jump = "null";
            if (lookupKey(hack_map, jump) != NULL) {
                char *jump_val = lookupKey(hack_map, jump);
                strcat(c_str, jump_val);
            }

        }

        // if there is no equal sign
        else if (strchr(line, '=') == NULL) {
            const char r[2] = ";";
            char *comp = strtok(line, r);
            char *jump = strchr(line, ';');

            char *dest = "000";
            if (lookupKey(hack_map, comp) != NULL) {
                char *comp_val = lookupKey(hack_map, comp);
                strcat(c_str, comp_val);
                strcat(c_str, dest);
            }
            if (jump == NULL) {
                jump = "null";
            }
            if (lookupKey(hack_map, jump) != NULL) {
                char *jump_val = lookupKey(hack_map, jump);
                strcat(c_str, jump_val);
            }

        }

        // if there is both a semicolon and an equal sign
        else {
            const char t[2] = "=";
            char *dest = strtok(line, t);
            char *dest_val = NULL;
            if (dest == NULL) {
                dest_val = "000";
            }
            else if (strcmp(dest, "M") != 0) {
                dest_val = "001";
            }
            else if (strcmp(dest, "D") != 0) {
                dest_val = "010";
            }
            else if (strcmp(dest, "MD") != 0) {
                dest_val = "011";
            }
            else if (strcmp(dest, "A") != 0) {
                dest_val = "100";
            }
            else if (strcmp(dest, "AM") != 0) {
                dest_val = "101";
            }
            else if (strcmp(dest, "AD") != 0) {
                dest_val = "110";
            }
            else if (strcmp(dest, "AMD") != 0) {
                dest_val = "111";
            }
            const char r[2] = ";";
            const char q[2] = "=";
            char *second_token = strtok(line, q);
            char *comp = strtok(second_token, r);
            char *jump = strchr(line, ';');
            if (lookupKey(hack_map, comp) != NULL) {
                char *comp_val = lookupKey(hack_map, comp);
                strcat(c_str, comp_val);
                strcat(c_str, dest_val);
            }
            if (jump == NULL) {
                char *jump_val = "000";
            }
            else if (lookupKey(hack_map, jump) != NULL) {
                char *jump_val = lookupKey(hack_map, jump);
                strcat(c_str, jump_val);
            }
        }

    }
}

// converts either the number or the variable on an A_COMMAND line to its binary equivalent
void convert_a(const char *line, char binstr[17]) {

    // check line
    char *command_line = check(line);
    binstr[16] = '\0';
    if (*command_line == 'A') {
        char a[2] = "@";
        char *first = strtok(line, a);
        char *a_key = strtok(line, a);
        if (lookupKey(store_values, a_key) != NULL) {
            char *a_value = lookupKey(store_values, a_key);
            int num = atoi(a_value);
            int counter = 15;
            while (counter >= 0) {
                // convert to binary
                // at the end, cast as string and put in the map as the value for the a_variable key
                int remainder = num % 2;
                num = num / 2;
                if (remainder == 1) {
                    binstr[counter] = '1';
                }
                if (remainder == 0) {
                    binstr[counter] = '0';
                }

                counter--;
            }
        }
        if (containsKey(store_values, a_key) == -1) {
            int num = atoi(a_key);
            int counter = 15;
            while (counter > 0) {
                int remainder = num % 2;
                num = num / 2;
                binstr[counter] = (char) remainder;
                counter--;
            }
        }
    }
}

char *convert_l(char *line, char binstr2[17]) {
    char *command_line = check(line);
    binstr2[16] = '\0';
    if (*command_line == 'L') {
        const char s[2] = ")";
        char *first = strtok(line, s);
        char t[2] = "(";
        char *l_first = strtok(first, t);
        char *l_key = strtok(first, t);
        if (lookupKey(store_values, l_key) != NULL) {
            char *l_value = lookupKey(store_values, l_key);

            int num = atoi(l_value);
            int counter = 15;
            while (counter >= 0) {
                // convert to binary
                // at the end, cast as string and put in the map as the value for the a_variable key
                int remainder = num % 2;
                num = num / 2;
                if (remainder == 1) {
                    binstr2[counter] = '1';
                }
                if (remainder == 0) {
                    binstr2[counter] = '0';
                }
                counter--;
            }
        }
    }
}

void freeM() {
    freeMap(store_values);
    freeMap(hack_map);
}
