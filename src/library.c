/* Name, library.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */

#include "library.h"

#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ftw.h>

/* Define find_parent_pointer here */

tree_node_t *g_song_library = NULL;

/*
 * find_parent_pointer
 * searches the associated tree
 * for the song_name
 */

tree_node_t **find_parent_pointer(tree_node_t **root, const char *song_name) {
  assert(root != NULL);
  assert(song_name != NULL);

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
} /* find_parent_pointer() */

/*
 * tree_insert
 * takes the given node
 * and inserts it into the associated tree
 */

int tree_insert(tree_node_t **root, tree_node_t *new_node) {
  assert(root != NULL);
  assert(new_node != NULL);

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
} /* tree_insert() */

/*
 * remove_song_from_tree
 * takes a specific song_name
 * and removes it from the tree
 */

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
} /* remove_song_from_tree() */

/*
 * free_node takes an
 * associated node and frees
 * all allocated memory
 */

void free_node(tree_node_t *node) {
  assert(node != NULL);
  free_song(node->song);
  free(node);
} /* free_node() */

/*
 * print_node
 * takes the given node and fp
 * printing the song_name to the document
 */

void print_node(tree_node_t *node, FILE *fp) {
  if (node == NULL) {
    return;
  }
  fprintf(fp, "%s\n", node->song_name);
} /* print_node() */

/*
 * traverse_pre_order
 * takes the given tree
 * and applies the function in pre_order
 */

void traverse_pre_order(tree_node_t *node, void *data, traversal_func_t func) {
  assert(data != NULL);
  func(node, data);
  if (node == NULL) {
    return;
  }
  if (node->left_child != NULL) {
    traverse_pre_order(node->left_child, data, func);
  }
  if (node->right_child != NULL) {
    traverse_pre_order(node->right_child, data, func);
  }
} /* traverse_pre_order() */

/*
 * traverse_in_order
 * takes the given tree
 * and applies the function in order
 */

void traverse_in_order(tree_node_t *node, void *data, traversal_func_t func){
  assert(data != NULL);
  if (node == NULL) {
    return;
  }
  if (node->left_child != NULL) {
    traverse_in_order(node->left_child, data, func);
  }
  func(node, data);
  if (node->right_child != NULL) {
    traverse_in_order(node->right_child, data, func);
  }
} /* traverse_in_order() */

/*
 * traverse_post_order
 * takes the given tree
 * and applies the funcion post order
 */

void traverse_post_order(tree_node_t *node, void *data, traversal_func_t func) {
  assert(data != NULL);
  if (node == NULL) {
    return;
  }
  if (node->left_child != NULL) {
    traverse_post_order(node->left_child, data, func);
  }
  if (node->right_child != NULL) {
    traverse_post_order(node->right_child, data, func);
  }
  func(node, data);
} /* traverse_post_order() */

/*
 * free_library
 * fress all memory on the
 * associated tree
 */

void free_library(tree_node_t *tree) {
  if (tree == NULL) {
    return;
  }

  if (tree->left_child != NULL) {
    free_library(tree->left_child);
  }

  if (tree->right_child != NULL) {
    free_library(tree->right_child);
  }

  free_node(tree);
} /* free_library() */

/*
 * write_song_list
 * takes the given tree and writes
 * all song_names to the given document
 */

void write_song_list(FILE *fp, tree_node_t *tree) {
  traverse_in_order(tree, fp, (void *) print_node);
} /* write_song_list() */

/*
 * ftw_insert is a supporter
 * function for the ftw passthrough
 * in make_library
 */

int ftw_insert(const char *path, const struct stat *sb, int typeflag) {
  char *extension = strrchr(path, '.');
  if (extension == NULL) {
    return 0;
  }
  extension = extension + 1;
  if ((typeflag != FTW_F) || (strcmp(extension, "mid") != 0)) {
    return 0;
  }
  if (find_parent_pointer(&g_song_library, strrchr(path, '/') + 1) != NULL) {
    tree_node_t **end = find_parent_pointer(&g_song_library,
      strrchr(path, '/') + 1);
    if (strcmp((*end)->song->path, path) == 0) {
      return 2;
    }
    return 1;
  }
  tree_node_t *new_node = malloc(sizeof(tree_node_t));
  new_node->left_child = NULL;
  new_node->right_child = NULL;
  new_node->song = parse_file(path);
  new_node->song_name = strrchr(new_node->song->path, '/') + 1;
  tree_insert(&g_song_library, new_node);
  return 0;
} /* ftw_insert() */

/*
 * make_library
 * takes a given directory
 * creating a library of midi files
 */

void make_library(const char *path) {
  int status = 0;
  do {
    status = ftw(path, (void *) ftw_insert, 1);
    assert(status != 1);
  } while ((status != 2) && (status != -1));
} /* make_library() */
