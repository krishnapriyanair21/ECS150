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

vector<string> parse(string rawInput);
void printError0();
void printError1();
void modes(vector<string> parsedInput);

int main (int argc, char *argv[]){

    vector<string> parsedInput;
    string rawInput;

    if (argc == 1){ // interactive
        cout << "wish> ";
        while (getline(cin, rawInput)){ 
            parsedInput = parse(rawInput);
            modes(parsedInput);
            cout<<"wish> ";
        }
    }else if (argc > 1){ // batch
        for (int i = 1; i < argc; i++){
            ifstream inputFile;
            inputFile.open(argv[i]);
                        
            while (getline(inputFile, rawInput)){
                parsedInput = parse(rawInput);
                modes(parsedInput);
                inputFile.close();
            }
        }
    }
    

    return 0;
}

void modes(vector<string> parsedInput){
    for (unsigned int i = 0; i < parsedInput.size(); i++){ 
        if (parsedInput[i] == "exit"){  
            exit(0);
        } else if (parsedInput[i] == "cd"){
            if (parsedInput.size() <= (i + 1)){
                printError0();
                break;
            }else{
                string convertString = parsedInput[i + 1];
                int destSize = convertString.size();
                char dirToChange[destSize + 1];
                strcpy(dirToChange, convertString.c_str()); // need char array for chdir()
                if (chdir(dirToChange) == 0){
                    continue;
                } else{
                    printError0();
                }     
            }
            break;
        } else if (parsedInput[i] == "ls"){
            // check if next input starts with -
                char *args[3];
                args[0] = strdup("ls");
                args[1] = strdup("-l");
                args[2]= NULL;
                execv(args[0], args); /// does not work 
        }
    }
    return;
}
vector<string> parse(string rawInput){  // stack overflow
    istringstream s(rawInput);
    vector<string> parsedInput;
    string cmdx; // command at x 
    while(s >> cmdx){
        parsedInput.push_back(cmdx);
    }
    return parsedInput;
}
void printError0(){
    cerr<<"An error has occurred"<<endl;
    exit(0);
}
void printError1(){
    cerr<<"An error has occurred"<<endl;
    exit(1);
}