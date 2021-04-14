#include <cstring>
#include <iostream>
#include <string>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

vector<string> parse(string rawInput){ // from stack overflow
    istringstream s(rawInput);
    vector<string> parsedInput;
    string cmdx; // command at x 
    while(s >> cmdx){
        parsedInput.push_back(cmdx);
    }
    return parsedInput;
}

int main (int argc, char *argv[]){

    cout<<"wish> ";

    vector<std::string> parsedInput;
    string rawInput;

    if (argc == 1){ // interactive
        while (getline(cin, rawInput)){ /// loop until exit
            cout <<"rawInput: " << rawInput<<endl;
            parsedInput = parse(rawInput);
            for (unsigned int i = 0; i < parsedInput.size(); i++){ // iterate through every previous command 
                cout << "testing! " << parsedInput[i] <<" at " << i <<endl;
                if (parsedInput[i] == "exit"){  
                    exit(0);
                }
                else if (parsedInput[i] == "cd"){
                    // cout<< "cd";
                    continue;
                }
            }
            cout<<"wish> ";
        }
    }else{ // batch
        for (int i = 1; i < argc; i++){
            ifstream inputFile;
            inputFile.open(argv[i]);
            while(getline(inputFile, rawInput)){
                parsedInput = parse(rawInput);
                for (unsigned int i = 0; i < parsedInput.size(); i++){ // iterate through every previous command 
                if (parsedInput[i] == "exit"){  
                    exit(0);
                }
                else if (parsedInput[i] == "cd"){
                    // cout<< "cd";
                    continue;
                }
            }
            cout<<"wish> ";
            }
            inputFile.close();
        }
    }
    

    return 0;
}

