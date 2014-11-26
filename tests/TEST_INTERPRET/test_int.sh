#!/usr/bin/env bash
export dir="INT"
export name="INTERPRET"
export file="../src/INT_interpret.c ../src/LEX_lexem.c"
export src='#include "../src/inc.h"

extern P3AC *INST;

int main () {
    TTerm op1 = { 
        .value.integer = 5,
        .type = TERM_INT,
        .name = "a"
    }, op2 = {
        .value.integer = 2,
        .type = TERM_INT,
        .name = "b"
    }, ret = {
        .value.integer = 0,
        .type = TERM_INT,
        .name = "c"
    };


    INST = malloc (sizeof (P3AC)*2);
    INST[1] = NULL;
    for (P3AC *i = INST; *i != NULL; i++)
        *i = malloc (sizeof (struct S3AC));
    
    struct S3AC s = (struct S3AC) {
        .op  = OP_PLUS,
        .op1 = &op1,
        .op2 = &op2,
        .ret = &ret
    };
    INST[0] = &s;
    INT_interpret ();
    printf("%d+%d=%d\n", op1.value.integer, op2.value.integer, ret.value.integer);
    
    s.op = OP_MINUS;
    INT_interpret ();
    printf("%d-%d=%d\n", op1.value.integer, op2.value.integer, ret.value.integer);

    s.op = OP_MUL;
    INT_interpret ();
    printf("%d*%d=%d\n", op1.value.integer, op2.value.integer, ret.value.integer);

    s.op = OP_DIV;
    INT_interpret ();
    printf("%d/%d=%d\n", op1.value.integer, op2.value.integer, ret.value.integer);

    s.op = OP_GREAT;
    INT_interpret ();
    printf("%d>%d=%s\n", op1.value.integer, op2.value.integer, ret.value.boolean ? "true" : "false");

    s.op = OP_LESS;
    INT_interpret ();
    printf("%d<%d=%s\n", op1.value.integer, op2.value.integer, ret.value.boolean ? "true" : "false");





    return 0;
}'
export input=""
export output=""
export retCode=0
