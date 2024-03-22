#ifndef FS_EMULATOR_H
#define FS_EMULATOR_H

#include <stdint.h>

#define MAX_INODES 1024
#define MAX_FILENAME_LENGTH 32

typedef struct {
    uint32_t inode;
    char name[MAX_FILENAME_LENGTH + 1];
    char type; 
} FileEntry;

extern FileEntry inodes[MAX_INODES];
extern uint32_t currentInode;
extern uint32_t inodeCount;

void loadInodeList(const char* path);
void saveInodeList(const char* path);
void changeDirectory(const char* name);
void listContents();
void createDirectory(const char* name);
void createFile(const char* name);

#endif 