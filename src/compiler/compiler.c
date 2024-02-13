#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>

#include "compiler.h"
#include "scanner.h"
#include "parser.h"
#include "node.h"
#include "symbol.h"
#include "type.h"
#include "ir.h"
#include "mips.h"
#include "optimization-1.h"
#include "optimization-2.h"

extern int errno;
int annotation = 0;
int optimization = 0;
bool reset_registers = false;
bool hide_detailed_output = false;
bool print_to_file = false;

void compiler_print_error(YYLTYPE location, const char *format, ...) {
  va_list ap;
  fprintf(stdout, "Error (%d, %d) to (%d, %d): ",
          location.first_line, location.first_column,
          location.last_line, location.last_column);
  va_start(ap, format);
  vfprintf(stdout, format, ap);
  va_end(ap);
  fputc('\n', stdout);
}

static void print_errors_from_pass(char *pass, int error_count) {
  fprintf(stdout, "%s encountered %d %s.\n",
          pass, error_count, (error_count == 1 ? "error" : "errors"));
}

/**
 * Launches the compiler.
 * 
 * The following describes the arguments to the program:
 * compiler [-s (scanner|parser|symbol|type|ir|mips)] [-o outputfile] [inputfile|stdin]
 *
 * -s : the name of the stage to stop after. Defaults to
 *      runs all of the stages.
 * -o : the name of the output file. Defaults to "output.s"      
 *
 * You should pass the name of the file to process or redirect stdin.
 */
int main(int argc, char **argv) {
  FILE *output;
  struct symbol_table *symbol_table = symbol_create_table(FILE_SCOPE);
  char *stage, output_name[NAME_MAX + 1];
  int opt;
  yyscan_t scanner;
  struct node *parse_tree;
  int error_count;

  strncpy(output_name, "output.s", NAME_MAX + 1);
  stage = "mips";
  while (-1 != (opt = getopt(argc, argv, "o:s:O:a"))) {
    switch (opt) {
      case 'o':
        strncpy(output_name, optarg, NAME_MAX);
        print_to_file = true;
        break;
      case 's':
        stage = optarg;
        break;
      case 'a':
        annotation = 1;
        break;
      case 'O': {
        if (NULL != optarg) {
          optimization = atoi(optarg);
        }
        break;
      }
    }
  }

  reset_registers = 0 == strcmp("mips", stage) && !optimization;
  hide_detailed_output = 0 == strcmp("mips", stage) || optimization;

  /* Figure out whether we're using stdin/stdout or file in/file out. */
  if (optind >= argc) {
    scanner_initialize(&scanner, stdin);
  } else if (optind == argc - 1) {
    scanner_initialize(&scanner, fopen(argv[optind], "r"));
  } else {
    fprintf(stdout, "Expected 1 input file, found %d.\n", argc - optind);
    return 1;
  }

  if (0 == strcmp("scanner", stage)) {
    error_count = 0;
    scanner_print_tokens(stdout, &error_count, scanner);
    scanner_destroy(&scanner);
    if (error_count > 0) {
      print_errors_from_pass("Scanner", error_count);
      return 1;
    } else {
      return 0;
    }
  }

  error_count = 0;
  parse_tree = parser_create_tree(&error_count, scanner);
  scanner_destroy(&scanner);
  if (NULL == parse_tree) {
    print_errors_from_pass("Parser", error_count);
    return 1;
  }

  if (0 == strcmp("parser", stage)) {
    node_print_statement_list(stdout, parse_tree);
    return 0;
  }

  error_count = symbol_add_from_statement_list(symbol_table, parse_tree);
  error_count += check_statement_labels_tables_validity();
  error_count += check_function_declarations_validity();
  if (error_count > 0) {
    print_errors_from_pass("Symbol table", error_count);
    return 1;
  }
  if (0 != strcmp("ir", stage) && !hide_detailed_output) {
    fprintf(stdout, "\n================= SYMBOLS ================\n");
    symbol_print_table(stdout);
    fprintf(stdout, "\n================= SYMBOLS FORMATTED ================\n");
    symbol_print_table_assignment_format(stdout);
  }
  if (0 == strcmp("symbol", stage)) {
    fprintf(stdout, "\n=============== PARSE TREE ===============\n");
    node_print_statement_list(stdout, parse_tree);
    fprintf(stdout, "\n\n");
    return 0;
  }

  error_count = type_assign_in_statement_list(parse_tree);
  error_count = type_check_main_func_validity();
  if (error_count > 0) {
    print_errors_from_pass("Type checking", error_count);
    return 1;
  }
  if (0 != strcmp("ir", stage) && !hide_detailed_output) {
    fprintf(stdout, "\n=============== PARSE TREE ===============\n");
    node_print_statement_list(stdout, parse_tree);
    fprintf(stdout, "\n\n");
  }
  if (0 == strcmp("type", stage)) {
    return 0;
  }

  error_count = ir_generate_for_statement_list_wrapper(parse_tree);
  if (error_count > 0) {
    print_errors_from_pass("IR generation", error_count);
    return 1;
  }
  if (!hide_detailed_output) {
    fprintf(stdout, "=================== IR ===================\n");
    ir_print_section(stdout);
    if (0 == strcmp("ir", stage)) {
      return 0;
    }
  }

  if (optimization >= 1) {
    ir_start_optimization_1();

    fprintf(stdout, "\n=============== IR OPTIMISED (1) ==============\n");
    ir_print_section(stdout);

  }
  if (optimization >= 2) {
    ir_start_optimization_2();

    fprintf(stdout, "\n=============== IR OPTIMISED (2) ==============\n");
    ir_print_section(stdout);
  }
  
  if (!print_to_file && (0 == strcmp("mips", stage) || optimization)) {
    mips_print_program(stdout);
  }
  output = fopen(output_name, "w");
  if (NULL == output) {
    fprintf(stdout, "Could not open output file %s: %s", optarg, strerror(errno));
    return 1;
  } else {
    mips_print_program(output);
    fclose(output);
  }

  return 0;
}

long int yylval;
