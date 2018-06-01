#include "hw3.h"

void print_bitmap(int fd, struct ext2_super_block& super, struct ext2_group_desc& group)
{
    printf("Reading from image file " IMAGE ":\n"
           "Blocks count            : %u\n"
           "First non-reserved inode: %u\n"
           "No of blocks per group  : %u\n"
           "Size of block           : %u\n",
           super.s_blocks_count,
           super.s_first_ino,
           super.s_blocks_per_group,
           block_size);
    printf("First data block: %u\n", super.s_first_data_block);
    printf("Reserved blocks count: %u\n", super.s_r_blocks_count);
    printf("Inodes count: %u\n", super.s_inodes_count);
    printf("Group No: %u\n", super.s_block_group_nr);	
    printf("s_free_inodes_count;%u - --- -- - -" , super.s_free_inodes_count);
    printf("s_inodes_count;%u \n", super.s_inodes_count);
    cout << "s_blocks_per_group: " << super.s_blocks_per_group << endl;
    cout << "s_inodes_per_group;: " << super.s_inodes_per_group << endl;
    // read block bitmap
    bmap *bitmap;
    bitmap = (bmap *)malloc(block_size);
    lseek(fd, BLOCK_OFFSET(group.bg_block_bitmap), SEEK_SET);
    read(fd, bitmap, block_size);
    int fr = 0;
    int nfr = 0;
    printf("Free block bitmap:\n");
    for (int i = 0; i < super.s_blocks_per_group; i++){
        if (BM_ISSET(i,bitmap)){
            printf("+");    // in use
            nfr++;
        }
        else{
            printf("-");    // empty
            fr++;
        }
    }
    printf("\n");
    printf("Free blocks count       : %u\n"
           "Non-Free block count    : %u\n"
           , fr, nfr);
    free(bitmap);
}

int handle_triple(int fd, ui triple_block, vector<ui>& blocks, ui block_size)
{
	int modified = 0;
	int i=0;
    lseek(fd, BLOCK_OFFSET(triple_block), SEEK_SET);
    ui indirect_ptr_block_no;
    vector<ui> indirects;                                            
    while(1)
    {
        read(fd, &indirect_ptr_block_no, sizeof(ui));
        // End of the pointers
        if(indirect_ptr_block_no == 0)
           break;
        blocks.push_back(indirect_ptr_block_no);
        // Data has been modified so cannot recover this file
        if(handle_double(fd, indirect_ptr_block_no, blocks, block_size))
        {
        	modified = 1;
        	break;
        }
    	i++;
    }
    return modified;
}

int handle_double(int fd, ui double_block, vector<ui>& blocks, ui block_size)
{
	int modified = 0;
	int i=0;
    lseek(fd, BLOCK_OFFSET(double_block), SEEK_SET);
    ui indirect_ptr_block_no;
    vector<ui> indirects;                                            
    while(1)
    {
        read(fd, &indirect_ptr_block_no, sizeof(ui));
        // End of the pointers
        if(indirect_ptr_block_no == 0)
           break;
        blocks.push_back(indirect_ptr_block_no);
        // Data has been modified so cannot recover this file
        if(handle_single(fd, indirect_ptr_block_no, blocks, block_size))
        {
        	modified = 1;
        	break;
        }
    	i++;
    }
    return modified;
}
// Finding the block numbers through the single indirect block number
// And putting them into the vector v 
int handle_single(int fd, ui single_block, std::vector<ui>& blocks, ui block_size)
{
	struct ext2_group_desc group;
	lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));

	int flag = 0;
	bmap *bitmap;
    bitmap = (bmap*) new char[block_size];//(bmap *)malloc(block_size);
    read_block(fd, (char*)bitmap, group.bg_block_bitmap, block_size); 
	
	int modified=0;
    lseek(fd, BLOCK_OFFSET(single_block), SEEK_SET);
    ui block_number;
    while(1) // no_of_blocks - 1-> because 1 block is reserved for PointerBlock
    {
        read(fd, &block_number, sizeof(ui));
        if(block_number == 0)
            break;
        if (BM_ISSET(block_number-1,bitmap))
        {
            modified = 1;
            break;
        }
        blocks.push_back(block_number);
    }
    delete[] bitmap;
    return modified;
}

void write_block(int fd, char* block, ui block_no, ui block_size)
{
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    write(fd, block, block_size);	
}
 
void read_block(int fd, char* block, ui block_no, ui block_size)
{
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    read(fd, block, block_size);	
}

// Setting the bits of the blocks of the recovered file
void set_bitmap(int fd, ui inode_no, vector<ui>& blocks, ui block_size)
{     
    struct ext2_super_block super;
    struct ext2_group_desc group;
    struct ext2_inode* inode;
    ui blocks_count;
    inode = new struct ext2_inode();

    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));

    read_inode(fd, group.bg_inode_table, inode, inode_no);
    blocks_count = (inode->i_blocks / (block_size/SECTOR_SIZE));

    lseek(fd, BASE_OFFSET, SEEK_SET);
    read(fd, &super, sizeof(super));

    super.s_free_blocks_count -= blocks_count;
    super.s_free_inodes_count -= 1;
    lseek(fd, BASE_OFFSET, SEEK_SET);
    write(fd, &super, sizeof(ext2_super_block));


    group.bg_free_blocks_count -= blocks_count;
    group.bg_free_inodes_count -= 1;
    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    write(fd, &group, sizeof(ext2_group_desc));

	bmap *bitmap;
    bitmap = (bmap*) new char[block_size];//(bmap *)malloc(block_size);
    read_block(fd, (char*)bitmap, group.bg_block_bitmap, block_size); 

    ui size = blocks.size();
    for (ui i = 0; i < size; i++)
    {
		BM_SET(blocks[i]-1, bitmap);
    }    
    write_block(fd, (char*) bitmap, group.bg_block_bitmap, block_size);
    delete inode;
    delete[] bitmap;
}

// Helper function to read the content of the lost+found's directory entries
void directory_info(char* block)
{
    struct ext2_dir_entry * lost_dir;
    lost_dir = (struct ext2_dir_entry *) ( (char*) block);
    int i=0;
    int pre_rec_len = 12;
    while(lost_dir->rec_len != 0)
    {
        printf("Entry:%d, %u - - " "Rec_len: %u - - " "Name: %s\n",i++, lost_dir->inode, lost_dir->rec_len, lost_dir->name);
        lost_dir = (struct ext2_dir_entry *) ( (char *)block + pre_rec_len);
        pre_rec_len += lost_dir->rec_len; 
    }
}

// Add the inode information to the lost+found directory entry
const std::string add_dir_entry(int fd, char* block, ui inode_no, ui block_size, int f_count)
{
	ui remaining_bytes = 1000;
	struct ext2_dir_entry* lost_dir;

	if(f_count == 0){
    	lost_dir = (struct ext2_dir_entry *) ( (char*) block + 12);
    	lost_dir->rec_len = 12;
	}
	else
	{
		lost_dir = (struct ext2_dir_entry *) ( (char*) block + 24 + 16 * (f_count - 1));
		lost_dir->rec_len = 16;
	}
	lost_dir = (struct ext2_dir_entry *) ( (char*) block + 24 + 16 * (f_count));
	const std::string no = to_string(f_count/10) + to_string(f_count%10);
	const std::string file = "file" + no;
	sprintf(lost_dir->name, "%s", file.c_str()); 
	lost_dir->rec_len = remaining_bytes - (16*f_count);
	lost_dir->inode = inode_no+1;
	lost_dir->name_len = file.size();
	lost_dir->file_type = EXT2_FT_REG_FILE;
    return file;
}

void read_inode(int fd, ui bg_inode_table, struct ext2_inode* inode, int inode_no)
{
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    read(fd, inode, sizeof(struct ext2_inode));
}

void write_inode(int fd, ui bg_inode_table, struct ext2_inode* inode, int inode_no)
{
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    write(fd, inode, sizeof(struct ext2_inode));	
}

// Sets the necessary properties for the file
void configure_file_settings(int fd, ui bg_inode_table, int inode_no)
{
    struct ext2_inode* inode;
    inode = new struct ext2_inode();
    read_inode(fd, bg_inode_table, inode, inode_no);

    inode->i_mode = (EXT2_S_IFREG | EXT2_S_IRUSR);
    inode->i_links_count = 1;
    inode->i_dtime = 0;
    inode->i_flags = 0;

    write_inode(fd, bg_inode_table, inode, inode_no);
    delete inode;
}

// Just prints the recovered files with their delete time and block numbers
void print_filename(struct dell& deleted)
{
	cout << deleted.file_name << " " << deleted.dtime << " " << deleted.blocks_count << "\n";
}

// Clear the dtime of file, clear the flag and set the rights for the file, and also set the bitmap according to the file's blocks
void clean_mess_of_file(int fd, const string file_name, ui bg_inode_table, ui block_size, int inode_no)
{
    // This is our deleted file
    struct ext2_inode* deleted; 
    deleted = new ext2_inode();
    read_inode(fd, bg_inode_table, deleted, inode_no);
    // Change the Removed File's Imode via inode.
    configure_file_settings(fd, bg_inode_table, inode_no);
    delete deleted;
}

// Add the deleted file to the lost+found directory
void add_lost(int fd, int inode_no, vector<ui>& blocks, ui block_size, int f_count)
{
    struct ext2_group_desc group;	
    struct ext2_inode* inode;
    //struct ext2_dir_entry* lost_dir;
    char* block;

    block =  new char[block_size];//(char* )malloc(EXT2_BLOCK_SIZE);
    inode = new struct ext2_inode();

    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));

    //Read the lost+found inode
    read_inode(fd, group.bg_inode_table, inode, 10);
    //Read the lost+found directory entry block
    read_block(fd, block, inode->i_block[0], block_size);
    // Adding the recovered file under lost+found directory
    const std::string file_name = add_dir_entry(fd, block, inode_no, block_size, f_count);
    // Write to make changes permanent
    write_block(fd, block, inode->i_block[0], block_size);

    // Refresh Start For a File
    clean_mess_of_file(fd, file_name, group.bg_inode_table, block_size, inode_no);
    set_bitmap(fd, inode_no, blocks, block_size);

    delete inode;
    delete[] block;
}

// If any block of the file(which will be recovered) is modified, This function will return 1
// -> Means it is modified -> Hence cannot be recovered 
int modified(int fd, ui inode_no, vector<ui>& blocks, ui block_size) // inode_no is the index number of the inode
{    
	struct ext2_group_desc group;
	lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));

	int modified = 0;
	bmap *bitmap;
    bitmap = (bmap*) new char[block_size];//(bmap *)malloc(block_size);
    read_block(fd, (char*)bitmap, group.bg_block_bitmap, block_size); 

    struct ext2_inode* deleted;
    deleted = new struct ext2_inode();
    read_inode(fd, group.bg_inode_table, deleted, inode_no );

    int i=0;
    while(deleted->i_block[i])
    {
        if(BM_ISSET(deleted->i_block[i]-1, bitmap))
        {
            modified = 1;
            break;    
        }
        if(i < 12)
        	blocks.push_back(deleted->i_block[i]);
	    if(i == 12)
	    {
	    	blocks.push_back(deleted->i_block[i]);
	    	modified = handle_single(fd, deleted->i_block[i], blocks, block_size);
	    }
        if(i == 13)
    	{
    		blocks.push_back(deleted->i_block[i]);
    		handle_double(fd, deleted->i_block[i], blocks, block_size);
    	}   
    	if(i == 14)
    	{
    		blocks.push_back(deleted->i_block[i]);
    		handle_triple(fd, deleted->i_block[i], blocks, block_size);
    	}
        i++;
    }
    delete deleted;
    delete[] bitmap;
	return modified;		// TO BE IMPLEMENTED
}

// Finds the not modified files and recover them.
// deleted_files holds the inode numbers
void recover(int fd, vector<dell>& deleted_files, ui block_size)
{
	ui size = deleted_files.size();
	ui file_no = 0;
	for(ui i=0; i < size; i++)
	{
		print_filename(deleted_files[i]);
		std::vector<ui> blocks;
		if(!modified(fd, deleted_files[i].inode_no, blocks, block_size)){
			add_lost(fd, deleted_files[i].inode_no, blocks, block_size, file_no);
			file_no++;
		}
		else
			deleted_files[i].file_name = "x";
	}
	cout << "###" << endl;
	for(ui i=0; i < size; i++)
	{
		if(deleted_files[i].file_name != "x")
			cout << deleted_files[i].file_name << endl;
	}
}
void traverse_inodes(int fd, ui s_inodes_count, ui bg_inode_table)
{
    for(int i = 0; i < s_inodes_count; i++)
    {
        struct ext2_inode inode;
        lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*i, SEEK_SET);
        read(fd, &inode, sizeof(struct ext2_inode));
        if(inode.i_size)
        {
            printf("Reading %d'th inode\n" "Size     : %u bytes\n" "Blocks   : %u\n", i+1, inode.i_size, inode.i_blocks/(block_size/SECTOR_SIZE)); // in number of sectors. A disk sector is 512 bytes.
            printf("Delete Time: %u\n", inode.i_dtime);
            for(int i=0; i < 15; i++)
            {
                if (i < 12)         // direct blocks
                    printf("Block %2u : %u\n", i, inode.i_block[i]);
                else if (i == 12)     // single indirect block
                    printf("Single   : %u\n", inode.i_block[i]);
                else if (i == 13)    // double indirect block
                    printf("Double   : %u\n", inode.i_block[i]);
                else if (i == 14)    // triple indirect block
                    printf("Triple   : %u\n", inode.i_block[i]);
            }
       }
    } 	
}
void find_deleted_files(int fd, ui s_inodes_count, ui bg_inode_table, vector<dell>& deleted_files)
{
    for(int i = 11; i < s_inodes_count; i++)
    {
        struct ext2_inode inode;
        lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*i, SEEK_SET);
        read(fd, &inode, sizeof(struct ext2_inode));
        if(inode.i_size)
        {
            if(inode.i_dtime){
            	struct dell deleted_file;
            	std::string no = to_string((i-11)/10) + to_string((i-11)%10);
    			deleted_file.file_name = "file" + no;
    			deleted_file.dtime = inode.i_dtime;
    			deleted_file.blocks_count = inode.i_blocks / (block_size/SECTOR_SIZE);
    			deleted_file.inode_no = i;
            	deleted_files.push_back(deleted_file);
            }
       }
    } 
}

int main(int argc, char* argv[])
{
    //Counter for indexing the recovered files
    int f_count = 0;
    struct ext2_super_block super;
    struct ext2_group_desc group;
    int fd;
    if ((fd = open(argv[1], O_RDWR)) < 0) {
        perror(IMAGE);
        exit(1);
    }
    // read super-block
    lseek(fd, BASE_OFFSET, SEEK_SET);
    read(fd, &super, sizeof(super));
    if (super.s_magic != EXT2_SUPER_MAGIC) {
        fprintf(stderr, "Not a Ext2 filesystem\n");
        exit(1);
    }
    block_size = 1024 << super.s_log_block_size;
    // read group descriptor
    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));

    vector<dell> deleted_files;   // Holds the inodes of the deleted files
    find_deleted_files(fd, super.s_inodes_count, group.bg_inode_table, deleted_files);
    recover(fd, deleted_files, block_size);
    //traverse_inodes(fd, super.s_inodes_per_group, group.bg_inode_table);
    /*
	print_bitmap(fd, super, group);
    vector<ui> v;
 	int modified;
    modified = handle_double(fd, 295, v, 1024);
    ui s = v.size();
    cout << "modified: " << modified << endl;
    for(ui x=0; x < s; x++)
    {
    	if( !(x % 256))
    		cout << endl;
    	cout << v.front() << " - " ;  
    	v.erase(v.begin());
    }
    cout << endl;
    printf("Inodes count: %u\n", super.s_inodes_count);
    printf("s_free_inodes_count;%u - --- -- - -" , super.s_free_inodes_count);
    printf("s_inodes_count;%u \n", super.s_inodes_count);
    cout << "s_inodes_per_group;: " << super.s_inodes_per_group << endl;
    printf("Group No: %u\n", super.s_blocks_count);	
    cout << "s_blocks_per_group: " << super.s_blocks_per_group << endl;
    */
    close(fd);
    return 0;
}
