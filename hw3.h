#ifndef FILE_HW3
#define FILE_HW3

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ext2.h"
#include <cstring>
#include <vector>
#include <string>
#include <iostream>

#define BASE_OFFSET 1024
#define EXT2_BLOCK_SIZE 1024
#define IMAGE "image.img"
#define SECTOR_SIZE 512

typedef unsigned char bmap;
#define __NBITS (8 * (int) sizeof (bmap))
#define __BMELT(d) ((d) / __NBITS)
#define __BMMASK(d) ((bmap) 1 << ((d) % __NBITS))
#define BM_SET(d, set) ((set[__BMELT (d)] |= __BMMASK (d)))
#define BM_CLR(d, set) ((set[__BMELT (d)] &= ~__BMMASK (d)))
#define BM_ISSET(d, set) ((set[__BMELT (d)] & __BMMASK (d)) != 0)

unsigned int block_size = 0;
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*block_size)

using namespace std;
typedef unsigned int ui;
struct dell{
	string file_name;
	ui dtime;
	ui blocks_count;
	ui inode_no;
};


int handle_triple(int fd, ui triple_block, vector<ui>& blocks, ui block_size);
int handle_double(int fd, ui double_block, vector<ui>& blocks, ui block_size);
int handle_single(int fd, ui single_block, std::vector<ui>& blocks, ui block_size);
void write_block(int fd, char* block, ui block_no, ui block_size);
void read_block(int fd, char* block, ui block_no, ui block_size);
void set_bitmap(int fd, ui inode_no, vector<ui>& blocks, ui block_size);
void directory_info(char* block);
const std::string add_dir_entry(int fd, char* block, ui inode_no, ui block_size, int f_count);
void read_inode(int fd, ui bg_inode_table, struct ext2_inode* inode, int inode_no);
void write_inode(int fd, ui bg_inode_table, struct ext2_inode* inode, int inode_no);
void configure_file_settings(int fd, ui bg_inode_table, int inode_no);
void print_filename(struct dell& deleted);
void clean_mess_of_file(int fd, const string file_name, ui bg_inode_table, ui block_size, int inode_no);
void add_lost(int fd, int inode_no, vector<ui>& blocks, ui block_size, int f_count);
int modified(int fd, ui inode_no, ui block_size); // inode_no is the index number of the inode
void recover(int fd, vector<dell>& deleted_files, ui block_size);
void traverse_inodes(int fd, ui s_inodes_count, ui bg_inode_table);
void find_deleted_files(int fd, ui s_inodes_count, ui bg_inode_table, vector<dell>& deleted_files);
void print_bitmap(int fd, struct ext2_super_block& super, struct ext2_group_desc& group);

#endif
