/* worked with Dave Khalasi and Divya Singh*/
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
    int ret = 0;
    char tempBuf[1];
    string tempReadIn = "";

    const char* searchString = argv[1];

    while ((ret = read(fd, tempBuf, 1)) > 0){
        tempReadIn = tempReadIn + tempBuf[0]; // read in one char at a time
        if (tempBuf[0] == '\n'){
           int lengthOfRead = tempReadIn.length() + 1; // find length of line
           char TBDPrintLine[lengthOfRead]; // allocate space
           strcpy(TBDPrintLine, tempReadIn.c_str());
            if(strstr(TBDPrintLine, searchString)){  // if match print
                write(STDOUT_FILENO, TBDPrintLine, lengthOfRead - 1); 
            }
            tempReadIn = {"\0"} ; // reset
        }
        tempBuf[0] = '\0';
    }
};


