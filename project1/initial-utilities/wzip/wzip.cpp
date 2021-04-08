#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace std;
void writeOut(int count, char temp[]);
int main (int argc, char *argv[]){
    // init vars
    int fd = 0;
    char buffer[4096];
    int ret = 0;
    int i = 0;
    char temp[1];
    char rememberWhoYouAre[1];
    int rememberCount = 1;
    int count = 1;
    // char arr[4096];
    // int arrCount = 0;

    if (argc == 1){
        cout << "wzip: file1 [file2 ...]" << endl;
        return 1; // if no file print and return 1 
    }
    for (int j = argc; j > 1; j--){
        fd = open(argv[++i], O_RDONLY);
        if (fd == -1){  /// exit if bad file
            cout <<"wzip: cannot open file"<<  endl;
            return 1;
        }
        while ((ret = read(fd, buffer, 4096)) > 0 ){
            temp[0] = rememberWhoYouAre[0];
            if (temp[0] != buffer[0]){
                count = 1;
            }
            else{
                count = rememberCount;
            }
            for (int j = 0; j <= ret; j++){
                if (temp[0] == buffer[j]){
                    count++;
                    cout<< count << ": is count" <<endl;
                }
                else{
                    if ()
                    temp[0] = buffer[j];
                    count = 1;
                }
            }
           rememberWhoYouAre[0] = temp[0];
           rememberCount = count;
        }
    }

    while (fd >= 3){
        if (close(fd) > 0 ){
            cout<< "wcat: cannot close file" <<endl;
            return 1;
        }
        else fd--;
    }
    return 0;
}

void writeOut(int count, char temp[]){
    write(STDOUT_FILENO, &count, 4);
    write(STDOUT_FILENO, temp, 1);
}