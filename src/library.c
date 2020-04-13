/* Name, library.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */

#include "library.h"

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/* Define find_parent_pointer here */

tree_node_t *g_song_library = NULL;

tree_node_t **find_parent_pointer(tree_node_t **root, const char *song_name) {
  assert(root != NULL);
  tree_node_t *curr_node = *root;
  while (curr_node != NULL) {
    if (strcmp(curr_node->song_name, song_name) > 0) {
      curr_node = curr_node->left_child;
    } else if (strcmp(curr_node->song_name, song_name) < 0) {
      curr_node = curr_node->right_child;
    } else {
      *root = curr_node;
      return root;
    }
  }
  return NULL;
}

/* Define tree_insert here */

int tree_insert(tree_node_t **root, tree_node_t *new_node) {
  assert(root != NULL);
  tree_node_t *curr_node = *root;
  while (curr_node != NULL) {
    if (strcmp(curr_node->song_name, new_node->song_name) > 0) {
      if (curr_node->left_child != NULL) {
        curr_node = curr_node->left_child;
      } else {
        curr_node->left_child = new_node;
        return INSERT_SUCCESS;
      }
    } else if (strcmp(curr_node->song_name, new_node->song_name) < 0) {
      if (curr_node->right_child != NULL) {
        curr_node = curr_node->right_child;
      } else {
        curr_node->right_child = new_node;
        return INSERT_SUCCESS;
      }
    } else {
      return DUPLICATE_SONG;
    }
  }

  *root = new_node;
  return INSERT_SUCCESS;
}

/* Define remove_song_from_tree here */

int remove_song_from_tree(tree_node_t **root, const char *song_name) {
  assert(root != NULL);
  assert(song_name != NULL);

  tree_node_t *parent_node = NULL;
  tree_node_t *curr_node = *root;
  tree_node_t *temp_left = NULL;
  tree_node_t *temp_right = NULL;

  while (curr_node != NULL) {
    if (strcmp(curr_node->song_name, song_name) > 0) {
      parent_node = curr_node;
      curr_node = curr_node->left_child;
    } else if (strcmp(curr_node->song_name, song_name) < 0) {
      parent_node = curr_node;
      curr_node = curr_node->right_child;
    } else {
      if (parent_node == NULL) {
        *root = NULL;
      } else if (parent_node->left_child == curr_node) {
        parent_node->left_child = NULL;
      } else if (parent_node->right_child == curr_node) {
        parent_node->right_child = NULL;
      }

      temp_left = curr_node->left_child;
      temp_right = curr_node->right_child;
      free_node(curr_node);

      if (temp_left != NULL) {
        tree_insert(root, temp_left);
      }
      if (temp_right != NULL) {
        tree_insert(root, temp_right);
      }
      return DELETE_SUCCESS;
    }
  }
  return SONG_NOT_FOUND;
}

/* Define free_node here */

void free_node(tree_node_t *node) {
  assert(node != NULL);
  free_song(node->song);
  free(node);
}

/* Define print_node here */

void print_node(tree_node_t *node, FILE *fp) {
  fprintf(fp, "%s\n", node->song_name);
}

/* Define traverse_pre_order here */

void traverse_pre_order(tree_node_t *node, void *data, traversal_func_t func) {
  func(node, data);

  if (node->left_child != NULL) {
    traverse_pre_order(node->left_child, data, func);
  }

  if (node->right_child != NULL) {
    traverse_pre_order(node->right_child, data, func);
  }
}

/* Define traverse_in_order here */

void traverse_in_order(tree_node_t *node, void *data, traversal_func_t func) {
  if (node->left_child != NULL) {
    traverse_in_order(node->left_child, data, func);
  } else {
    func(node, data);
    if (node->right_child != NULL) {
      traverse_in_order(node->right_child, data, func);
    }
  }
}

/* Define traverse_post_order here */

void traverse_post_order(tree_node_t *node, void *data, traversal_func_t func) {
  if (node->left_child != NULL) {
    traverse_post_order(node->left_child, data, func);
  }
  if (node->right_child != NULL) {
    traverse_post_order(node->right_child_data, func);
  }
  func(node, data);
}

/* Define free_library here */

void free_library(tree_node_t *tree) {
  if (tree->left_child != NULL) {
    free_library(tree->left_child);
  }

  if (tree->right_child != NULL) {
    free_library(tree->right_child);
  }

  free_node(tree);
}

/* Define write_song_list here */

void write_song_list(FILE *fp, tree_node_t *tree) {
  traverse_in_order(tree, fp, (void *) print_node);
}

/* Define make_library here */

void make_library(const char *directory) {
  
}
