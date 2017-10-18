#include "filesystem.h"
#include "types.h"
#include "lib.h"
#include "systemCalls.h"

extern uint32_t file_mod_address;


/*
 * filesystem_init
 *   DESCRIPTION: Initializes the file system by storing the start address for it from kernel.c and setting the bootblock
 *				  to point at that address.
 *   INPUTS: start_address - address to load file system into (from kernel.c)
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

void filesystem_init(uint32_t start_address)
{
	file_mod_address = start_address; //save copy
	bootblock_ptr = (bootblock_t*)file_mod_address; //get bootblock pointer
}


/*
 * read_dentry_by_name
 *   DESCRIPTION: Using the filename, it loads the filename, file_type, and inode number into dentry struct
 *   INPUTS: fname - the file name
 *			 dentry - dentry struct to fill in
 *   OUTPUTS: none
 *   RETURN VALUE: -1  - indicates that function failed
 *				    0  - indicates success
 *   SIDE EFFECTS: none
 */

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
	int i;

	//input file name is too long or NULL
	if(strlen((int8_t*)fname) > filename_size || fname == NULL)
	{
		return -1;
	}

	//for each directory entry
	for(i = 0; i < bootblock_ptr->num_dir_entries; i++)
	{
		//check for a file name match
		if(strncmp((int8_t*)bootblock_ptr->dentries[i].filename, (int8_t*)fname, filename_size) == 0)
		{
			//if found match, copy over entries into dentry
			strncpy((int8_t*)dentry->filename, bootblock_ptr->dentries[i].filename, filename_size);
			dentry->filename[filename_size] = '\0';
			dentry->file_type = bootblock_ptr->dentries[i].file_type;
			dentry->inode_num = bootblock_ptr->dentries[i].inode_num;
			return 0; //return 0 for success
		}
	}

	//no matches were found, return -1 for failure
	return -1;
}



/*
 * read_dentry_by_index
 *   DESCRIPTION: Using the index, it loads the filename, file_type, and inode number into dentry struct
 *   INPUTS: index - index of the file
 *			 dentry - dentry struct to fill in
 *   OUTPUTS: none
 *   RETURN VALUE: -1  - indicates that function failed
 *				    0  - indicates success
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
	//check for invalid index
	if((index < 0) || (index >= max_num_dentries))
	{
		return -1;
	}

	//copy over entries into dentry based on given index
	strncpy((int8_t*)dentry->filename, bootblock_ptr->dentries[index].filename, filename_size);
	dentry->filename[filename_size] = '\0';
	dentry->file_type = bootblock_ptr->dentries[index].file_type;
	dentry->inode_num = bootblock_ptr->dentries[index].inode_num;

	return 0; //return 0 for success
}


/*
 * read_data
 *   DESCRIPTION: Reads data blocks into given buffer
 *   INPUTS: inode - inode number 
 *			 offset - offset to start reading from
 *			 buf - buffer to read into
 *			 length - length to read
 *   OUTPUTS: none
 *   RETURN VALUE: -1  - indicates that function failed
 *				    i  - indicates success, return number of bytes read
 *   SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	int i;
	unsigned int address;
	unsigned int which_block; //keep track of which data black we're looking at
	unsigned int where_in_block; //keep track of where in the data block we're looking

	inode_t* inode_ptr = (inode_t*)(file_mod_address + blocksize*(inode + 1));

	which_block = offset / blocksize;
	where_in_block = offset % blocksize;

	//check that the given inode is within range
	if((inode < 0) || (inode >= bootblock_ptr->num_inodes))
	{
		return -1;
	}

	//check that offset did not surpass file_size
	if(offset > inode_ptr->file_size)
	{
		return 0;
	}

	//cut off length if passed paramaters
	if(offset + length> inode_ptr->file_size)
	{
		length = inode_ptr->file_size - offset;
	}
	
	//calculate first data block address
	address = file_mod_address + ((bootblock_ptr->num_inodes + 1) + (inode_ptr->blocks[which_block]))*blocksize;

	//ready to store bytes into buffer
	//only want to store up to the length given
	for(i = 0; i < length; i++)
	{
		//if reached end of the file -> done
		if(offset + i > inode_ptr->file_size)
		{
			return i;
		}

		//check if we reached the end of the data block
		if (where_in_block >= blocksize)
		{
			//end of data block, increment to next one
			which_block++;
			where_in_block = 0;

			//check that next data block is valid
			if (inode_ptr->blocks[which_block] >= bootblock_ptr->num_data_blocks)
			{
				return -1;
			}

		}

		//update the address
		address = file_mod_address + (bootblock_ptr->num_inodes + 1 + inode_ptr->blocks[which_block]) * blocksize;

		//store the byte into buffer
		buf[i] = (uint8_t) ((uint8_t *)address)[where_in_block];
		where_in_block++;
	}

	//return number of bytes read
	return i;
}


/*
 * file_open
 *   DESCRIPTION: opens file
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes to open
 *   OUTPUTS: none
 *   RETURN VALUE: always returns 0
 *   SIDE EFFECTS: none
 */
int32_t file_open(const uint8_t * filename)
{
	return 0;
}


/*
 * file_close
 *   DESCRIPTION: closes file
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes to close
 *   OUTPUTS: none
 *   RETURN VALUE: always returns 0
 *   SIDE EFFECTS: none
 */
int32_t file_close(int32_t fd)
{
	return 0;
}


/*
 * file_read
 *   DESCRIPTION: reads file into buffer
 *   INPUTS: fd - file descriptor index
 *			 buf - buffer to read into
 *			 nbytes - number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: Failure: -1
 *				   Success: Number of bytes read
 *   SIDE EFFECTS: none
 */
 int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
 {

 	int bytesRead;

 	//need pcb for inode_num and offset
 	pcb_t* temp_pcb = get_pcb();

 	//check if valid buf
 	if(buf == NULL)
 	{
 		return -1;
 	}
 	
 	bytesRead = read_data(temp_pcb->file_array[fd].inode_num, temp_pcb->file_array[fd].f_pos, buf, nbytes);
 	
 	//if not stdin or stdout
 	if(fd > 1)
 	{
 		//update file position
 		temp_pcb->file_array[fd].f_pos +=bytesRead;	
 	}

 	return bytesRead;
 }



/*
 * file_write
 *   DESCRIPTION: writes file
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: always returns -1
 *   SIDE EFFECTS: none
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
{
	return -1;
}


/*
 * dir_open
 *   DESCRIPTION: opens directory
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: always returns 0
 *   SIDE EFFECTS: none
 */
int32_t dir_open(const uint8_t * filename)
{
	return 0;
}


/*
 * dir_close
 *   DESCRIPTION: closes directory
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: always returns 0
 *   SIDE EFFECTS: none
 */
int32_t dir_close(int32_t fd)
{
	return 0;
}


/*
 * dir_read
 *   DESCRIPTION: reads directory
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0 - no directory to read
 *				   or length of directory name if directory read occured
 *   SIDE EFFECTS: none
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{
	int a, i;
	int8_t* buffer = (int8_t *)buf;

	if(num_dir_reads < bootblock_ptr->num_dir_entries)
	{
		//clear buffer
		for(i = 0; i < filename_size; i++)
		{
			buffer[i] = '\0';
		}

		strncpy((int8_t*)buffer, (const int8_t*)bootblock_ptr->dentries[num_dir_reads].filename, strlen(bootblock_ptr->dentries[num_dir_reads].filename));
		num_dir_reads++;
	}
	else
	{
		num_dir_reads = 0;
		return 0;
	}

	a = strlen((int8_t*)buffer);

	//if length of buffer is longer than filename size, return just filename size (32 characters)
	if(a > filename_size)
	{
		return filename_size;
	}
	
	return strlen((int8_t*)buffer);

}


/*
 * dir_write
 *   DESCRIPTION: writes to file
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes to open
 *   OUTPUTS: none
 *   RETURN VALUE: always returns -1
 *   SIDE EFFECTS: none
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
	return -1;
}


/*
 * filesize_debug
 *   DESCRIPTION: Finds the size of the file.
 *   INPUTS: index  - index of file
 *   OUTPUTS: none
 *   RETURN VALUE: Failure: -1
 *				   Success: file size
 *   SIDE EFFECTS: none
 */
int32_t filesize_debug(uint32_t index)
{

	dentry_t size_stuff;

	//fill in dentry
	if(read_dentry_by_index(index, &size_stuff) == -1)
	{
		return -1;
	}

	//check for invalid inode
	if (size_stuff.inode_num < 0 || size_stuff.inode_num >= bootblock_ptr->num_inodes)
	{
		return -1;
	}

	inode_t * inode_ptr = (inode_t *) (file_mod_address + (size_stuff.inode_num + 1)*blocksize);
	return (int32_t) inode_ptr->file_size;
}


