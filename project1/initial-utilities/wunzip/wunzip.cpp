/* worked with Dave Khalasi and Divya Singh*/
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace std;

int main (int argc, char *argv[]){
    // init vars
    int fd = 0;
    char buffer[4096];
    int ret = 0;
    int i = 0;
   // char tempInt[12];
   char charToRepeat;
   // int count = 0;
    int finalCount = 0;

    if (argc == 1){
        write(STDOUT_FILENO, "wunzip: file1 [file2 ...]\n", 27);
        exit(1); // if no file print and return 1 
    }

    for (int j = argc; j > 1; j--){
        fd = open(argv[++i], O_RDONLY);
        if (fd == -1){  /// exit if bad file
            write(STDOUT_FILENO, "wunzip: cannot open file\n", 26);
            return 1;
        }
        while ((ret = read(fd, &buffer, 4)) > 0 ){
            finalCount = (int) buffer[0];
            read(fd, &buffer, 1);
            charToRepeat = buffer[0];
            // cout<< finalCount << ": is final Count" <<endl;
            // cout << charToRepeat << " is char" <<endl;
            for (int i = 0; i < finalCount; i++){
                write(STDOUT_FILENO, &charToRepeat, 1);
            }
        }
    }

    while (fd >= 3){
        if (close(fd) > 0 ){
            write(STDOUT_FILENO,"wunzip: cannot close file\n",27);
            return 1;
        }
        else fd--;
    }
    return 0;
}

