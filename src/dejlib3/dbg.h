#pragma once

// #define DBG_PRINT_SCI_MESSAGES

#define DUMP(typ, var) __mingw_printf("* " #var " = " typ "\n", var)
#define DUMP_VEC(typ, vec) do { __mingw_printf("* " #vec " (%d) : ", vec.size()); for (auto var : vec) __mingw_printf(" " typ, var);  __mingw_printf("\n"); } while(0)

char const * sci_code_to_str(long code);
