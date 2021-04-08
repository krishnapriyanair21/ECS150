#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <string>

using namespace std;

void findString(int fd, char* argv[]);
int main (int argc, char *argv[]){
    // init vars
    int fd = 0;
    int i = 1; // first term is search term

    if (argc == 1){
        cout << "wgrep: searchterm [file ...]" <<endl;
        return 1; // if no file error and cout 
    }
    if (argc < 3) {
        findString(STDIN_FILENO, argv);     
    } else{
        /// how to check for no files?
        for (int j = argc; j > 2; j--){ // search term is first arg. open files after search term
            fd = open(argv[++i], O_RDONLY);
            if (fd == -1){  /// exit if bad file
                cout <<"wgrep: cannot open file"<<  endl;
                return 1;
            }
            findString(fd, argv);           
        }
    } 

    while (fd > 3){  /// CHECK
        if (close(fd) > 0 ){
            cout<< "wgrep: cannot close file" <<endl;
            return 1;
        }
        else fd--;
    }
    return 0;
}

void findString(int fd, char* argv[]){
    char buffer[4096];
    int ret = 0;
    char tempBuf[4096];
    int index = 0;
    char emptyLine[4096];

    while ((ret = read(fd, tempBuf, 1)) > 0){
        emptyLine.append(tempBuf[0]);
        if (tempBuf[0] != '\n'){
            if(emptyLine.find(argv[1]) < emptyLine){
                emptyLine.c_str(); // not sure what this does 
            
                //     ret = write(STDOUT_FILENO, buffer, ret);
            }
            else {cout <<"not in if statement" <<endl ;}
        }
        index++; 
    }
};


