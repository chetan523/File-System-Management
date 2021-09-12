#include <iostream>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;

class Disk{
  private:
  // data structure to store Inode information
    struct Inode{
        char file_name[8];
        int size;
        int blockPointers[8];
        int used;
    };

  // data structure to store SuperBlock information
    struct SuperBlock{
        bool usedBlocks[128];
        struct Inode inodes[16];
    }superBlock;

    char dataBlocks[127][1024]; // 127 data blocks of size 1kB each

    // function to check whether a file is present is or not
    bool exists(char newFile[]){
        for(auto inode : superBlock.inodes)
            if(inode.used && strcmp(inode.file_name, newFile) == 0) return true;
        return false;
    }

    // function to check whether the specified number of data blocks are availble
    bool available(int size){
        int freeBlockCount = 0;
        for(auto block : superBlock.usedBlocks)
            if(!block) freeBlockCount++;
        return freeBlockCount >= size;
    }

    // function to find unused Inode
    int getInode(){
        for(int i=0; i<16; ++i)
            if(!superBlock.inodes[i].used) return i;
        return -1;
    }

  public:
    Disk(){
        superBlock.usedBlocks[0] = true;
        memset(dataBlocks, '\0', sizeof(dataBlocks));
    }

    // function to create new file
    void create(char file_name[8], int size){
        // refuse to create the file if size of greater than 8kB
        if(size > 8){
            cout << "File too large!" << endl;
            return;
        }

        // check if a file with same name already exists
        if(exists(file_name)){
            cout << "File already exists!" << endl;
            return;
        }

        // check if specified number of data blocks are available
        if(!available(size)){
            cout << "Could not create file. Specified number of data blocks are not available!" << endl;
            return;
        }

        // get a free Inode if available
        int curINode = getInode();
        if(curINode == -1){
            cout << "Maximum number of supported files already exist!" << endl;
            return;
        }

        // setting Inode properties
        strcpy(superBlock.inodes[curINode].file_name, file_name);
        superBlock.inodes[curINode].used = 1;
        superBlock.inodes[curINode].size = size;

        // allocate data blocks to the file
        for(int i=1,j=0; i<128 && j<size; ++i){
            if(!superBlock.usedBlocks[i]){
                superBlock.inodes[curINode].blockPointers[j] = i;
                superBlock.usedBlocks[i] = true;
                ++j;
            }
        }

        cout << "Created new file: " << file_name << endl;
    }

    void Delete(char file_name[]){
        // check if the file exists
        if(!exists(file_name)){
            cout << "File does not exist!" << endl;
            return;
        }

        // free the data blocks used by the file
        for(int i=0; i<16; ++i){
            // get the Inode for the file and free corresponding data blocks
            if(strcmp(file_name,superBlock.inodes[i].file_name) == 0){
                superBlock.inodes[i].used = 0;
                for(int dataBlock : superBlock.inodes[i].blockPointers){
                    memset(dataBlocks[dataBlock], '\0', 1024);  // clearing the data block
                    superBlock.usedBlocks[dataBlock] = false;  // adding to the free block lists
                }
                break;
            }
        }

        cout << "Deleted file " << file_name << endl;
    }

    void ls(){
        cout << "---------------------------------" << endl;
        cout << "|  File Name\t|\tSize\t|" << endl;
        cout << "---------------------------------" << endl;

        // check for every Inode which is in use
        for(auto inode : superBlock.inodes)
            if(inode.used) cout << "|    " << inode.file_name << "\t|\t" << inode.size << " kB\t|" <<  endl;
        cout << "---------------------------------" << endl;
    }

    void read(char file_name[8], int blockNum, char buf[1024]){
        // check if the file exists or not
        if(!exists(file_name)){
            cout << "File does not exist!" << endl;
            return;
        }

        // finding the Inode for the file
        for(auto inode : superBlock.inodes){
            if(strcmp(file_name, inode.file_name) == 0){
                if(blockNum > inode.size){
                    cout << "Invalid block number!" << endl;
                    return;
                }

                // copy data in the specified block to the buffer
                strcpy(buf, dataBlocks[inode.blockPointers[blockNum]]);
                break;
            }
        }

    }

    void write(char file_name[8], int blockNum, char buf[1024]){
        // check if the file exists or not
        if(!exists(file_name)){
            cout << "File does not exist!" << endl;
            return;
        }

        // finding the inode for the file
        for(auto inode : superBlock.inodes){
            if(strcmp(file_name, inode.file_name) == 0){
                if(blockNum > inode.size){
                    cout << "Invalid block number!" << endl;
                    return;
                }

                // copying the data in the buffer to the specified data block
                strcpy(dataBlocks[inode.blockPointers[blockNum]], buf);
                cout << "Data written succesfully." << endl;
                break;
            }
        }
    }

};

int main(){
    Disk disk;
    
    int choice = 0;
    while(choice != 9){
        cout << "Enter the operation that you want to perform." << endl;
        cout << " 1. Create a new file" << endl;
        cout << " 2. Delete an existing file" << endl;
        cout << " 3. Read data from a file" << endl;
        cout << " 4. Write data to a file" << endl;
        cout << " 5. View all files" << endl;
        cout << " 9. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cout << endl;

        char file_name[8];  // name of the file
        memset(file_name, '\0', 8);
        int blockNum;  // bolck number on which the read/write operation is to be performed
        char buffer[1024];  // buffer array to read or write the data
        memset(buffer, '\0', 1024);
        
        switch(choice){
            case 1:
                int size;
                cout << "Enter the file name: ";
                cin >> file_name;
                cout << "Enter file size(in blocks): ";
                cin >> size;
                disk.create(file_name, size);
                break;

            case 2:
                cout << "Enter the file name: ";
                cin >> file_name;
                disk.Delete(file_name);
                break;
            
            case 3:
                cout << "Enter the file name: ";
                cin >> file_name;
                cout << "Enter block number to read: ";
                cin >> blockNum;
                disk.read(file_name, blockNum, buffer);
                cout << "Contents of the file are: " << buffer << endl;
                break;

            case 4:
                cout << "Enter the file name: ";
                cin >> file_name;
                cout << "Enter block number to write: ";
                cin >> blockNum;
                cout << "Enter the data to be written: ";
                cin.ignore(256, '\n');
                cin.getline(buffer, 1024);
                disk.write(file_name, blockNum, buffer);
                break;

            case 5:
                disk.ls();
                break;
            
            case 9: break;

            default:
                cout << "Invalid choice!" << endl;
                break;
        }

        cout << endl;
    }

    return 0;
}