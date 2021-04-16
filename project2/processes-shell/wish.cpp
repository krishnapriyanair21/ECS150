/* This project was done with Dave Khalasi and Divya Singh */

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
void executeCmd(vector<string> parsedInput, vector<string> &path);
void pathFunction(vector<string> parsedInput, vector<string> &path);
int main (int argc, char *argv[]){

    vector<string> parsedInput;
    string rawInput;
    vector<string> curPath;
    curPath.push_back("");
    curPath.push_back("/bin/");

    if (argc == 1){ // interactive
        cout << "wish> ";
        while (getline(cin, rawInput)){ 
            parsedInput = parse(rawInput);
            executeCmd(parsedInput, curPath);
            cout<<"wish> ";
        }
    }else if (argc > 1){ // batch
        for (int i = 1; i < argc; i++){
            ifstream inputFile;
            inputFile.open(argv[i]);
                        
            while (getline(inputFile, rawInput)){
                parsedInput = parse(rawInput);
                executeCmd(parsedInput,curPath);
                inputFile.close();
            }
        }
    } 
    

    return 0;
}

void executeCmd(vector<string> parsedInput, vector<string> &path){
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
                    vector<string> newDir;
                    newDir.push_back(convertString);
                    pathFunction(newDir,path);
                } else{
                    printError0();
                }     
            }
            break;
        } else if (parsedInput[i] == "path") {
           pathFunction(parsedInput, path);
        }else{
            
            printError0();
        }
    }
    return;
}

void pathFunction(vector<string> parsedInput, vector<string> &path){
    path.clear();
    path.push_back("");
    for (unsigned int i = 1; i < parsedInput.size(); i++){
        if (parsedInput[i][0] != '/') 
            parsedInput[i] = "/" + parsedInput [i];
        if (parsedInput[i][parsedInput.size() - 1] != '/')
            parsedInput[i] += "/";
        char curDir[4096];
        getcwd(curDir, 4096);
        parsedInput[i] = curDir + parsedInput[i];
        path.push_back(parsedInput[i]);
    }
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