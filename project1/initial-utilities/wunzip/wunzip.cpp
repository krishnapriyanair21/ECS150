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
    int buffer[4];
    int ret = 0;
    int i = 0;
    char tempChar[4];
    char charToRepeat;
    int finalCount = 0;

    if (argc == 1){
        cout<< "wunzip: file1 [file2 ...]"<<endl;
        return 1; // if no file print and return 1 
    }

    for (int j = argc; j > 1; j--){
        fd = open(argv[++i], O_RDONLY);
        if (fd == -1){  /// exit if bad file
            cout<<"wunzip: cannot open file"<<endl;
            return 1;
        }
        while ((ret = read(fd, &buffer, 4)) > 0 ){
            finalCount = (int) buffer[0]; //pull int
            read(fd, &tempChar, 1);
            charToRepeat = tempChar[0]; // pull char
            for (int i = 0; i < finalCount; i++){
                write(STDOUT_FILENO, &charToRepeat, 1);
            }
        }
    }

    while (fd >= 3){
        if (close(fd) > 0 ){
            cout<<"wunzip: cannot close file"<<endl;
            return 1;
        }
        else fd--;
    }
    return 0;
}

