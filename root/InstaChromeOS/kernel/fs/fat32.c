#include "fat32.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include "../mm/pmm.h"
#include <stddef.h>  /* For NULL */

static fs_node_t* root = NULL;
static fs_node_t* current_dir = NULL;

/* Create a new filesystem node */
static fs_node_t* create_node(const char* name, int is_dir, fs_node_t* parent) {
    fs_node_t* node = (fs_node_t*)pmm_alloc_page();
    
    if(node == NULL) {
        return NULL;  /* Out of memory */
    }
    
    int i;
    for(i = 0; name[i] && i < 255; i++) {
        node->name[i] = name[i];
    }
    node->name[i] = '\0';
    
    node->is_directory = is_dir;
    node->parent = parent;
    node->child_count = 0;
    
    /* Initialize content for files */
    if(!is_dir) {
        node->content[0] = '\0';
    }
    
    return node;
}

/* Initialize filesystem with the required structure */
void init_fs(void) {
    /* Create root */
    root = create_node("Root", 1, NULL);
    current_dir = root;
    
    /* Create InstaChromeOS directory */
    fs_node_t* instachrome = create_node("InstaChromeOS", 1, root);
    root->children[root->child_count++] = instachrome;
    
    /* Create system directory */
    fs_node_t* system = create_node("system", 1, instachrome);
    instachrome->children[instachrome->child_count++] = system;
    
    /* Create int_handler directory */
    fs_node_t* int_handler = create_node("int_handler", 1, system);
    system->children[system->child_count++] = int_handler;
    
    /* Create int.ih file */
    fs_node_t* int_ih = create_node("int.ih", 0, int_handler);
    int_handler->children[int_handler->child_count++] = int_ih;
    
    /* Create OSsys.os file */
    fs_node_t* os_sys = create_node("OSsys.os", 0, system);
    system->children[system->child_count++] = os_sys;
    
    /* Create Linker.hmk file */
    fs_node_t* linker = create_node("Linker.hmk", 0, system);
    system->children[system->child_count++] = linker;
    
    /* Create instachrome.jl file */
    fs_node_t* jl = create_node("instachrome.jl", 0, system);
    system->children[system->child_count++] = jl;
    
    /* Create personal directory */
    fs_node_t* personal = create_node("personal", 1, instachrome);
    instachrome->children[instachrome->child_count++] = personal;
    
    /* Create space directory */
    fs_node_t* space = create_node("space", 1, instachrome);
    instachrome->children[instachrome->child_count++] = space;
    
    /* Create tmp directory */
    fs_node_t* tmp = create_node("tmp", 1, instachrome);
    instachrome->children[instachrome->child_count++] = tmp;
}

/* Print filesystem tree */
void fs_print_tree(void) {
    screen_write("\n");
    screen_write("Root/\n");
    screen_write("+-------InstaChromeOS\n");
    screen_write("+              +---------system/\n");
    screen_write("+              +            +----int_handler/\n");
    screen_write("+              +            +          +----int.ih\n");
    screen_write("+              +            +---- OSsys.os\n");
    screen_write("+              +            +---- Linker.hmk\n");
    screen_write("+              +            +---- instachrome.jl\n");
    screen_write("+              +------ personal/\n");
    screen_write("+              +------ space/\n");
    screen_write("+              +------ tmp/\n");
    screen_write("\n");
}

/* List current directory */
void fs_list_current(void) {
    if(!current_dir) return;
    
    screen_write("\nDirectory contents of ");
    screen_write(current_dir->name);
    screen_write(":\n");
    screen_write("----------------------------------------\n");
    
    for(int i = 0; i < current_dir->child_count; i++) {
        fs_node_t* child = current_dir->children[i];
        if(child->is_directory) {
            screen_write("[DIR]  ");
            screen_write(child->name);
            screen_write("/\n");
        } else {
            screen_write("[FILE] ");
            screen_write(child->name);
            screen_write("\n");
        }
    }
    screen_write("----------------------------------------\n");
}

/* Change directory */
void fs_change_dir(const char* path) {
    if(!current_dir) return;
    
    if(strcmp(path, "..") == 0) {
        if(current_dir->parent) {
            current_dir = current_dir->parent;
            screen_write("Changed to directory: ");
            screen_write(current_dir->name);
            screen_write("\n");
        } else {
            screen_write("Already at root directory\n");
        }
        return;
    }
    
    for(int i = 0; i < current_dir->child_count; i++) {
        fs_node_t* child = current_dir->children[i];
        if(child->is_directory && strcmp(child->name, path) == 0) {
            current_dir = child;
            screen_write("Changed to directory: ");
            screen_write(current_dir->name);
            screen_write("\n");
            return;
        }
    }
    
    screen_write("Directory '");
    screen_write(path);
    screen_write("' not found\n");
}

/* Create new file */
void fs_create_file(const char* name) {
    if(!current_dir) return;
    
    /* Check if already exists */
    for(int i = 0; i < current_dir->child_count; i++) {
        if(strcmp(current_dir->children[i]->name, name) == 0) {
            screen_write("File '");
            screen_write(name);
            screen_write("' already exists\n");
            return;
        }
    }
    
    fs_node_t* new_file = create_node(name, 0, current_dir);
    if(new_file) {
        current_dir->children[current_dir->child_count++] = new_file;
        screen_write("File '");
        screen_write(name);
        screen_write("' created successfully\n");
    } else {
        screen_write("Error: Out of memory\n");
    }
}

/* Create new directory */
void fs_create_dir(const char* name) {
    if(!current_dir) return;
    
    /* Check if already exists */
    for(int i = 0; i < current_dir->child_count; i++) {
        if(current_dir->children[i]->is_directory && 
           strcmp(current_dir->children[i]->name, name) == 0) {
            screen_write("Directory '");
            screen_write(name);
            screen_write("' already exists\n");
            return;
        }
    }
    
    fs_node_t* new_dir = create_node(name, 1, current_dir);
    if(new_dir) {
        current_dir->children[current_dir->child_count++] = new_dir;
        screen_write("Directory '");
        screen_write(name);
        screen_write("' created successfully\n");
    } else {
        screen_write("Error: Out of memory\n");
    }
}

/* Get current path */
void fs_get_path(char* buffer, int size) {
    if(!current_dir) {
        buffer[0] = '/';
        buffer[1] = '\0';
        return;
    }
    
    /* Build path from root */
    char temp[256][256];
    int depth = 0;
    fs_node_t* node = current_dir;
    
    while(node && node != root) {
        int i;
        for(i = 0; node->name[i] && i < 255; i++) {
            temp[depth][i] = node->name[i];
        }
        temp[depth][i] = '\0';
        depth++;
        node = node->parent;
    }
    
    /* Build path string */
    int pos = 0;
    buffer[pos++] = '/';
    
    for(int i = depth - 1; i >= 0; i--) {
        for(int j = 0; temp[i][j] && pos < size - 1; j++) {
            buffer[pos++] = temp[i][j];
        }
        if(i > 0 && pos < size - 1) {
            buffer[pos++] = '/';
        }
    }
    
    buffer[pos] = '\0';
}