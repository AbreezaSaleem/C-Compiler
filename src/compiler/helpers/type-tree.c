#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "./util-functions.c"
#include "./type-tree.h"
#include "../type.h"
#include "../symbol.h"
#include "../node.h"

int is_function_decl = 0;
int is_function_decl_pointer = 0;
struct node *variable_name;

void print_type_tree(struct type_tree *type_tree) {
  if (NULL == type_tree) {
    return;
  }


  struct type_chain *type_chain_temp = type_tree->type_chain;
  assert(NULL != type_tree->type_chain);

  printf("\n================ TYPE TREE ==============\n");

  printf("Type:");
  // print type
  while(NULL != type_chain_temp) {
    switch(type_chain_temp->type->kind) {
      case NODE_POINTER:
        printf("          Pointer-->");
        break;
      case NODE_ARRAY: 
        printf("          Array-->");
        break;
      case NODE_FUNCTION_DECL: 
        printf("          Function decl-->");
        break;
      case NODE_DECLARATION_STATEMENT: 
        printf("          Decl-->");
        break;
      case NODE_IDENTIFIER: 
        printf("          %s-->", type_chain_temp->type->data.identifier.name);
        break;
      case NODE_NUMBER: 
        printf("          %ld-->", type_chain_temp->type->data.number.value);
        break;
      case NODE_TYPE: {
        char *type = get_type_string(type_chain_temp->type->data.type.type_node);
        if (NULL == type_chain_temp->next) {
          printf("          (%s) \n", type);
        } else {
          printf("          %s", type);
        }
        type = NULL;
        free(type);
        break;
      }
      default:
        printf("Handler not found %d\n", type_chain_temp->type->kind);
        assert(0);
        break;
    }
    type_chain_temp = type_chain_temp->next;
  }

  // print name
  printf("\nIdentifier:    %s \n", type_tree->identifier->data.identifier.name);


  free(type_chain_temp);
}


const char* get_return_type(struct type_tree *type_tree) {
  struct type_chain *type_chain_temp = type_tree->type_chain;

  while(NULL != type_chain_temp->next) {
    type_chain_temp = type_chain_temp->next;
  }

  const char* return_type = "";

  switch(type_chain_temp->type->kind) {
    case NODE_POINTER: {
      assert(NODE_TYPE == type_chain_temp->type->data.pointer.points_to->kind);
      char return_type_cat[100] = "pointer(";
      strcat(return_type_cat, get_type_string(type_chain_temp->type->data.pointer.points_to->data.type.type_node));
      strcat(return_type_cat, ")");
      return_type = return_type_cat;
      return_type = strdup(return_type_cat);
      // should free memory... but at what point?
      // the variable is inaccessable after this function returns
      break;
    }
    case NODE_TYPE:
      return_type = get_type_string(type_chain_temp->type->data.type.type_node);
      break;
    default: 
      printf("Unknown return type %d\n", type_chain_temp->type->kind);
      assert(0);
      break;
  }

  /* cleanup */
  type_chain_temp = NULL;
  free(type_chain_temp);

  return return_type;
}

struct type *get_return_type_struct(struct type_tree *type_tree) {
  assert(NULL != type_tree);

  return type_tree->type;
}

signed long get_array_size(struct type_tree *type_tree) {
  assert(NULL != type_tree);

  signed long size = 0;
  struct type_chain *type_chain_temp = type_tree->type_chain;

  while(NULL != type_chain_temp) {
    if (NODE_ARRAY == type_chain_temp->type->kind) {
      if (NODE_NUMBER == type_chain_temp->next->type->kind) {
        size = type_chain_temp->next->type->data.number.value;
      }
    }
    type_chain_temp = type_chain_temp->next;
  }

  /* cleanup */
  type_chain_temp = NULL;
  free(type_chain_temp);

  return size;
}

int does_array_contain_pointer(struct type_tree *type_tree) {
  assert(NULL != type_tree);

  int pointer = 0;
  struct type_chain *type_chain_temp = type_tree->type_chain;

  while(NULL != type_chain_temp) {
    if (NODE_POINTER == type_chain_temp->type->kind) {
      pointer = 1;
    }
    type_chain_temp = type_chain_temp->next;
  }

  /* cleanup */
  type_chain_temp = NULL;
  free(type_chain_temp);

  return pointer;
}

/* Compare function signatures */
int compare_type_trees(struct type_tree *type_tree_1, struct type_tree *type_tree_2) {
  int identical = 0;

  if (NULL == type_tree_1 || NULL == type_tree_2) {
    return 1;
  }

  struct type_chain *type_temp_1 = type_tree_1->type_chain;
  struct type_chain *type_temp_2 = type_tree_2->type_chain;

  if (
    0 != strcmp(
      type_tree_1->identifier->data.identifier.name,
      type_tree_2->identifier->data.identifier.name
    )) {
    return 1;
  }

  while(NULL != type_temp_1) {
    if (NULL == type_temp_2) {
      return 1;
    }

    /* we are ignorning the identifiers in function argument lists! */
    if (type_temp_1->type->kind == NODE_IDENTIFIER) {
      type_temp_1 = type_temp_1->next;
    }
    if (type_temp_2->type->kind == NODE_IDENTIFIER) {
      type_temp_2 = type_temp_2->next;
    }

    if (type_temp_1->type->kind != type_temp_2->type->kind) {
      return 1;
    }

    /* If the type is TYPE you need to go further in to compare the actual types */
    if (type_temp_1->type->kind == NODE_TYPE) {
      char *type_1 = get_type_string(type_temp_1->type->data.type.type_node);
      char *type_2 = get_type_string(type_temp_2->type->data.type.type_node);

      if (0 != strcmp(type_1, type_2)) {
        type_1 = NULL;
        type_2 = NULL;
        free(type_1);
        free(type_2);
        return 1;
      }
    }

    if (
      NODE_POINTER == type_temp_1->type->kind && NODE_POINTER == type_temp_2->type->kind
      && NULL == type_temp_1->next && NULL == type_temp_2->next
    ) {
      // compare pointer values
      char *type_1 = get_type_string(type_temp_1->type->data.pointer.points_to->data.type.type_node);
      char *type_2 = get_type_string(type_temp_2->type->data.pointer.points_to->data.type.type_node);
      if (0 != strcmp(type_1, type_2)) {
        type_1 = NULL;
        type_2 = NULL;
        free(type_1);
        free(type_2);
        return 1;
      }
    }

    type_temp_1 = type_temp_1->next;
    type_temp_2 = type_temp_2->next;
  }


  /* meaning the first tree was shorter than the second one */
  if (NULL != type_temp_2) {
    return 1;
  }

  /* cleanup */
  type_tree_1 = NULL;
  type_tree_2 = NULL;
  free(type_tree_1);
  free(type_tree_2);

  return identical;
}

static void insert_at_bottom_of_type_chain(struct node *insert, struct type_tree *type_tree) {
  struct type_chain *type_chain_temp = type_tree->type_chain;
  struct type_chain *type_chain_new = malloc(sizeof(struct type_chain));
  assert(NULL != type_chain_new);
  type_chain_new->type = insert;
  type_chain_new->next = NULL;

  if (NULL == type_chain_temp) {
    /* First entry */
    type_tree->type_chain = type_chain_new;
    return;
  }

  while(NULL != type_chain_temp->next) {
    type_chain_temp = type_chain_temp->next;
  }

  type_chain_temp->next = type_chain_new;
}

void print_type_tree_assignment_format_handler(struct type_chain *type_chain, struct type_tree *type_tree) {
  if (NULL == type_chain) {
    return;
  }

  if (NULL == type_chain->next && is_function_decl == 1) {
    return;
  }
  
  switch(type_chain->type->kind) {
    case NODE_POINTER:
      printf("pointer(");
      print_type_tree_assignment_format_handler(type_chain->next, type_tree);
      printf(")");
      return;
    case NODE_ARRAY:
      printf("array(");
      print_type_tree_assignment_format_handler(type_chain->next, type_tree);
      printf(")");
      return;
    case NODE_FUNCTION_DECL:
      is_function_decl = 1;
      printf("function(");
      printf("%s, ", get_return_type(type_tree));
      printf("[");
      print_type_tree_assignment_format_handler(type_chain->next, type_tree);
      printf("])");
      is_function_decl = 0;
      return;
    case NODE_DECLARATION_STATEMENT:
      print_type_tree_assignment_format_handler(type_chain->next, type_tree);
      return;
    case NODE_IDENTIFIER:
      if (type_tree->type->kind == TYPE_ARRAY || type_tree->type->kind == TYPE_POINTER) {
        printf("%s, ", type_chain->type->data.identifier.name);
      }
      print_type_tree_assignment_format_handler(type_chain->next, type_tree);
      return;
    case NODE_NUMBER: {
      /* for incomplete arrays */
      if (type_chain->type->data.number.value == _SIGNED_LONG_MIN) {
        printf("N/A, ");
      } else {
        printf("%ld, ", type_chain->type->data.number.value);
      }
      print_type_tree_assignment_format_handler(type_chain->next, type_tree);
      return;
    }
    case NODE_TYPE: {
      char *type = get_type_string(type_chain->type->data.type.type_node);
      printf("(%s)", type);
      if (NULL != type_chain->next) {
        printf(", ");
      }
      type = NULL;
      free(type);
      print_type_tree_assignment_format_handler(type_chain->next, type_tree);
      return;
    }
    default:
      return;
  }
}


void print_type_tree_assignment_format(struct type_tree *type_tree) {
  if (NULL == type_tree) {
    return;
  }

  struct type_chain *type_chain_temp = type_tree->type_chain;
  assert(NULL != type_tree->type_chain);

  printf("%s -> ", type_tree->identifier->data.identifier.name);

  print_type_tree_assignment_format_handler(type_chain_temp, type_tree);

  type_chain_temp = NULL;
}

/**
 * Here we recursively traverse the declaration subtree
 * and generate its Type Tree as we go
*/
void traverse_decl_subtree(
	struct node *declaration, struct type_tree *type_tree
) {
	if (NULL == declaration) return;

	switch(declaration->kind) {
    case NODE_NUMBER:
    case NODE_IDENTIFIER:
    case NODE_TYPE:
			/* this is for array sizes + function decl/call arguments */
      insert_at_bottom_of_type_chain(declaration, type_tree);
			return;
    case NODE_POINTER: {
      if (NODE_IDENTIFIER == declaration->data.pointer.points_to->kind && is_function_decl == 0) {
        /* since function can have pointers are arguments */
        type_tree->identifier = declaration->data.pointer.points_to;
      } else {
        /* traverse here if the name is preceeded by a pointer */
        traverse_decl_subtree(declaration->data.pointer.points_to, type_tree);
      }
      if (NODE_FUNCTION_DECL != declaration->data.pointer.points_to->kind) {
        insert_at_bottom_of_type_chain(declaration, type_tree);
      } else {
        is_function_decl_pointer = 1;
      }
      return;
    }
    case NODE_ARRAY: {
      if (NODE_IDENTIFIER == declaration->data.array.name->kind && is_function_decl == 0) {
        type_tree->identifier = declaration->data.array.name;
      } else {
        /* traverse here if the name is preceeded by a pointer */
        traverse_decl_subtree(declaration->data.array.name, type_tree);
      }
      insert_at_bottom_of_type_chain(declaration, type_tree);
      struct expression_result result = evaluate_expression(declaration->data.array.size);
      if (result.does_contain_identifier) {
        traverse_decl_subtree(declaration->data.array.size, type_tree);
      } else {
        char str[30];
        sprintf(str, "%ld", result.value);
        // Setting the length arg to 0 because its not getting used inside the function!
        struct node *array_size = node_number(declaration->location, str, 0);
        traverse_decl_subtree(array_size, type_tree);
      }
      return;
    }
    case NODE_FUNCTION_DECL: {
      if (NODE_IDENTIFIER == declaration->data.function.name->kind) {
        type_tree->identifier = declaration->data.function.name;
        is_function_decl = 1;
        variable_name = declaration->data.function.name;
      } else {
        /* traverse here if the name is preceeded by a pointer */
        traverse_decl_subtree(declaration->data.function.name, type_tree);
      }
      insert_at_bottom_of_type_chain(declaration, type_tree);
      traverse_decl_subtree(declaration->data.function.argument_list, type_tree);
      return;
    }
    case NODE_EXPRESSION_LIST_STATEMENT: 
      traverse_decl_subtree(declaration->data.expression_list_statement.self, type_tree);
      traverse_decl_subtree(declaration->data.expression_list_statement.next, type_tree);
      return;
    case NODE_EXPRESSION_STATEMENT: 
      traverse_decl_subtree(declaration->data.expression_statement.expression, type_tree);
      return;
    case NODE_ARGUMENTS_LIST: 
      /* for functions */
      traverse_decl_subtree(declaration->data.arguments_list.self, type_tree);
      traverse_decl_subtree(declaration->data.arguments_list.next, type_tree);
      return;
    case NODE_DECLARATION_STATEMENT: 
      /* for functions */
      insert_at_bottom_of_type_chain(declaration, type_tree);
      traverse_decl_subtree(declaration->data.declaration_statement.declaration, type_tree);
      traverse_decl_subtree(declaration->data.declaration_statement.type, type_tree);
      return;
    default:
			/* We don't want to create nodes in Type Tree for the rest of the Node Types */
			return;
  }
}

struct type_tree *create_type_tree(struct node *declaration_type, struct node *declaration_comma) {

	struct type_tree *type_tree = malloc(sizeof(struct type_tree));
  type_tree->type_chain = NULL;
	assert(NULL != type_tree);

	/* After the 'traverse_decl_subtree' has done executing
	* we must have reached the end of the declaration part of the statement
	* we can now append the type to the leaf of the type tree
	* and set the name we get from the function in our type tree as well
	*/

  enum type_kind kind = TYPE_BASIC;

  if (NODE_IDENTIFIER == declaration_comma->kind) {
    type_tree->identifier = declaration_comma;
  } else {
	  traverse_decl_subtree(declaration_comma, type_tree);

    /* this could have been done in the parser */
    switch(type_tree->type_chain->type->kind) {
      case NODE_ARRAY:
        kind = TYPE_ARRAY;
        break;
      case NODE_FUNCTION_DECL: 
        kind = TYPE_FUNCTION;
        break;
      case NODE_POINTER: 
        kind = TYPE_POINTER;
        break;
      default:
        break;
    }
  }

  /* assign type node */
  /* function return can be pointers */
  struct type *declaration_type_copy = NULL;
  if (NODE_POINTER == declaration_type->kind) {
    declaration_type_copy = type_basic(
      declaration_type->data.pointer.points_to->data.type.type_node->data.basic.is_unsigned,
      declaration_type->data.pointer.points_to->data.type.type_node->data.basic.datatype
    );
  } else {
    declaration_type_copy = type_basic(
      declaration_type->data.type.type_node->data.basic.is_unsigned,
      declaration_type->data.type.type_node->data.basic.datatype
    );
  }
  declaration_type_copy->kind = kind;
  type_tree->type = declaration_type_copy;

  /*
    if its pointer then its pointer to function return type
    and in that case is_function_decl_pointer should be true as well

    here we want to insert pointer to return types... and print it properly too
  */

  if (is_function_decl_pointer == 1) {
    struct node *type = node_pointer(declaration_type->location, declaration_type);
    insert_at_bottom_of_type_chain(type, type_tree);
    is_function_decl_pointer = 0;
  } else {
    insert_at_bottom_of_type_chain(declaration_type, type_tree);
  }

  is_function_decl = 0;

	return type_tree;
}
