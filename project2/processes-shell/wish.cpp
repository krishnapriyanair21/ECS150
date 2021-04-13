/// if (wish)
/// while (getline != bad)

/// do stuff

#include <cstring>
#include <iostream>
#include <string>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
using namespace std;



/// taken from https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
// std::vector<std::string> parseInput(const std::string& rawinput, char delimiter)
// {
//    std::vector<std::string> parsedInput;
//    std::string rawInput;
//    std::istringstream inputStream(s);
//    while (std::getline(inputStream, rawInput, delimiter))
//    {
//       parsedInput.push_back(token);
//    }
//    return parsedInput;
// }
int main (int argc, char *argv[]){

    cout<<"wish> ";

    vector<std::string> parsedInput;
    std::string rawInput;
    std::string inputForPushBack; 

    while (getline(cin, rawInput)){
        unsigned int startChar = 0; // change startChar to zero every new input
        for (unsigned int i = 0; i < rawInput.length(); i++){
            if (rawInput[i] == ' ' ){
                inputForPushBack = rawInput.substr(startChar, i);
                startChar = (i + 1); 
                cout << startChar << " is start char" <<endl;
                parsedInput.push_back(inputForPushBack);
            }
            else if (i == (rawInput.length() - 1)) { // if nothing else PUSH IT
                cout << startChar << " is start char in else if" <<endl;
                inputForPushBack = rawInput.substr(startChar, i + 1); // startChar == 0????  WHY 
                parsedInput.push_back(inputForPushBack);                
            }
            else{
                continue; /// add case where multiple spaces or tabs or newlines all in first if 
            }
            cout << startChar<<" startChar outside for loop" <<endl;
        }
        cout<<inputForPushBack<< " is input for push back" <<endl; 

        cout << parsedInput.size() << " is size" <<endl;
        // need to track last iteration input point and run from that point to size
            for (unsigned int i = 0; i < parsedInput.size(); i++){
                if (parsedInput[i] == "exit"){  /// iterate through every previous command 
                    return 0;
                }
                else if (parsedInput[i] == "cd"){
                    cout<< "cd";
                }
                else{
                    cout << parsedInput[i] << " is parsedInput[i]" <<endl;
                    
                }
            }
            cout<<"wish> ";
    }
    

    return 0;
}

