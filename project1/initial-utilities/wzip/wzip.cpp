/* worked with Dave Khalasi and Divya Singh*/
#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace std;
//void writeOut(int count, char temp[]);
int main (int argc, char *argv[]){
    // init vars
    int fd = 0;
    char buffer[4096];
    int ret = 0;
    int i = 0;
    char temp[1];
    char rememberWhoYouAre[1];
    int rememberCount = 1;
    int count = 0;
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
            temp[0] = buffer[0];
            /* check previous file char */
            if (temp[0] != rememberWhoYouAre[0]){ //check if previous file had same char
                if (j < argc - 2){
                    // cout << count<<" :is count in first write" <<endl;
                    write(STDOUT_FILENO, &rememberCount, 4);
                    write(STDOUT_FILENO, rememberWhoYouAre, 1);
                    count = 1;
                }
                else{
                    count = 0;
                }
            }
            else{
                count = rememberCount;
            }

           // cout <<ret<< ": is ret"<<endl;
            // cout <<argc<<":is argc" <<endl;
            // cout <<j <<": is j"<<endl;
            /*count iterations of cur char*/
            for (int k = 0; k < ret; k++){
                if (temp[0] == buffer[k]){
                    count++;
                    // cout <<temp[0] << "is temp" <<endl;
                    // cout <<k <<": is k and char is equal"<<endl;
                }else if (temp[0] != buffer[k] && (k < ret)){
                  //  cout <<"else if write" <<endl;
                    write(STDOUT_FILENO, &count, 4);
                    write(STDOUT_FILENO, temp, 1);
                    temp[0] = buffer[k];
                    count = 1;
                }
                else {
                    rememberWhoYouAre[0] = temp[0];
                    rememberCount = count;
                }
               if (j == 2 && k == (ret - 1)){ // last file and last char
                    //cout <<"write from in"<<endl;
                    // cout << count<< ": is count" <<endl;
                    write(STDOUT_FILENO, &count, 4);
                    write(STDOUT_FILENO, temp, 1);
                    temp[0] = buffer[k];
                    count = 1;
                }
                else{
                    rememberWhoYouAre[0] = temp[0];
                    rememberCount = count;
                }
            }
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
