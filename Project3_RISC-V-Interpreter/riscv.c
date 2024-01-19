#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"

/************** BEGIN HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ***************/
const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int UNKNOWN_TYPE = 4;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "or", "xor", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi", "ori", "xori", "slti"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

registers_t *registers;
// TODO: create any additional variables to store the state of the interpreter
hashtable_t *state;


void init(registers_t *starting_registers)
{
    registers = starting_registers;
    // TODO: initialize any additional variables needed for state
    state = ht_init(32);

}

// TODO: create any necessary helper functions

int *  words_to_bytes(int w) {
    int* b = (int *)malloc(4 * sizeof(int));
    for(int i = 3; i >= 0; i--) {
        b[i] = (((int) (w >> (8*(3-i)))) & 0xff);
    }
    return b;
}

int byte_to_word(int * b) {
    int w = 0;
    for(int i = 0; i <=3; i++) {
        w = w + (b[i] << (8*(3-i)));
    }
    return w;
}

char* remove_space(char* ins) {
    char* space = (char*)malloc(100*sizeof(char));
    int a = 0;
    int b = 0;
    while (ins[a] != '\0') {
        if (ins[a] != ' ') {
            space[b] = ins[a];
            b++;
        }
        a++;
    }
    space[b] = '\0';
    return space;
}

int* R_control(char* ins) {
    int r1 = 0;
    int r2 = 0;
    int r3_or_shamt = 0;
    int comma1;
    int comma2;
    int end;
    int count = 0;
    while(ins[count] != ',') {
        count++;
    }
    comma1 = count;
    count = comma1 + 1;
    while(ins[count] != ',') {
        count++;
    }
    comma2 = count;
    count = comma2 + 1;
    while(ins[count] != '\0') {
        count++;
    }
    end = count;
    count = comma1 - 1;
    int multi = 1;
    while(ins[count] != '$') {
        r1 = r1 + (multi*((int)(ins[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    count = comma2 - 1;
    multi = 1;
    while(ins[count] != '$') {
        r2 = r2 +  (multi*((int)(ins[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    count = end - 1;    
    multi = 1;
    int hexflag = 0;
    while(ins[count] != ',') {
        if (ins[count] == 'x') {
            hexflag = 1;
        }
        count--;
    }
    count = end - 1;
    multi = 1;
    if (hexflag == 0) {
        while(ins[count] != ',' && ins[count] != '-') {
            if(ins[count] != '$') {
                r3_or_shamt = r3_or_shamt + (multi*((int)(ins[count]) - '0'));
                count--;
                multi = multi * 10;
            }
            else {
                count--;
            }
        }
    }
    else {
         count = comma2 + 1;
         char* hexstring = &ins[count];
         r3_or_shamt = (int)strtol(hexstring, NULL, 0);
    }
    int* values = (int*)malloc(3*sizeof(int));
    values[0] = r1;
    values[1] = r2;
    values[2] = r3_or_shamt;
    count = comma2 + 1;
    if(ins[count] == '-') {
        values[2] = (-1) * values[2];
    }
    return values;
}

int* I_control(char* ins, char* op) {
    if(op[0] == 'l' && op[1] == 'u' && op[2] == 'i') {
        int r1 = 0;
        int r2 = 0;
        int comma1;
        int end;
        int count = 0;
        while(ins[count] != ',') {
            count++;
        }
        comma1 = count;
        count = comma1 + 1;
        while(ins[count] != '\0') {
            count++;
        }
        end = count;
        count = comma1 - 1;
        int multi = 1;
        while(ins[count] != '$') {
            r1 = r1 + (multi*((int)(ins[count]) - '0'));
            count--;
            multi = multi * 10;
        }
        count = end - 1;
        multi = 1;
        int hexflag = 0;
        while(ins[count] !=',') {
            if (ins[count] == 'x') {
                hexflag = 1;
            }
            count--;
        }
        count = end - 1;
        if (hexflag == 0) {
            while(ins[count] != ',') {
                if(ins[count] != '$') {
                    r2 = r2 + (multi*((int)(ins[count]) - '0'));
                    count--;
                    multi = multi * 10;
                }
                else {
                    count--;
                }
            }
        }
        else {
             count = comma1 + 1;
             char* hexstring = &ins[count];
             r2 = (int)strtol(hexstring, NULL, 0);
        }
        int* values = (int*)malloc(3*sizeof(int));
        values[0] = r1;
        values[1] = r2;
        values[2] = 0;
        return values;
    }
    else {
        return R_control(ins);
    }
}

// same but for mmemory instructions
int* memory_control(char* ins) {
    int r1 = 0;
    int offset = 0;
    int r2 = 0;
    int comma1 = 0;
    int par1 = 0;
    int par2 = 0;
    int count = 0;
    while(ins[count] != ',') {
        count++;
    }
    comma1 = count;
    while(ins[count] != '(') {
        count++;
    }
    par1 = count;
    while(ins[count] != ')') {
        count++;
    }
    par2 = count;
    count = comma1 - 1;
    int multi = 1;
    while(ins[count] != '$') {
        r1 = r1 + (multi*((int)(ins[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    count = comma1;
    multi = 1;
    int hexflag = 0;
    while(ins[count] !='\0') {
        if (ins[count] == 'x') {
            hexflag = 1;
        }
        count++;
    }
    if (hexflag == 0) {
        count = par1 - 1;
        int multi = 1;
        while(ins[count] != ',') {
            offset = offset + (multi*((int)(ins[count]) - '0'));
            count--;
            multi = multi * 10;
        }
    }
    else {
        count = comma1 + 1;
        char* hexstring = &ins[count];
        offset = (int)strtol(hexstring, NULL, 0);
    }
    count = par2 - 1;
    multi = 1;
    while (ins[count] != '$') {
        r2 = r2 + (multi*((int)(ins[count]) - '0'));
        count--;
        multi = multi * 10;
    }
    int* values = (int*)malloc(3*sizeof(int));
    values[0] = r1;
    values[1] = offset;
    values[2] = r2;
    return values;
}

void step(char *ins)
{
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    char *op = strsep(&ins, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op);
    // Skip this instruction if it is not in our supported set of instructions
    if (op_type == UNKNOWN_TYPE)
    {
        return;
    }

    // TODO: write logic for evaluating instruction on current interpreter state
    char* instruction_nospace = remove_space(ins);
    if (op_type == R_TYPE) {
        int* values = R_control(instruction_nospace);
        if(op[0] == 'a' && op[1] == 'd' && op[2] == 'd' && op[3] == 'u') {
            registers -> r[values[0]] = registers -> r[values[1]] + registers -> r[values[2]];
        }
        if(op[0] == 's' && op[1] == 'u' && op[2] == 'b' && op[3] == 'u') {
            registers -> r[values[0]] = registers -> r[values[1]] - registers -> r[values[2]];
        }
        if(op[0] == 'a' && op[1] == 'n' && op[2] == 'd') {
            registers -> r[values[0]] = registers -> r[values[1]] & registers -> r[values[2]];
        }
        if(op[0] == 'o' && op[1] == 'r') {
            registers -> r[values[0]] = registers -> r[values[1]] | registers -> r[values[2]];
        }
        if(op[0] == 'x' && op[1] == 'o' && op[2] == 'r') {
            registers -> r[values[0]] = registers -> r[values[1]] ^ registers -> r[values[2]];
        }
        if(op[0] == 'n' && op[1] == 'o' && op[2] == 'r') {
            registers -> r[values[0]] = ~(registers -> r[values[1]] ^ registers -> r[values[2]]);
        }
        if(op[0] == 's' && op[1] == 'l' && op[2] == 't') {
            registers -> r[values[0]] = (registers -> r[values[1]] < registers -> r[values[2]]);
        }
        if(op[0] == 'm' && op[1] == 'o' && op[2] == 'v' && op[3] == 'z') {
            if(registers -> r[values[2]] == 0) {
                registers -> r[values[0]] = registers -> r[values[1]];
            }
        }
    }
    else if(op_type == I_TYPE) {
        int* values = I_control(instruction_nospace, op);
        if(op[0] == 'a' && op[1] == 'd' && op[2] == 'd' && op[3] == 'i' && op[4] == 'u') {
            registers -> r[values[0]] = registers -> r[values[1]] + values[2];
        }
        if(op[0] == 'a' && op[1] == 'n' && op[2] == 'd' && op[3] == 'i') {
            registers -> r[values[0]] = registers -> r[values[1]] & values[2];
        }
        if(op[0] == 'o' && op[1] == 'r' && op[2] == 'i') {
            registers -> r[values[0]] = registers -> r[values[1]] | values[2];
        }
        if(op[0] == 'x' && op[1] == 'o' && op[2] == 'r' && op[3] == 'i') {
            registers -> r[values[0]] = registers -> r[values[1]] ^ values[2];
        }
        if(op[0] == 's' && op[1] == 'l' && op[2] == 't' && op[3] == 'i') {
            registers -> r[values[0]] = (registers -> r[values[1]]) < values[2];
        }
        if(op[0] == 's' && op[1] == 'l' && op[2] == 'l') {
            registers -> r[values[0]] = (registers -> r[values[1]]) << values[2];
        }
        if(op[0] == 's' && op[1] == 'r' && op[2] == 'a') {
            registers -> r[values[0]] = (registers -> r[values[1]]) >> values[2];
        }
        if(op[0] == 'l' && op[1] == 'u' && op[2] == 'i') {
            registers -> r[values[0]] = values[1] << 16;
        }
    }
    else if(op_type = MEM_TYPE) {
        int* values = memory_control(instruction_nospace);
        if(op[0] == 's' && op[1] == 'w') {
            int* bytes = words_to_bytes(registers -> r[values[0]]);
            ht_add(state, values[1] + (registers -> r[values[2]]) + 0, bytes[3]);
            ht_add(state, values[1] + (registers -> r[values[2]]) + 1, bytes[2]);
            ht_add(state, values[1] + (registers -> r[values[2]]) + 2, bytes[1]);
            ht_add(state, values[1] + (registers -> r[values[2]]) + 3, bytes[0]);
        }
       if(op[0] == 'l' && op[1] == 'w') {
            int bytes[4] = {0, 0, 0, 0};
            bytes[3] = ht_get(state, values[1] + (registers -> r[values[2]]));
            bytes[2] = ht_get(state, values[1] + registers -> r[values[2]] + 1);
            bytes[1] = ht_get(state, values[1] + registers -> r[values[2]] + 2);
            bytes[0] = ht_get(state, values[1] + registers -> r[values[2]] + 3);
            int word = byte_to_word(bytes);
            registers -> r[values[0]] = word;
       }
       if(op[0] == 's' && op[1] == 'b') {
            int byte = (registers -> r[values[0]] & 0xff);
            ht_add(state, values[1] + (registers -> r[values[2]]), byte);
       }
       if(op[0] == 'l' && op[1] == 'b') {
            int byte = ht_get(state, values[1] + (registers -> r[values[2]]));
            registers -> r[values[0]] = byte;
       }
       if(op[0] == 'l' && op[1] == 'b' && op[2] == 'u') {
            unsigned int byte = ht_get(state, values[1] + (registers -> r[values[2]]));
            registers -> r[values[0]] = byte;
       }
    }
}


