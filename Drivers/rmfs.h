#ifndef RMFS_H
#define RMFS_H
#include<stdint.h>
#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 20
#define MAX_FILE_SIZE 4096
#define FS_MAGIC 0x46534D53
typedef struct 
{
char name[MAX_FILENAME_LENGTH];
uint32_t size;
uint32_t dataoffset;
uint8_t used;
}file_entry_t;
typedef struct 
{
uint32_t magic;
uint32_t num_files;
file_entry_t files[MAX_FILES];
uint8_t data_pool[MAX_FILE_SIZE*MAX_FILES];
}rmfs_t;

void rmfs_init(void);
int create_files(const char *filename);
int delete_files(const char *filename);
int write_files(const char *filename,const char *data, uint32_t size);
int read_files(const char *filename,char *buffer,uint32_t buffer_size);
int list_files(char *output,uint32_t output_size);
int file_exists(const char *filename);
uint32_t get_file_size(const char *filename);

#endif
