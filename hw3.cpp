#include "hw3.h"
using namespace std;

void set_blocks(struct ext2_inode* inode, std::vector<unsigned int>& v)
{
	int i=0;
	while(inode->i_block[i])
	{
	    //printf("%d-",inode->i_block[i]);
		v.push_back(inode->i_block[i]);
        i++;
    }
} 

void set_bitmap(int fd, struct ext2_inode* inode, unsigned int block_size)
{     
    struct ext2_super_block super;
    struct ext2_group_desc group;
    lseek(fd, BASE_OFFSET, SEEK_SET);
    read(fd, &super, sizeof(super));

    super.s_free_blocks_count -= inode->i_blocks;
    lseek(fd, BASE_OFFSET, SEEK_SET);
    write(fd, &super, sizeof(ext2_super_block));

    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));

    group.bg_free_blocks_count -= inode->i_blocks;
    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    write(fd, &group, sizeof(ext2_group_desc));

	bmap *bitmap;
    bitmap = (bmap *)malloc(block_size);
    lseek(fd, BLOCK_OFFSET(group.bg_block_bitmap), SEEK_SET);
    read(fd, bitmap, block_size);
    std::vector<unsigned int> v;
    set_blocks(inode, v);
    for (unsigned int i = 0; i < super.s_blocks_count; i++){
        if (BM_ISSET(i,bitmap)){
            ;
        }
        else{
        	if( (i+1) == v.front() ){
                std::cout << *v.begin() << "-";
        		v.erase(v.begin());
        		BM_SET(i, bitmap);
        		if(v.empty())
        			break;
        	}
        }
    }    
    lseek(fd, BLOCK_OFFSET(group.bg_block_bitmap), SEEK_SET);
    write(fd, bitmap, block_size);
    free(bitmap);
}

void directory_info(char* block)
{
    struct ext2_dir_entry * lost_dir;
    lost_dir = (struct ext2_dir_entry *) ( (char *)block);
    int i=0;
    int pre_rec_len = 12;
    while(lost_dir->rec_len != 0)
    {
        printf("Entry:%d, %u - - " "Rec_len: %u - - " "Name: %s\n",i++, lost_dir->inode, lost_dir->rec_len, lost_dir->name);
        lost_dir = (struct ext2_dir_entry *) ( (char *)block + pre_rec_len);
        pre_rec_len += lost_dir->rec_len; 
    }
}

void add_dir_entry(int fd, char* block, struct ext2_dir_entry* lost_dir, unsigned int inode_no, unsigned int block_size, int f_count)
{
    lost_dir = (struct ext2_dir_entry *) ( (char *)block + 12);
    lost_dir->rec_len = 12;
    int pre_rec_len = 24;
    lost_dir = (struct ext2_dir_entry *) ( (char *)block + 24);
    while(lost_dir->name_len != 0)
    {
        pre_rec_len += lost_dir->rec_len;
        if(pre_rec_len == block_size){
            pre_rec_len -= lost_dir->rec_len;
            pre_rec_len  += 16;  
            lost_dir->rec_len = 16;
        }
        lost_dir = (struct ext2_dir_entry *) ( (char *)block + pre_rec_len);
    }
    std::string no = to_string(f_count/10) + to_string(f_count%10);
    std::string file = "file" + no;
    sprintf(lost_dir->name, "%s", file.c_str()); 
    
    lost_dir->rec_len = block_size - pre_rec_len;
    lost_dir->inode = inode_no+1;
    lost_dir->name_len = file.size();
    lost_dir->file_type = EXT2_FT_REG_FILE;
}

void write_block(int fd, char* block, unsigned int block_no)
{
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    write(fd, block, EXT2_BLOCK_SIZE);	
}


void read_block(int fd, char* block, unsigned int block_no)
{
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    read(fd, block, EXT2_BLOCK_SIZE);	
}

void read_inode(int fd, unsigned int bg_inode_table, struct ext2_inode* inode, int inode_no)
{
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    read(fd, inode, sizeof(struct ext2_inode));
}

void write_inode(int fd, unsigned int bg_inode_table, struct ext2_inode* inode, int inode_no)
{
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    write(fd, inode, sizeof(struct ext2_inode));	
}


void configure_file_settings(int fd, unsigned int bg_inode_table, int inode_no)
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

void add_lost(int fd, int inode_no, unsigned int block_size, int f_count)
{
    struct ext2_group_desc group;	
    struct ext2_inode* inode;
    struct ext2_dir_entry* lost_dir;
    char* block;

    block = new char[EXT2_BLOCK_SIZE];//(char* )malloc(EXT2_BLOCK_SIZE);
    inode = new struct ext2_inode();

    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));
    //Read the lost+found inode
    read_inode(fd, group.bg_inode_table, inode, 10);
    //Read the lost+found directory entry block
    read_block(fd, block, inode->i_block[0]);
    //directory_info(block);
    // Read First Directory Entry
    add_dir_entry(fd, block, lost_dir, inode_no, block_size, f_count);
    write_block(fd, block, inode->i_block[0]);

    // Change the Removed File's Imode via inode.
    configure_file_settings(fd, group.bg_inode_table, inode_no);

    struct ext2_inode* removed;
    removed = new ext2_inode();
    read_inode(fd, group.bg_inode_table, removed, inode_no);
    set_bitmap(fd, removed, block_size);

    delete removed;
    delete inode;
    delete[] block;
    //free(block);
}
void check_modified(vector<int>& deleted_files)
{
	;		// TO BE IMPLEMENTED
}
void recover(int fd, vector<int>& deleted_files)
{
	check_modified(deleted_files);
	for(unsigned int i=0; i < deleted_files.size(); i++)
	{
		printf("Deleted File %d: %d\n", i , deleted_files[i]);
		add_lost(fd, deleted_files[i], block_size, i);
	}
}
void traverse_inodes(int fd, unsigned int bg_inode_table)
{
    for(int i = 0; i < 32; i++)
    {
        struct ext2_inode inode;
        lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*i, SEEK_SET);
        read(fd, &inode, sizeof(struct ext2_inode));
        if(inode.i_size)
        {
            printf("Reading %d'th inode\n" "Size     : %u bytes\n" "Blocks   : %u\n", i+1, inode.i_size, inode.i_blocks/2); // in number of sectors. A disk sector is 512 bytes.
            printf("Delete Time: %u\n", inode.i_dtime);
            for(int i=0; i < 15; i++){
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
void find_deleted_files(int fd, unsigned int bg_inode_table, vector<int>& deleted_files)
{
    for(int i = 11; i < 32; i++)
    {
        struct ext2_inode inode;
        lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*i, SEEK_SET);
        read(fd, &inode, sizeof(struct ext2_inode));
        if(inode.i_size)
        {
            if(inode.i_dtime){
            	deleted_files.push_back(i);
            }
       }
    } 
}

int main(void)
{
    //Counter for indexing the recovered files
    int f_count = 0;
    struct ext2_super_block super;
    struct ext2_group_desc group;
    int fd;

    if ((fd = open(IMAGE, O_RDWR)) < 0) {
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

    // read group descriptor
    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));
    
    // read block bitmap
    bmap *bitmap;
    bitmap = (bmap *)malloc(block_size);
    lseek(fd, BLOCK_OFFSET(group.bg_block_bitmap), SEEK_SET);
    read(fd, bitmap, block_size);
    int fr = 0;
    int nfr = 0;
    printf("Free block bitmap:\n");
    for (int i = 0; i < super.s_blocks_count; i++){
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

    vector<int> deleted_files;   // Holds the inodes of the deleted files
    find_deleted_files(fd, group.bg_inode_table, deleted_files);
    recover(fd, deleted_files);
    traverse_inodes(fd, group.bg_inode_table);

    close(fd);
    return 0;
}
