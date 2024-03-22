#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_INODES 1024
#define MAX_FILENAME_LENGTH 32

typedef struct {
    uint32_t inode;
    char name[MAX_FILENAME_LENGTH + 1];
    char type; 
} FileEntry;

FileEntry inodes[MAX_INODES];uint32_t currentInode = 0;
uint32_t inodeCount = 0;

void loadInodeList(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("Error opening file: %s\n", path);
        return;
    }

    inodeCount = 0;
    FileEntry entry;
    size_t bytesRead;

    while ((bytesRead = fread(&entry, sizeof(FileEntry), 1, fp)) == 1) {
        if (entry.inode >= MAX_INODES || (entry.type != 'd' && entry.type != 'f')) {
           printf("Invalid inode\n");
        } else {
            inodes[inodeCount++] = entry;
        }
    }

    if (bytesRead != 0 && !feof(fp)) {
        printf("Error reading file: %s\n", path);
    }

    fclose(fp);
}

void saveInodeList(const char* path) {
    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        printf("Error opening file: %s\n", path);
        return;
    }

    fwrite(inodes, sizeof(FileEntry), inodeCount, fp);
    fclose(fp);
}

void changeDirectory(const char* name) {
    if (strcmp(name, ".") == 0) {
        return;
    }

    if (strcmp(name, "..") == 0) {
        for (uint32_t i = 0; i < inodeCount; i++) {
            if (inodes[i].inode == currentInode) {
                currentInode = inodes[i].inode;
                return;
            }
        }
        printf("Cannot go to parent directory\n");
        return;
    }

    for (uint32_t i = 0; i < inodeCount; i++) {
        if (strcmp(inodes[i].name, name) == 0 && inodes[i].type == 'd') {
            currentInode = inodes[i].inode;
            return;
        }
    }

    printf("Directory not found: %s\n", name);
}

void listContents() {
    printf("Contents of directory %u:\n", currentInode);
    for (uint32_t i = 0; i < inodeCount; i++) {
        if (inodes[i].inode == currentInode) {
            continue;
        }
        for (uint32_t j = 0; j < inodeCount; j++) {
            if (inodes[j].inode == inodes[i].inode) {
                printf("%s (%c)\n", inodes[j].name, inodes[j].type);
                break;
            }
        }
    }
}

void createDirectory(const char* name) {
    if (inodeCount >= MAX_INODES) {
        printf("Maximum number of inodes reached\n");
        return;
    }

    for (uint32_t i = 0; i < inodeCount; i++) {
        if (inodes[i].inode == currentInode && strcmp(inodes[i].name, name) == 0) {
            printf("Directory already exists: %s\n", name);
            return;
        }
    }

    FileEntry newDir = {inodeCount, {0}, 'd'};
    strcpy(newDir.name, name);
    inodes[inodeCount++] = newDir;
    printf("Directory created: %s\n", name);
}

void createFile(const char* name) {
    if (inodeCount >= MAX_INODES) {
        printf("Maximum number of inodes has gotten reached\n");
        return;
    }

    for (uint32_t i = 0; i < inodeCount; i++) {
        if (inodes[i].inode == currentInode && strcmp(inodes[i].name, name) == 0) {
            printf("File already exists: %s\n", name);
            return;
        }
    }

    FileEntry newFile = {inodeCount, {0}, 'f'};
    strcpy(newFile.name, name);
    inodes[inodeCount++] = newFile;
    printf("File created: %s\n", name);
}

int main(int argc, char* argv[]) {
    printf("start\n");
    if (argc < 2) {
        printf("Usage: %s <file_system_directory>\n", argv[0]);
        return 1;
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/inodes_list", argv[1]);

    printf("Opened file system %s\n", argv[1]);

    loadInodeList(path);

    char command[32];
    char name[MAX_FILENAME_LENGTH + 1];

    printf("Enter a command ('cd', 'ls', 'mkdir', 'touch', 'exit'):\n");

    while (1) {
        printf("> ");
        if (scanf("%31s%*[^\n]", command) != 1) {
            break;
        }

        switch (command[0]) {
            case 'c':
                if (strcmp(command, "cd") == 0) {
                    if (scanf("%32s%*[^\n]", name) != 1) {
                        printf("Missing directory name\n");
                        continue;
                    }
                    changeDirectory(name);
                }
                break;
            case 'l':
                if (strcmp(command, "ls") == 0) {
                    listContents();
                }
                break;
            case 'm':
                if (strcmp(command, "mkdir") == 0) {
                    if (scanf("%32s%*[^\n]", name) != 1) {
                        printf("Missing directory name\n");
                        continue;
                    }
                    createDirectory(name);
                }
                break;
            case 't':
                if (strcmp(command, "touch") == 0) {
                    if (scanf("%32s%*[^\n]", name) != 1) {
                        printf("Missing file name\n");
                        continue;
                    }
                    createFile(name);
                }
                break;
            case 'e':
                if (strcmp(command, "exit") == 0) {
                    goto exit_loop;
                }
                break;
            default:
                printf("Unknown command: %s\n", command);
                break;
        }
    }

exit_loop:
    snprintf(path, sizeof(path), "%s/inodes_list", argv[1]);
    saveInodeList(path);

    return 0;
}