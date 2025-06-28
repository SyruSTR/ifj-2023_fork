/**
 * @file main.c
 * @author Nelia Mashlai (xmashl00)
 * @author Nikita Vetluzhskikh (xvetlu00)
 * @author Oleg Borsh (xborsh00)
 * @author Murad Mikogaziev (xmikog00)
 * @brief main brain
 */

#include "parser.h"
#include "error.h"
#include <stdio.h>

int main()
{
    int ret_code;
    if ((ret_code = analyse())) {
        return ret_code;
    }

    return ER_NONE;
}
