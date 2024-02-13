#ifndef _MIPS_H
#define _MIPS_H

#include <stdio.h>

#include "type.h"
#include "symbol.h"
#include "ir.h"
#include "helpers/type-tree.h"
#include "helpers/util-functions.h"


void mips_initialise_temp_regsiter_map();
void mips_initialise_variable_address_map();
void mips_print_program(FILE *output);

#endif
