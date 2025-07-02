/**
 * @file main.c
 * @author Nelia Mashlai (xmashl00)
 * @author Nikita Vetluzhskikh (xvetlu00)
 * @author Oleg Borsh (xborsh00)
 * @author Murad Mikogaziev (xmikog00)
 * @brief main brain
 */

#include <signal.h>

#include "parser.h"
#include "error.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc > 2 && strcmp(argv[1],"-d") == 0) {
        raise(SIGSTOP);
    }
    int ret_code;
    if ((ret_code = analyse())) {
        return ret_code;
    }

    return ER_NONE;
}
