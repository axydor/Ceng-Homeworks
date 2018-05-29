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
    super.s_free_inodes_count -= 1;
    lseek(fd, BASE_OFFSET, SEEK_SET);
    write(fd, &super, sizeof(ext2_super_block));

    lseek(fd, BASE_OFFSET + 1024, SEEK_SET);
    read(fd, &group, sizeof(group));

    group.bg_free_inodes_count -= 1;
    group.bg_free_blocks_count -= inode->i_blocks;
    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    write(fd, &group, sizeof(ext2_group_desc));

	bmap *bitmap;
    bitmap = (bmap *)malloc(block_size);
    lseek(fd, BLOCK_OFFSET(group.bg_block_bitmap), SEEK_SET);
    read(fd, bitmap, block_size);
    std::vector<unsigned int> v;
    set_blocks(inode, v);
    for (int i = 0; i < super.s_blocks_count; i++){
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

void directory_info(void* block)
{
    struct ext2_dir_entry * lost_dir;
    lost_dir = (struct ext2_dir_entry *) ( (char *)block);
    int i=0;
    int pre_rec_len = 12;
    while(lost_dir->rec_len != 0)
    {
        printf("Entry:%d, %u\n" "Rec_len: %u\n" "Name: %s\n",i++, lost_dir->inode, lost_dir->rec_len, lost_dir->name);
        lost_dir = (struct ext2_dir_entry *) ( (char *)block + pre_rec_len);
        pre_rec_len += lost_dir->rec_len; 
    }
}

void add_dir_entry(int fd, void* block, struct ext2_dir_entry* lost_dir, unsigned int inode_no, unsigned int block_size, int f_count)
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
    lost_dir->inode = inode_no;
    lost_dir->name_len = file.size()+1;
    lost_dir->file_type = EXT2_FT_REG_FILE;
}
void add_lost(int fd, unsigned int s_blocks_count, unsigned bg_inode_table, struct ext2_inode * removed, unsigned int inode_no, unsigned int block_size, int f_count)
{
    int pre_rec_len;
    struct ext2_inode* inode;
    inode = (struct ext2_inode *)malloc(sizeof(struct ext2_inode));
    //Read the lost+found inode
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*10, SEEK_SET);
    read(fd, inode, sizeof(struct ext2_inode));
    
    struct ext2_dir_entry* lost_dir;
    struct ext2_dir_entry* removed_entry;
    void* block;
    block = (void* )malloc(EXT2_BLOCK_SIZE);
    lseek(fd, BLOCK_OFFSET(inode->i_block[0]),SEEK_SET);
    read(fd, block, EXT2_BLOCK_SIZE);
    // Read First Directory Entry
 //   directory_info(block);
    add_dir_entry(fd, block, lost_dir, inode_no, block_size, f_count);
    lseek(fd, BLOCK_OFFSET(inode->i_block[0]),SEEK_SET);
    write(fd,block,EXT2_BLOCK_SIZE);
    
    // Change the Removed File's Imode via inode.
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    read(fd, inode, sizeof(struct ext2_inode));
    inode->i_mode = (EXT2_S_IFREG | EXT2_S_IRUSR);
    inode->i_links_count = 1;
    inode->i_dtime = 0;
    inode->i_flags = 0;
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    write(fd, inode, sizeof(struct ext2_inode));
    set_bitmap(fd, removed, block_size);
/*
*/
    free(inode);
    free(block);
}

void traverse_inodes(int fd, unsigned int s_blocks_count, unsigned int bg_inode_table, int f_count)
{
    for(int i = 0; i < 32; i++)
    {
        struct ext2_inode inode;
        lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*i, SEEK_SET);
        read(fd, &inode, sizeof(struct ext2_inode));
        if(inode.i_size)
        {
        	//rintf("Our Value Imode : %d", inode.i_mode);
        	//printf("Imode should be: %d", ((EXT2_S_IFREG | EXT2_S_IRUSR) ) );
            printf("Reading %d'th inode\n"
                   "Size     : %u bytes\n"
                   "Blocks   : %u\n",
                    i+1,
                   inode.i_size,
                   inode.i_blocks/2); // in number of sectors. A disk sector is 512 bytes.
            printf("Delete time: %u\n", inode.i_dtime);
            if(inode.i_dtime){
                printf("Deleted inode index: %d\n", i);
                add_lost(fd, s_blocks_count, bg_inode_table, &inode, i, block_size, f_count);
                f_count++;
            }
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
    traverse_inodes(fd, super.s_blocks_count, group.bg_inode_table, f_count);
    close(fd);
    return 0;
}
