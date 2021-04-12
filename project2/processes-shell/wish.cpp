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

    // std::vector<std::string> parsedInput = parseInput(); 
    cout<<"wish> ";
    vector<std::string> parsedInput{};
    std::string rawInput;
    while (getline(cin, rawInput))
    {
        cout <<"check"<<endl;
        parsedInput.push_back(rawInput);
        /// track which element we're on 
        while (!parsedInput.empty()){
            if (parsedInput[0] == "exit"){ // curr element in parsedInput == "exit" IS INCORRECT
                return 0;
            }
            else{
                cout<<"wish> ";
                break;
            }
        }
    }
    

    return 0;
}

