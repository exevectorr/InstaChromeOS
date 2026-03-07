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
    char content[4096];  /* File content (for .txt files) */
    uint32_t content_size; /* Size of content in bytes */
} fs_node_t;

/* Function declarations */
void init_fs(void);
void fs_print_tree(void);
void fs_list_current(void);
void fs_change_dir(const char* path);
void fs_create_file(const char* name);
void fs_create_dir(const char* name);
void fs_get_path(char* buffer, int size);
fs_node_t* fs_find(const char* path);
int fs_write_file(const char* path, const char* content);  /* New */
int fs_read_file(const char* path, char* buffer, uint32_t size);  /* New */
int fs_edit_file(const char* path, const char* new_content);  /* New */
void fs_display_file(const char* path);  /* New */

#endif