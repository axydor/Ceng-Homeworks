#include "hw3.h"
using namespace std;


void add_lost(int fd, unsigned bg_inode_table, struct ext2_inode * removed, unsigned int inode_no )
{
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
    lost_dir = (struct ext2_dir_entry *) block;
    lost_dir = (struct ext2_dir_entry *) ( (void*)lost_dir + lost_dir->rec_len);
    //lost_dir->rec_len = 12;

    removed_entry = (struct ext2_dir_entry *) ( (void*)block + 24);
    removed_entry->inode = inode_no+1;
    removed_entry->rec_len = 1000;
    removed_entry->name_len = 2;
    removed_entry->file_type = EXT2_FT_REG_FILE;
    strcpy(removed_entry->name, "f"); 

    lseek(fd, BLOCK_OFFSET(inode->i_block[0]),SEEK_SET);
    write(fd,block,EXT2_BLOCK_SIZE);
    
    lost_dir = (struct ext2_dir_entry *) ( (void*)lost_dir + lost_dir->rec_len);
    printf("LOST_DIR : Inode: %u\n" "Rec_len: %u\n" "Name: %s\n", lost_dir->inode, lost_dir->rec_len, lost_dir->name);
    printf("Inode: %u\n" "Rec_len: %u\n" "Name: %s\n", removed_entry->inode, removed_entry->rec_len, removed_entry->name);
    // Change the Removed File's Imode via inode.
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    read(fd, inode, sizeof(struct ext2_inode));
    inode->i_mode = (EXT2_S_IFREG | EXT2_S_IRUSR);
    inode->i_links_count = 1;
    lseek(fd, BLOCK_OFFSET(bg_inode_table)+sizeof(struct ext2_inode)*inode_no, SEEK_SET);
    write(fd, inode, sizeof(struct ext2_inode));

    free(inode);
    free(block);
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
        	//rintf("Our Value Imode : %d", inode.i_mode);
        	//printf("Imode should be: %d", ((EXT2_S_IFREG | EXT2_S_IRUSR) ) );
            printf("Reading %d'th inode\n"
                   "Size     : %u bytes\n"
                   "Blocks   : %u\n",
                    i+1,
                   inode.i_size,
                   inode.i_blocks/2); // in number of sectors. A disk sector is 512 bytes.
            printf("Delete time: %u\n", inode.i_dtime);
            if(i == 11)
            	inode.i_dtime = 12;
            if(inode.i_dtime)
                add_lost(fd, bg_inode_table, &inode, i);
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
    traverse_inodes(fd, group.bg_inode_table);
    close(fd);
    return 0;
}
