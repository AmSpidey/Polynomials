/** @file
   Kalkulator wielomianow

   @author Marta Nowakowska
   @copyright Uniwersytet Warszawski
   @date 2017-05-25,
*/

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include "poly.h"

Poly parsePoly(char **iter, int *column, int row, Mono *garbage);

/**
 * Czy jest przecinkiem?
 * @param c
 * @return is a comma?
 */

bool isAComma(char c) {
    return c == ',';
}

/**
 * Czy jest enterem?
 * @param c
 * @return is an enter
 */

bool isAnEnter(char c) {
    return (c == '\n');
}

/**
 * Czy plus?
 * @param c
 * @return is a plus
 */

bool isAPlus(char c) {
    return c == '+';
}

/**
 * Czy litera?
 * @param c
 * @return is a letter?
 */

bool isALetter(char c) {
    if ( 'A' <= c && c <= 'Z' ) return true;
    if ( 'a' <= c && c <= 'z' ) return true;
    return false;
}

/**
 * Czy zamykajacy nawias?
 * @param c
 * @return is a closing bracket?
 */

bool isAClosingBracket(char c) {
    return (c == ')');
}

/**
 * Czy to otwierajacy nawias?
 * @param c
 * @return is an opening bracket?
 */

bool isAnOpeningBracket(char c) {
    return c == '(';
}

/**
 * czy jest numerem?
 * @param c
 * @return is a number?
 */

bool isANumber(char c) {
    return ('0' <= c && c <= '9');
}

/**
 * czy jest minusem?
 * @param c
 * @return is a Minus?
 */

bool isAMinus(char c) {
    return (c == '-');
}

/**
 * Struktura stosu wielomianow.
 */

typedef struct Stack {
    Poly p; ///<wielomian
    struct Stack *next; ///<nastepny stos.
} Stack;

/**
 * Blad parsowania wielomianu.
 * @param row
 * @param column
 */

void errorParsingPoly(int row, int column) {
    fprintf(stderr, "ERROR %d %d\n", row, column);
}

/**
 * Za malo wielomianow na stacku. Blad.
 * @param row
 */

void errorStackUnderflow(int row) {
    fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", row);
}

/**
 * Zla komenda.
 * @param row
 */

void errorWrongCommand(int row) {
    fprintf(stderr, "ERROR %d WRONG COMMAND\n", row);
}

/**
 * Blad : zla zmienna.
 * @param row
 */

void errorWrongValue(int row) {
    fprintf(stderr, "ERROR %d WRONG VALUE\n", row);
}

/**
 * Blad : zla zmienna.
 * @param row
 */

void errorWrongVariable(int row) {
    fprintf(stderr, "ERROR %d WRONG VARIABLE\n", row);
}

/**
 * Wez od uzytkownika nastepny znak.
 * @param c
 * @param column
 */

void nextCharacter(char *c, int *column) {
    (*column) = (*column) + 1;
    char l = getchar();
    *c = l;
}

/**
 * Odczytywanie wykladnika
 * @param input
 * @param column
 * @return
 */

poly_exp_t getExp(char **input, int *column) {

    poly_exp_t number = 0;
    while (isANumber(**input)) {
        number *= 10;
        number += (poly_exp_t) ((**input) - '0');
        nextCharacter(*input, column);
    }
    return number;
}

/**
 * Odczytaj zmienna.
 * @param input
 * @param column
 * @param ok
 * @return variable
 */

long int getVariable(char **input, int *column, bool *ok) {

    long int number = 0;
    while (isANumber(**input)) {
        if ((UINT_MAX / 10) < number ) {
            *ok = false;
            return number;
        }
        number *= 10;
        number += (long int) ((**input) - '0');
        nextCharacter(*input, column);
        if ( number > UINT_MAX) {
            return number;
        }
    }

    return number;
}

/**
 * Odczytaj zmienna.
 * @param input
 * @param column
 * @param ok
 * @return value
 */

long long int getValue(char **input, int *column, bool *ok) {
    long long int number = 0;
    while (isANumber(**input)) {
        if ((LONG_MAX / 10) < number ) {
            *ok = false;
            return number;
        }
        if ( number * 10 < number ) {
            *ok = false;
            return number;
        }
        number *= 10;
        number += (long long int) ((**input) - '0');
        nextCharacter(*input, column);
        if ( number > LONG_MAX ) {
            return number;
        }
    }
    return number;
}

/**
 * Czy stos pusty?
 * @param s
 * @return is Stack empty?
 */

bool isEmpty(Stack **s) {
    return (*s == NULL);
}

/**
 * Stworz stos.
 * @param s
 */

void create(Stack **s) {
    (*s) = NULL;
}

/**
 * Zdejmij ze stosu.
 * @param s
 * @return
 */

Poly pop(Stack **s) {
    Poly poop = (**s).p;
    Stack *temp = *s;
    *s = (*s)->next;
    free(temp);
    return poop;
}

/**
 * Gorna wartosc na stosie.
 * @param s
 * @return
 */

Poly *top(Stack **s) {
    return &(**s).p;
}

/**
 * Wrzuc na stos.
 * @param s
 * @param pushup
 */

void push(Stack **s, Poly pushup) {
    Stack *stack = malloc(sizeof(struct Stack));
    stack->next = (*s);
    stack->p = pushup;
    (*s) = stack;
}

/**
 * Czysc stos.
 * @param s
 */

void clear(Stack **s) {
    while (!isEmpty(s)) {
        Poly poop = (**s).p;
        Stack *temp = *s;
        *s = (*s)->next;
        free(temp);
    }
}

/**
 * Drukuj jednomian.
 * @param pMono
 */

void printMono(Mono *pMono) {
    if ( pMono->next != NULL) {

        printMono(pMono->next);
        printf("+");
    }

    printf("(");
    printPoly(&pMono->p);
    printf(",%d)", pMono->exp);
}

/**
 * Drukuj wielomian
 * @param p
 */

void printPoly(Poly *p) {

    if ( PolyIsCoeff(p)) {
        printf("%li", p->coeff);
        return;
    }
    printMono(p->mono);
}

/**
 * Policz Coeff.
 * @param input
 * @param column
 * @return coeff
 */

poly_coeff_t getCoeff(char **input, int *column) {

    poly_coeff_t number = 0;
    while (isANumber(**input)) {
        number *= 10;
        number += (poly_coeff_t) ((**input) - '0');
        nextCharacter(*input, column);
    }

    return number;
}

/**
 * Policz jednomiany sasiednie.
 * @param m
 * @return monos count
 */

int MonoCounterByMono(Mono *m) {
    int i = 0;
    Mono *temp = m;
    while (temp != NULL) {
        temp = temp->next;
        i++;
    }
    return i;
}

/**
 * Automatycznie zwroc smieciowy jednomian (error).
 * @param garbage
 * @return
 */

Mono returnGarbage(Mono *garbage) {
    Mono m = (Mono) {
            .p=PolyZero(),
            .next=garbage,
            .exp=0,
    };
    return m;
}

/**
 * Wczytywanie jednomianu
 * @param row
 * @param column
 * @param iter
 * @param garbage
 * @return
 */

Mono parseMono(int row, int *column, char **iter, Mono *garbage) {
    Mono m1;
    if ( isAMinus(**iter)) {
        nextCharacter(*iter, column);
        if ( isANumber(**iter)) {
            m1 = (Mono) {
                    .p=PolyFromCoeff((-1) * getCoeff(iter, column)),
                    .next=NULL,
                    .exp=0,
            };
        } else {
            return returnGarbage(garbage);
        }
    } else if ( isANumber(**iter)) {
        m1 = (Mono) {
                .p=PolyFromCoeff((1) * getCoeff(iter, column)),
                .next=NULL,
                .exp=0,
        };
    } else if ( isAnOpeningBracket(**iter)) {
        nextCharacter(*iter, column);
        m1 = (Mono) {
                .p=parsePoly(iter, column, row, garbage),
                .next=NULL,
                .exp=0,
        };

        if ( isAComma(**iter)) {
            nextCharacter(*iter, column);
        } else {
            return returnGarbage(garbage);
        }
        m1.exp = getExp(iter, column);
//        nextCharacter(iter, &c, column);
        if ( !isAClosingBracket(**iter)) {
            return returnGarbage(garbage);
        }
        nextCharacter(*iter, column); //closing bracket
    } else {
        return returnGarbage(garbage);
    }
    return m1;
}

/**
 * Wczytywanie wielomianu.
 * @param iter
 * @param column
 * @param row
 * @param garbage
 * @return Poly
 */

Poly parsePoly(char **iter, int *column, int row, Mono *garbage) {

    Mono m1 = parseMono(row, column, iter, garbage);

    if ( m1.next == garbage ) {
        Poly toReturn = PolyFromCoeff(1);
        toReturn.mono = garbage;
        return toReturn;
    }

    Mono *temp = &m1;

    while (isAPlus(**iter)) {
        nextCharacter(*iter, column);
        Mono m2 = parseMono(row, column, iter, garbage);
        if ( m2.next == garbage ) {
            Poly toReturn = PolyFromCoeff(1);
            toReturn.mono = garbage;
            return toReturn;
        }
        temp->next = MonoPointerTo(&m2);
        temp = temp->next;
    }
    temp->next = NULL;

    unsigned int i = MonoCounterByMono(&m1);
    Mono k[i];
    Mono *temp2 = &m1;
    for (int j = 0; j <= i - 1; j++) {
        k[j] = *temp2;
        temp2 = temp2->next;
    }
    Poly added = PolyAddMonos(i, k);
    return added;
}


/**
 * Wczytywanie poly ze sprawdzeniem bledu.
 * @param iter
 * @param column
 * @param row
 * @param garbage
 * @return Poly or garbage (error).
 */

Poly parsePolynomial(char **iter, int *column, int row, Mono *garbage) {

    Poly p = parsePoly(iter, column, row, garbage);

    if ( p.mono == garbage ) return p;

    if ( !isAnEnter(**iter)) {
        //PolyDestroy(&p);
        Poly q = PolyFromCoeff(1);
        q.mono = garbage;
        return q;
    }

    return p;
}

/**
 * Czy komendy sa rowne do danego miejsca?
 * @param sizeCommand
 * @param command
 * @param table
 * @return command == table
 */

bool compareCommand(int sizeCommand, char command[], char table[]) {

    for (int i = 0; i <= sizeCommand - 1; i++) {
        if ( command[i] != table[i] ) return false;
    }
    return true;
}

/**
 * parsowanie komendy.
 * @param iter
 * @param row
 * @param s
 */

void parseCommand(char **iter, int row, Stack **s) {

    char temp = **iter;

    char table[9];

    int column = 1;
    int i = 0;

    while (temp != '\n' && i < 9 && temp != ' ') {
        **iter = temp;
        table[i] = temp;
        i++;
        nextCharacter(&temp, &column);
    }

    if ((temp == '\n' || temp == ' ') && i < 9 ) {
        table[i] = temp;
    }
    **iter = temp;

    if ( compareCommand(4, "ADD\n", table)) {
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }
        Poly const p = pop(s);
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            push(s, p);
            return;
        }
        Poly const q = pop(s);
        Poly k = PolyAdd(&p, &q);
        PolyDestroy(&q);
        push(s, k);
        return;
    } else if ( compareCommand(9, "IS_COEFF\n", table)) {

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }

        if ( PolyIsCoeff(top(s))) {
            printf("1\n");
        } else {
            printf("0\n");
        }

    } else if ( compareCommand(8, "IS_ZERO\n", table)) {

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }

        if ( PolyIsZero(top(s))) {
            printf("1\n");
        } else {
            printf("0\n");
        }

    } else if ( compareCommand(6, "CLONE\n", table)) {
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }

        push(s, PolyClone(top(s)));

    } else if ( compareCommand(5, "ZERO\n", table)) {
        push(s, PolyZero());

    } else if ( compareCommand(4, "MUL\n", table)) {
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }
        Poly const p = pop(s);
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            push(s, p);
            return;
        }
        Poly const q = pop(s);
        Poly k = PolyMul(&p, &q);
        PolyDestroy(&q);
        push(s, k);
        return;
    } else if ( compareCommand(4, "NEG\n", table)) {

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }
        Poly p = pop(s);
        Poly neg = PolyNeg(&p);
        PolyDestroy(&p);
        push(s, neg);

    } else if ( compareCommand(4, "SUB\n", table)) {

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }
        Poly const p = pop(s);
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            push(s, p);
            return;
        }
        Poly const q = pop(s);
        Poly k = PolySub(&p, &q);
        push(s, k);
        PolyDestroy(&q);
        return;

    } else if ( compareCommand(6, "IS_EQ\n", table)) {
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }
        Poly const p = pop(s);
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            push(s, p);
            return;
        }
        Poly const q = pop(s);

        if ( PolyIsEq(&p, &q)) {
            printf("1\n");
        } else {
            printf("0\n");
        }

        push(s, q);
        push(s, p);

    } else if ( compareCommand(4, "DEG\n", table)) {

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }

        printf("%d\n", PolyDeg(top(s)));

    } else if ( compareCommand(7, "DEG_BY ", table)) {
        bool ok = true;
        long int j = 0;

        nextCharacter(*iter, &column);

        if ( !isANumber(**iter)) {
            if ( **iter != '\n' ) {
                while (**iter != '\n' && **iter != EOF) {
                    nextCharacter(*iter, &column);
                }
            }
            errorWrongVariable(row);
            return;
        }

        j = getVariable(iter, &column, &ok);
        if ( !ok ) {
            errorWrongVariable(row);
            if ( **iter != '\n' ) {
                while (**iter != '\n' && **iter != EOF) {
                    nextCharacter(*iter, &column);
                }
            }
            return;
        }

        if ( j > UINT_MAX) {
            errorWrongVariable(row);
            return;
        }
        if ( !isAnEnter(**iter)) {
            errorWrongVariable(row);
            if ( **iter != '\n' ) {
                while (**iter != '\n' && **iter != EOF) {
                    nextCharacter(*iter, &column);
                }
            }
            return;
        }

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }

        printf("%d\n", PolyDegBy(top(s), j));

    } else if ( compareCommand(3, "AT ", table)) {
        bool ok = true;
        int column = 1;
        long long int j = 0;

        nextCharacter(*iter, &column);

        if ( isAMinus(**iter)) {
            nextCharacter(*iter, &column);
            if ( !isANumber(**iter)) {
                if ( **iter != '\n' ) {
                    while (**iter != '\n' && **iter != EOF) {
                        nextCharacter(*iter, &column);
                    }
                }
                errorWrongValue(row);
                return;
            }

            j = (-1) * getValue(iter, &column, &ok);
            if ( !ok ) {
                errorWrongValue(row);
                if ( **iter != '\n' ) {
                    while (**iter != '\n' && **iter != EOF) {
                        nextCharacter(*iter, &column);
                    }
                }
                return;
            }
            if ( !isAnEnter(**iter)) {
                if ( **iter != '\n' ) {
                    while (**iter != '\n' && **iter != EOF) {
                        nextCharacter(*iter, &column);
                    }
                }
                errorWrongValue(row);
                return;
            }
            if ( j > LONG_MAX || j < LONG_MIN) {
                if ( **iter != '\n' ) {
                    while (**iter != '\n' && **iter != EOF) {
                        nextCharacter(*iter, &column);
                    }
                }
                errorWrongValue(row);
                return;
            }

            if ( isEmpty(s)) {
                errorStackUnderflow(row);
                return;
            }

            Poly q = pop(s);
            Poly p = PolyAt(&q, j);
            PolyDestroy(&q);
            push(s, p);
            return;

        }
        if ( !isANumber(**iter)) {
            if ( **iter != '\n' ) {
                while (**iter != '\n' && **iter != EOF) {
                    nextCharacter(*iter, &column);
                }
            }
            errorWrongValue(row);
            return;
        }
        j = getValue(iter, &column, &ok);
        if ( !ok ) {
            if ( **iter != '\n' ) {
                while (**iter != '\n' && **iter != EOF) {
                    nextCharacter(*iter, &column);
                }
            }
            errorWrongValue(row);
            return;
        }
        if ( j > LONG_MAX || j < LONG_MIN) {
            if ( **iter != '\n' ) {
                while (**iter != '\n' && **iter != EOF) {
                    nextCharacter(*iter, &column);
                }
            }
            errorWrongValue(row);
            return;
        }

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }

        Poly q = pop(s);
        Poly p = PolyAt(&q, j);
        push(s, p);
        PolyDestroy(&q);
        return;

    } else if ( compareCommand(6, "PRINT\n", table)) {

        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        } else {
            printPoly(top(s));
            printf("\n");
        }

    } else if ( compareCommand(4, "POP\n", table)) {
        if ( isEmpty(s)) {
            errorStackUnderflow(row);
            return;
        }
        Poly q = pop(s);
        PolyDestroy(&q);
    } else {
        errorWrongCommand(row);
    }
    if ( **iter != '\n' ) {
        while (**iter != '\n' && **iter != EOF) {
            nextCharacter(*iter, &column);
        }
    }
}

/**
 * Parsowanie wiersza
 * @param iter
 * @param row
 * @param head
 * @param garbage
 */

void parseRow(char *iter, int row, Stack **head, Mono *garbage) {
    int column = 1;
    if ( isALetter(*iter)) {
        parseCommand(&iter, row, head);
        return;
    } else {
        Poly p = parsePolynomial(&iter, &column, row, garbage);
        if ( p.mono == garbage ) {
            errorParsingPoly(row, column);
            nextCharacter(iter, &column);
        } else {
            push(head, p);
        }
    }
}

/**
 * Parsowanie inputu
 * @param garbage
 */

void parseInput(Mono *garbage) {
    int i = 0;
    Stack **s = malloc(sizeof(Stack *));
    create(s);

    char c = '\0';

    while (true) {
        c = getchar();
        if ( c == EOF) {
            break;
        }
        i++;
        parseRow(&c, i, s, garbage);
    }
    clear(s);
}

/**
 * Glowna funkcja
 * @return 0
 */

int main() {

    Mono garbage = (Mono) {
            .p=PolyZero(), .next=NULL, .exp=0
    };
    Mono *garb = MonoPointerTo(&garbage);

    parseInput(garb);

    free(garb);

    return 0;
}
