#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "types.h"
#include "lib.h"

#define filename_size 32
#define max_num_dentries 63
#define blocks_array_size 1023
#define blocksize 4096

unsigned int file_mod_address; //starting address of file module
unsigned int num_dir_reads; //keep track of the number of directories read

//dentry
typedef struct{
	int8_t filename[filename_size]; //file name length
	unsigned int file_type;
	unsigned int inode_num;
	uint8_t reserved[24]; //24 reserved spots
} dentry_t;

//bootblock
typedef struct{
	unsigned int num_dir_entries;
	unsigned int num_inodes;
	unsigned int num_data_blocks;
	uint8_t reserved[52]; //52 reserved spots
	dentry_t dentries[max_num_dentries]; //63 possible directory entries
} bootblock_t;


//inode block
typedef struct{
	unsigned int file_size;
	unsigned int blocks[blocks_array_size];
} inode_t;

bootblock_t* bootblock_ptr;

//initializes the filesystem
void filesystem_init(uint32_t start_address);

//creates dentry using name of file -- helper function
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

//creates dentry using index of file -- helper function
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

//reads file data into buffer -- helper function
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

//opens file
int32_t file_open(const uint8_t * filename);

//closes file
int32_t file_close(int32_t fd);

//reads file into buffer
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

//writes file
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

//opens directory
int32_t dir_open(const uint8_t * filename);

//closes directory
int32_t dir_close(int32_t fd);

//reads directory
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

//writes directory
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

//gets size of file using index of file
int32_t filesize_debug(uint32_t index);


#endif
