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
    char tempInt[12];
   char charToRepeat;
    int count = 0;
    int finalCount = 0;

    if (argc == 1){
        cout << "wunzip: file1 [file2 ...]" << endl;
        return 0; // if no file print and return 1 
    }

    for (int j = argc; j > 1; j--){
        fd = open(argv[++i], O_RDONLY);
        if (fd == -1){  /// exit if bad file
            cout <<"wunzip: cannot open file"<<  endl;
            return 1;
        }
        while ((ret = read(fd, buffer, 4)) > 0 ){
            finalCount = (int) buffer;
            read(fd, buffer, 1);
            charToRepeat = buffer[0];
            cout<< finalCount << ": is final Count" <<endl;
            cout << charToRepeat << " is char" <<endl;
        //    if (count == 4){ // five loops
        //       // charToRepeat = buffer[0];
        //     //    std::stringstream ss(tempInt);
        //     //    ss >> finalCount;
        //         int num = (int)tempInt[3];
        //         cout <<num << " is num" <<endl;
        //        cout<< finalCount<<": finalCount" <<endl;
        //        //char result[finalCount];  
        //         count = 0;
        //    }
        //    else{
        //        tempInt[count] = buffer[0];
        //    }
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

