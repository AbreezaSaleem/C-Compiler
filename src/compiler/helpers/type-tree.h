#ifndef _TYPETREE_H
#define _TYPETREE_H


#include "../type.h"

struct type_chain {
  struct node *type;
  struct type_chain *next;
};

struct type_tree {
  struct node *identifier;
  struct type *type;
  enum type_kind type_kind;
  struct type_chain *type_chain;
};

void print_type_tree(struct type_tree *type_tree);
const char* get_return_type(struct type_tree *type_tree);
struct type *get_return_type_struct(struct type_tree *type_tree);
signed long get_array_size(struct type_tree *type_tree);
int does_array_contain_pointer(struct type_tree *type_tree);
int compare_type_trees(struct type_tree *type_tree_1, struct type_tree *type_tree_2);
void print_type_tree_assignment_format_handler(struct type_chain *type_chain, struct type_tree *type_tree);
void print_type_tree_assignment_format(struct type_tree *type_tree);
struct type_tree *create_type_tree(struct node *declaration_type, struct node *declaration_comma);

#endif /* _TYPETREE_H */
