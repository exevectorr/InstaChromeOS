#include "fat32.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include "../mm/pmm.h"
#include <stddef.h>

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
    node->content_size = 0;
    
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

/* Print the entire filesystem tree in the original format */
void fs_print_tree(void) {
    if(!root) return;
    
    screen_write("\n");
    screen_write("Root/\n");
    
    /* Find InstaChromeOS directory */
    fs_node_t* instachrome = NULL;
    for(int i = 0; i < root->child_count; i++) {
        if(strcmp(root->children[i]->name, "InstaChromeOS") == 0) {
            instachrome = root->children[i];
            break;
        }
    }
    
    if(!instachrome) return;
    
    /* Print InstaChromeOS */
    screen_write("+-------InstaChromeOS\n");
    
    /* Print system directory */
    fs_node_t* system = NULL;
    for(int i = 0; i < instachrome->child_count; i++) {
        if(strcmp(instachrome->children[i]->name, "system") == 0) {
            system = instachrome->children[i];
            break;
        }
    }
    
    if(system) {
        screen_write("+             +---------system/\n");
        
        /* Print int_handler */
        fs_node_t* int_handler = NULL;
        for(int i = 0; i < system->child_count; i++) {
            if(strcmp(system->children[i]->name, "int_handler") == 0) {
                int_handler = system->children[i];
                break;
            }
        }
        
        if(int_handler) {
            screen_write("+             +            +----int_handler/\n");
            
            /* Print int_handler contents */
            for(int i = 0; i < int_handler->child_count; i++) {
                screen_write("+             +            +        +----");
                screen_write(int_handler->children[i]->name);
                screen_write("\n");
            }
        }
        
        /* Print other system files */
        for(int i = 0; i < system->child_count; i++) {
            fs_node_t* node = system->children[i];
            if(!node->is_directory) {
                if(strcmp(node->name, "OSsys.os") == 0) {
                    screen_write("+             +            +---- OSsys.os\n");
                }
                else if(strcmp(node->name, "Linker.hmk") == 0) {
                    screen_write("+             +            +---- Linker.hmk\n");
                }
                else if(strcmp(node->name, "instachrome.jl") == 0) {
                    screen_write("+             +            +---- instachrome.jl\n");
                }
                else {
                    /* New files in system directory */
                    screen_write("+             +            +---- ");
                    screen_write(node->name);
                    screen_write("\n");
                }
            }
        }
    }
    
    /* Print personal directory and its contents */
    fs_node_t* personal = NULL;
    for(int i = 0; i < instachrome->child_count; i++) {
        if(strcmp(instachrome->children[i]->name, "personal") == 0) {
            personal = instachrome->children[i];
            break;
        }
    }
    
    if(personal) {
        screen_write("+             +------ personal/\n");
        
        /* Show files in personal */
        for(int i = 0; i < personal->child_count; i++) {
            screen_write("+                  |    +---- ");
            screen_write(personal->children[i]->name);
            if(personal->children[i]->is_directory) {
                screen_write("/");
            }
            screen_write("\n");
        }
    }
    
    /* Print space directory and its contents */
    fs_node_t* space = NULL;
    for(int i = 0; i < instachrome->child_count; i++) {
        if(strcmp(instachrome->children[i]->name, "space") == 0) {
            space = instachrome->children[i];
            break;
        }
    }
    
    if(space) {
        screen_write("+             +------ space/\n");
        
        /* Show files in space */
        for(int i = 0; i < space->child_count; i++) {
            screen_write("+             +    +---- ");
            screen_write(space->children[i]->name);
            if(space->children[i]->is_directory) {
                screen_write("/");
            }
            screen_write("\n");
        }
    }
    
    /* Print tmp directory and its contents */
    fs_node_t* tmp = NULL;
    for(int i = 0; i < instachrome->child_count; i++) {
        if(strcmp(instachrome->children[i]->name, "tmp") == 0) {
            tmp = instachrome->children[i];
            break;
        }
    }
    
    if(tmp) {
        screen_write("+             +------ tmp/\n");
        
        /* Show files in tmp */
        for(int i = 0; i < tmp->child_count; i++) {
            screen_write("+                  |    +---- ");
            screen_write(tmp->children[i]->name);
            if(tmp->children[i]->is_directory) {
                screen_write("/");
            }
            screen_write("\n");
        }
    }
    
    /* Check for any new directories directly under InstaChromeOS */
    for(int i = 0; i < instachrome->child_count; i++) {
        fs_node_t* node = instachrome->children[i];
        if(node->is_directory && 
           strcmp(node->name, "system") != 0 &&
           strcmp(node->name, "personal") != 0 &&
           strcmp(node->name, "space") != 0 &&
           strcmp(node->name, "tmp") != 0) {
            
            screen_write("+             +------ ");
            screen_write(node->name);
            screen_write("/\n");
            
            /* Show contents of new directory */
            for(int j = 0; j < node->child_count; j++) {
                screen_write("+             +    +---- ");
                screen_write(node->children[j]->name);
                if(node->children[j]->is_directory) {
                    screen_write("/");
                }
                screen_write("\n");
            }
        }
    }
    
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
            if(child->content_size > 0) {
                screen_write(" (");
                int_to_str(child->content_size, (char*)child->content);  /* Reuse buffer temporarily */
                screen_write(" bytes)");
            }
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
    
    /* Handle absolute paths (starting with /) */
    if(path[0] == '/') {
        fs_node_t* target = root;
        const char* p = path + 1;
        char component[256];
        int comp_idx = 0;
        
        while(*p) {
            if(*p == '/') {
                component[comp_idx] = '\0';
                
                /* Find component in current target */
                int found = 0;
                for(int i = 0; i < target->child_count; i++) {
                    if(target->children[i]->is_directory && 
                       strcmp(target->children[i]->name, component) == 0) {
                        target = target->children[i];
                        found = 1;
                        break;
                    }
                }
                
                if(!found) {
                    screen_write("Directory '");
                    screen_write(path);
                    screen_write("' not found\n");
                    return;
                }
                
                comp_idx = 0;
                p++;
            } else {
                component[comp_idx++] = *p++;
            }
        }
        
        /* Handle last component */
        if(comp_idx > 0) {
            component[comp_idx] = '\0';
            int found = 0;
            for(int i = 0; i < target->child_count; i++) {
                if(target->children[i]->is_directory && 
                   strcmp(target->children[i]->name, component) == 0) {
                    target = target->children[i];
                    found = 1;
                    break;
                }
            }
            
            if(!found) {
                screen_write("Directory '");
                screen_write(path);
                screen_write("' not found\n");
                return;
            }
        }
        
        current_dir = target;
        screen_write("Changed to directory: ");
        screen_write(current_dir->name);
        screen_write("\n");
        return;
    }
    
    /* Handle relative paths */
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

/* Search for a file or directory */
fs_node_t* fs_find(const char* path) {
    if(!root) return NULL;
    
    if(path[0] == '/') {
        /* Absolute path */
        fs_node_t* current = root;
        char component[256];
        int comp_idx = 0;
        const char* p = path + 1;
        
        while(*p) {
            if(*p == '/' || *(p+1) == '\0') {
                if(*(p+1) == '\0') {
                    component[comp_idx++] = *p;
                }
                component[comp_idx] = '\0';
                
                /* Find component */
                int found = 0;
                for(int i = 0; i < current->child_count; i++) {
                    if(strcmp(current->children[i]->name, component) == 0) {
                        current = current->children[i];
                        found = 1;
                        break;
                    }
                }
                
                if(!found) return NULL;
                
                comp_idx = 0;
                p++;
            } else {
                component[comp_idx++] = *p++;
            }
        }
        
        return current;
    } else {
        /* Relative path - search from current_dir */
        if(!current_dir) return NULL;
        
        for(int i = 0; i < current_dir->child_count; i++) {
            if(strcmp(current_dir->children[i]->name, path) == 0) {
                return current_dir->children[i];
            }
        }
    }
    
    return NULL;
}

/* Write content to a file */
int fs_write_file(const char* path, const char* content) {
    fs_node_t* file = fs_find(path);
    
    if(!file || file->is_directory) {
        return -1;  /* File not found or is a directory */
    }
    
    /* Copy content (limit to 4095 bytes) */
    int i;
    for(i = 0; content[i] && i < 4095; i++) {
        file->content[i] = content[i];
    }
    file->content[i] = '\0';
    file->content_size = i;
    
    return 0;  /* Success */
}

/* Read content from a file */
int fs_read_file(const char* path, char* buffer, uint32_t size) {
    fs_node_t* file = fs_find(path);
    
    if(!file || file->is_directory) {
        return -1;  /* File not found or is a directory */
    }
    
    uint32_t i;
    for(i = 0; i < file->content_size && i < size - 1; i++) {
        buffer[i] = file->content[i];
    }
    buffer[i] = '\0';
    
    return i;  /* Return number of bytes read */
}

/* Edit file content (replace existing) */
int fs_edit_file(const char* path, const char* new_content) {
    return fs_write_file(path, new_content);  /* Same as write for now */
}

/* Display file content */
void fs_display_file(const char* path) {
    fs_node_t* file = fs_find(path);
    char buffer[64];
    
    if(!file) {
        screen_write("File not found: ");
        screen_write(path);
        screen_write("\n");
        return;
    }
    
    if(file->is_directory) {
        screen_write("Cannot display directory: ");
        screen_write(path);
        screen_write("\n");
        return;
    }
    
    screen_write("\n--- Content of ");
    screen_write(path);
    screen_write(" (");
    int_to_str(file->content_size, buffer);
    screen_write(buffer);
    screen_write(" bytes) ---\n");
    
    if(file->content_size == 0) {
        screen_write("[File is empty]\n");
    } else {
        screen_write(file->content);
        screen_write("\n");
    }
    
    screen_write("--- End of file ---\n");
}