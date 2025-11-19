#include "rmfs.h"
#include "string.h"

static rmfs_t *filesystem = (rmfs_t*)0x100000; // 1MB mark for filesystem

void rmfs_init(void) {
    // Check if filesystem needs initialization
    if (filesystem->magic != FS_MAGIC) {
        filesystem->magic = FS_MAGIC;
        filesystem->num_files = 0;
        
        // Initialize all file entries as unused
        for (int i = 0; i < MAX_FILES; i++) {
            filesystem->files[i].used = 0;
            filesystem->files[i].name[0] = '\0';
            filesystem->files[i].size = 0;
            filesystem->files[i].dataoffset = 0;
        }
    }
}

int create_files(const char *filename) {
    if (mystrlen(filename) >= MAX_FILENAME_LENGTH || mystrlen(filename) == 0) {
        return -1; // Invalid filename
    }
    
    // Check if file already exists
    if (file_exists(filename)) {
        return -2; // File exists
    }
    
    // Find free file entry
    for (int i = 0; i < MAX_FILES; i++) {
        if (!filesystem->files[i].used) {
            // Initialize file entry
            mystrcpy(filesystem->files[i].name, filename);
            filesystem->files[i].used = 1;
            filesystem->files[i].size = 0;
            filesystem->files[i].dataoffset = i * MAX_FILE_SIZE;
            filesystem->num_files++;
            return 0; // Success
        }
    }
    
    return -3; // No free file entries
}

int delete_files(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem->files[i].used && mystrcmp(filesystem->files[i].name, filename) == 0) {
            filesystem->files[i].used = 0;
            filesystem->files[i].name[0] = '\0';
            filesystem->files[i].size = 0;
            filesystem->num_files--;
            return 0; // Success
        }
    }
    return -1; // File not found
}

int write_files(const char *filename, const char *data, uint32_t size) {
    if (size > MAX_FILE_SIZE) {
        return -1; // File too large
    }
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem->files[i].used && mystrcmp(filesystem->files[i].name, filename) == 0) {
            // Copy data to file system
            mymemcpy(&filesystem->data_pool[filesystem->files[i].dataoffset], data, size);
            filesystem->files[i].size = size;
            return 0; // Success
        }
    }
    return -2; // File not found
}

int read_files(const char *filename, char *buffer, uint32_t buffer_size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem->files[i].used && mystrcmp(filesystem->files[i].name, filename) == 0) {
            if (buffer_size < filesystem->files[i].size) {
                return -1; // Buffer too small
            }
            mymemcpy(buffer, &filesystem->data_pool[filesystem->files[i].dataoffset], filesystem->files[i].size);
            return filesystem->files[i].size; // Return bytes read
        }
    }
    return -2; // File not found
}

int list_files(char *output, uint32_t output_size) {
    if (filesystem->num_files == 0) {
        mystrcpy(output, "No files found.\n");
        return 0;
    }
    
    char temp[64];
    output[0] = '\0';
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem->files[i].used) {
            mystrcpy(temp, filesystem->files[i].name);
            mystrcat(temp, " (");
            
            // Convert size to string
            char size_str[16];
            uint32_t size = filesystem->files[i].size;
            int pos = 0;
            
            if (size == 0) {
                size_str[pos++] = '0';
            } else {
                char temp_str[16];
                int temp_pos = 0;
                while (size > 0) {
                    temp_str[temp_pos++] = '0' + (size % 10);
                    size /= 10;
                }
                for (int j = temp_pos - 1; j >= 0; j--) {
                    size_str[pos++] = temp_str[j];
                }
            }
            size_str[pos] = '\0';
            
            mystrcat(temp, size_str);
            mystrcat(temp, " bytes)\n");
            
            // Check if we have space
            if (mystrlen(output) + mystrlen(temp) < output_size) {
                mystrcat(output, temp);
            } else {
                break;
            }
        }
    }
    
    return 0;
}

int file_exists(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem->files[i].used && mystrcmp(filesystem->files[i].name, filename) == 0) {
            return 1;
        }
    }
    return 0;
}

uint32_t get_file_size(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem->files[i].used && mystrcmp(filesystem->files[i].name, filename) == 0) {
            return filesystem->files[i].size;
        }
    }
    return 0;
}