#ifndef UTILS_H
# define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
# include <yaml.h>

#include "../include/taskmaster.h"


/*
 * ============================================================
 * CONTRACT INTERFACE — DO NOT MODIFY
 * ============================================================
 * These prototypes form the public API of the config module.
 * They are used across the entire codebase and must remain
 * stable. Any change to a signature breaks all callers.
 *
 * Rules:
 *   - Never rename, remove, or change the signature of any
 *     function declared in this section.
 *   - New public functions may be added below the contract
 *     section only if they follow the same naming convention
 *     and are documented with their purpose and return value.
 * ============================================================
 */

int	write_error_fmt(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
int	write_success_fmt(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

int	validate_arg(int arg_count, char **arg_value);

/*
 * ============================================================
 * INTERNAL HELPERS — MODIFY WITH CARE
 * ============================================================
 * These functions are internal to the config module.
 * You may add, rename, or refactor them freely, but ensure
 * that the contract functions above continue to work correctly.
 * ============================================================
 */

bool	has_one_arg(int arg_count);
int 	has_yaml_extension(const char *filename);
int 	is_file_existing(const char *filename);
int 	is_file_readable(const char *filename);
int	    is_file_regular(const char *filename, struct stat* statbuf);
int     check_file_extension(const char *file, const char *file_ext);
int     process_file(char *file);
int     check_file_empty_v2(char *file);
int	has_correct_arg_nbr(int arg_count);

/*Functions to print something*/

void print_yaml_node(yaml_document_t *document, yaml_node_t *node, int indent);

/*Get and convert the types of the nodes*/

const char    *get_scalar_str(yaml_node_t *node);
int           get_scalar_int(yaml_node_t *node);
bool          get_scalar_bool(yaml_node_t *node);
mode_t        get_scalar_octal(yaml_node_t *node);
t_autorestart get_autorestart(yaml_node_t *node);
int           get_signal(yaml_node_t *node);

#endif