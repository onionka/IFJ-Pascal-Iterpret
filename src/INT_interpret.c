/**
 * \file INT_parser.c
 * \authon Miroslav Cibulka
 * \brief Interpret trojadresneho kodu
 */

#include "inc.h"
#include "init.h"

extern PGLOB_DEST pointers;
static PTSStack *STACK;
TTerm  __ebp = {
        .type = TERM_EBP
    }, __esp = {
        .type = TERM_ESP,
    },*EBP = &__ebp,
      *ESP = &__esp;

__attribute__ ((unused))
P3AC *EIP, *PEIP;

static PTSStack SInit () {
    return NULL;
}

static bool SEmpty () {
    return *STACK == NULL;
}

static void SPush (TTerm *add) {
    PTSStack el = malloc (sizeof(TSStack));
    el->next = *STACK;
    el->term = add;
    *STACK = el;
}

__attribute__((unused))
static TTerm *STop () {
    return (*STACK)->term;
}

static TTerm *SPop () {
    TTerm *ret = (*STACK)->term;
    PTSStack pom = (*STACK)->next;
    free(*STACK);
    *STACK = pom;
    return ret;
}

static TTerm *SPick (uint32_t pos) {
    PTSStack s = *STACK;
    for (; pos != 0; pos--)
        s = s->next;
    return s->term;
}

static void SFree () {
    while (*STACK != NULL) {
        PTSStack toDel = *STACK;
        *STACK = (*STACK)->next;
        free(toDel);
    }
    free(STACK);
}

static void plus (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.integer = op1->value.integer + op2->value.integer;
        break;
            
        case TERM_REAL   :
            ret->value.real = op1->value.real + op2->value.real;
        break;

        case TERM_STRING :
            ret->value.string = strcat(op1->value.string, op2->value.string);
        break;

        default          : break;
    }
}

static void minus (TTerm *op1, TTerm *op2, TTerm *ret) {
     switch (ret->type) {
        case TERM_INT    :
            ret->value.integer = op1->value.integer - op2->value.integer;
        break;
            
        case TERM_REAL   :
            ret->value.real = op1->value.real - op2->value.real;
        break;

        default          :  break;
    }
}

static void mul (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            //log ("op1 %d, op2 %d\n", op1->value.integer, op2->value.integer);
            ret->value.integer = op1->value.integer * op2->value.integer;
        break;
        case TERM_REAL   :
            ret->value.real = op1->value.real * op2->value.real;
        break;
        default          : break;
    }
}

static void division (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.integer = op1->value.integer / op2->value.integer;
        break;
        case TERM_REAL   :
            ret->value.real = op1->value.real / op2->value.real;
        break;
        default          : break;
    }
}

static void assign (TTerm *op1, 
__attribute__ ((unused)) TTerm *op2, TTerm *ret) {
    ret->value = op1->value;
}

static void less (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.boolean = op1->value.integer < op2->value.integer;
        break;
            
        case TERM_REAL   :
            ret->value.boolean = op1->value.real < op2->value.real;
        break;

        case TERM_STRING :
            ret->value.boolean = strcmp(op1->value.string, op2->value.string) < 0;
        break;

        default          : break;
    }
}

static void greater (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.boolean = op1->value.integer > op2->value.integer;
        break;
            
        case TERM_REAL   :
            ret->value.boolean = op1->value.real > op2->value.real;
        break;

        case TERM_STRING :
            ret->value.boolean = strcmp(op1->value.string, op2->value.string) > 0;
        break;

        default          : break;
    }
}

static void lesseq (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.boolean = op1->value.integer <= op2->value.integer;
        break;
            
        case TERM_REAL   :
            ret->value.boolean = op1->value.real <= op2->value.real;
        break;

        case TERM_STRING :
            ret->value.boolean = strcmp(op1->value.string, op2->value.string) <= 0;
        break;

        default          : break;
    }
}

static void greateq (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.boolean = op1->value.integer >= op2->value.integer;
        break;
            
        case TERM_REAL   :
            ret->value.boolean = op1->value.real >= op2->value.real;
        break;

        case TERM_STRING :
            ret->value.boolean = strcmp(op1->value.string, op2->value.string) >= 0;
        break;

        default          : break;
    }
}

static void equal (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.boolean = op1->value.integer == op2->value.integer;
        break;
            
        case TERM_REAL   :
            ret->value.boolean = op1->value.real == op2->value.real;
        break;

        case TERM_STRING :
            ret->value.boolean = strcmp(op1->value.string, op2->value.string) == 0;
        break;

        default          : break;
    }
}

static void nequal (TTerm *op1, TTerm *op2, TTerm *ret) {
    switch (ret->type) {
        case TERM_INT    :
            ret->value.boolean = !(op1->value.integer == op2->value.integer);
        break;
            
        case TERM_REAL   :
            ret->value.boolean = !(op1->value.real == op2->value.real);
        break;

        case TERM_STRING :
            ret->value.boolean = strcmp(op1->value.string, op2->value.string) != 0;
        break;

        default          : break;
    }
}

static void ret (        TTerm *op1, TTerm *op2,
__attribute__ ((unused)) TTerm *ret) {
    // Local variable cleaning
    for (int i = 0; i < op1->value.integer; i++)
        free(SPop(&STACK));
    
    TTerm *add = SPop(&STACK);
    P3AC *pom = &EIP[add->value.address];
    free(add);
    //log("RETURN: on address:%u op1:%d op2:%d\n", (uint32_t)(pom-EIP), op1->value.integer, op2->value.integer);
    PEIP = pom;

    // Arguments cleaning
    for (int i = 0; i < op2->value.integer; i++)
        free(SPop(&STACK));
}

static void push (       TTerm *op1,
__attribute__ ((unused)) TTerm *op2,
__attribute__ ((unused)) TTerm *ret) {
    TTerm *s = malloc (sizeof(TTerm));
    if (s == NULL) error(ERR_INTERNAL, "Chyba alokacie pamete!\n");
    memcpy(s, op1, sizeof(TTerm));
    //log("PUSH: op1:%u on stack:%u\n", op1->value.address, s->value.address);
    SPush(s);
}

static void pop (
__attribute__ ((unused)) TTerm *op1,
__attribute__ ((unused)) TTerm *op2, TTerm *ret) {
    if (ret != NULL) {
        if (!SEmpty(STACK)) {
            ret->value = SPop(&STACK)->value;
        } else 
            error (ERR_INTERNAL, "Stack is empty");
    } else {
        SPop(&STACK);
    }
    //log ("POP\n");
}

static void jmp (        TTerm *op1,
__attribute__ ((unused)) TTerm *op2,
__attribute__ ((unused)) TTerm *ret) {
    PEIP = &EIP[op1->value.address-1];
    //log("JMP on adress %u with offset %u\n", (uint32_t)(PEIP-EIP), (*PEIP)->op);
}

static void jtrue (      TTerm *op1, TTerm *op2,
__attribute__ ((unused)) TTerm *ret) {
    if (op1->value.boolean == true) {
        jmp(op2, NULL, NULL);
    }
        //log("Not jumping on adress %u with offset %u\n", (uint32_t)(PEIP-EIP), (*PEIP)->op);
}

static void call (       TTerm *op1, 
__attribute__ ((unused)) TTerm *op2, 
__attribute__ ((unused)) TTerm *ret) {
    // Jump address
    TTerm Address = {
        .value.address = PEIP-EIP,
        .type = TERM_OFFSET
    };
    push(&Address, NULL, NULL);
    for (int i = 0; i < 6; i++) {
        if (&embededFunc[i] == op1) {
            op1->value.emb_function();
            return;
        }
    }
    jmp(op1, NULL, NULL);
    //log ("CALL: to address: %u\n", op1->value.address);
}

static void nop (
__attribute__ ((unused)) TTerm *op1,
__attribute__ ((unused)) TTerm *op2,
__attribute__ ((unused)) TTerm *ret) {
    /// NOPE NOPE NOPE
    //log("NOP");
    return;
}

static void not (        TTerm *op1,
__attribute__ ((unused)) TTerm *op2, TTerm *ret) {
    /// Neguje vstup op1 ulozi negaciu do ret
    ret->value.boolean = !(op1->value.boolean);
}

static void load (       TTerm *op1,
__attribute__ ((unused)) TTerm *op2, TTerm *ret) {
    /// nacita zo zasobnika nti prvok
    ret->value = SPick(op1->value.address)->value;
    //log ("LOAD: value: %d\n", ret->value.integer);
}

static void store (      TTerm *op1,
__attribute__ ((unused)) TTerm *op2, TTerm *ret) {
    /// ulozi na zasobnik na presne miesto data do termu
    SPick(ret->value.address)->value = op1->value;
}

static void __sort () {
    TTerm *str = SPick(1),
           zero = {
               .value.integer = 0,
               .type = TERM_INT
           };
    str->value.string = EMB_sort (str->value.string, EMB_length(str->value.string));
    ret(&zero, &zero, NULL);
}

static void __copy () {
    TTerm *str  = SPick(1),
          *from = SPick(2),
          *size = SPick(3),
           zero = {
               .value.integer = 0,
               .type = TERM_INT
           }, two = {
               .value.integer = 2,
               .type = TERM_INT
           };
    size->value.string = EMB_copy(str->value.string, from->value.integer, size->value.integer);
    ret(&zero, &two, NULL);
}

static void __length () {
    TTerm *str = SPick(1),
           zero = {
               .value.integer = 0
           };
    str->value.integer = EMB_length(str->value.string);
    ret(&zero, &zero, NULL);
}

static void __find () {
    TTerm *str = SPick(1),
          *fstr= SPick(2),
           zero = {
               .value.integer = 0
           },
           one = {
               .value.integer = 1
           };
    fstr->value.integer = EMB_find(str->value.string, fstr->value.string);
    ret(&zero, &one, NULL);
}

static void __write () {
    TTerm *n, k;
    int pocet = SPick (1)->value.integer; 
    for (int i = 0; i < pocet; i++) {
        n = SPick(2+i);
        switch (n->type) {
            case TERM_INT : 
                printf ("%d", n->value.integer);
            break;
            case TERM_REAL :
                printf ("%f", n->value.real);
            break;
            case TERM_STRING :
                printf ("%s", n->value.string);
            break;
            case TERM_BOOL :
                printf ("%s", n->value.boolean ? "true" : "false");
            break;

            default :
            break;
        }
    }
    k.value.offset = pocet+1;
    TTerm zero = (TTerm) { 
        .value.offset = 0,
        .type = TERM_OFFSET
    };
    ret(&zero, &k, NULL);
}

static void ____readln () {
    TTerm *id = SPick(1),
           one = {
               .value.integer = 1
           },
           zero = {
               .value.integer = 0
           };
    char *pom, *beg;
    int c, size;
    switch (id->type) {
        case TERM_INT :
            scanf("%d", &id->value.pointer->value.integer);
        break;
        case TERM_REAL :
            scanf("%f", &id->value.pointer->value.real);
        break;
        case TERM_STRING :
            beg=pom=malloc (sizeof(char)*32);
            while ((c=getchar())!=EOF&&(c!='\n')) {
                *(pom++)=c;
                *pom='\0';
                size=pom-beg;
                if (size%32 == 0) {
                    pom=size+(beg=realloc(beg,size*2));
                }
            }
            id->value.pointer->value.string = beg;
        break;
        default :
            error(ERR_SEM_TYPE, "Nekompatabilny typ\n");
        break;
    }
    ret(&zero, &one, NULL);
}

__attribute__ ((unused))
static void (*INST[])(TTerm *op1, TTerm *op2, TTerm *ret) = {
    &plus, &minus, &mul, &division, 
    &assign, &less, &greater, &lesseq, 
    &greateq, &equal, &nequal,  &call, &ret, 
    &push, &pop, &jtrue, &jmp, 
    &nop, &load, &not, &store
    // TODO: Pridat dalsie funkcie!
};

TTerm embededFunc[] = {
    {
        .value.emb_function = &__sort,
        .type = TERM_EMB,
        .name = "sort"
    },
    {
        .value.emb_function = &__copy,
        .type = TERM_EMB,
        .name = "copy" 
    },
    {
        .value.emb_function = &__length,
        .type = TERM_EMB,
        .name = "length" 
    },
    {
        .value.emb_function = &__find,
        .type = TERM_EMB,
        .name = "find" 
    }, 
    {
        .value.emb_function = &__write,
        .type = TERM_EMB,
        .name = "write" 
    }, 
    {
        .value.emb_function = &____readln,
        .type = TERM_EMB,
        .name = "readln" 
    }
};


void INT_interpret () {
    /// Instruction pointer
    PEIP = EIP;
    /// Stack pointer
    ESP->value.esp = STACK;
    // TODO: 
    //      * pridat volanie semantiky
    //      * pridat dealokacie: snad DONE
    pointers->ebp = STACK = malloc (sizeof (TSStack*));
    *STACK = SInit();

    for (int i = 0; *PEIP != NULL; i++) {
        //log("INST %u %d\n", (uint32_t)(PEIP-EIP), (*PEIP)->op);
        INST[(*PEIP)->op]((*PEIP)->op1, (*PEIP)->op2, (*PEIP)->ret);
        PEIP++;
    }
    SFree();
    return;
}
