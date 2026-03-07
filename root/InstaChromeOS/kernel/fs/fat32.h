#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

/* Filesystem node */
typedef struct fs_node {
    char name[256];
    int is_directory;
    struct fs_node* parent;
    struct fs_node* children[100];
    int child_count;
    char content[4096];
} fs_node_t;

/* Function declarations */
void init_fs(void);
void fs_print_tree(void);
void fs_list_current(void);
void fs_change_dir(const char* path);
void fs_create_file(const char* name);
void fs_create_dir(const char* name);
void fs_get_path(char* buffer, int size);

#endif