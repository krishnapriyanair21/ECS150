#define RAPIDJSON_HAS_STDSTRING 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "WwwFormEncodedDict.h"
#include "HttpClient.h"

#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;

int API_SERVER_PORT = 8080;
string API_SERVER_HOST = "localhost";
string PUBLISHABLE_KEY = "";

string auth_token;
string user_id;

void errorMessage();
vector<string> parse(string rawInput);
void executeCmd(vector<string> parsedInput);
bool serverError(HTTPClientResponse *response);
void auth(vector<string> parsedInput);
void balance(vector<string> parsedInput);
void deposit(vector<string> parsedInput);
void send(vector<string> parsedInput);
void logout(vector<string> parsedInput);

int main(int argc, char *argv[]) {
  stringstream config;
  int fd = open("config.json", O_RDONLY);
  if (fd < 0) {
	cout << "could not open config.json" << endl;
	exit(1);
  }
  int ret;
  char buffer[4096];
  while ((ret = read(fd, buffer, sizeof(buffer))) > 0) {
	config << string(buffer, ret);
  }
  Document d;
  d.Parse(config.str());
  API_SERVER_PORT = d["api_server_port"].GetInt();
  API_SERVER_HOST = d["api_server_host"].GetString();
  PUBLISHABLE_KEY = d["stripe_publishable_key"].GetString();

  vector<string> parsedInput;
  string rawInput;
  if (argc == 1){ // interactive
    cout <<"D$>";
    while(getline(cin, rawInput)){
      parsedInput = parse(rawInput);
      if (parsedInput.size() == 0){
        errorMessage(); 
      }else{
        executeCmd(parsedInput);
      }
      cout<<"D$>";
    }
  }else if (argc == 2){ // batch
    for (int i = 1; i < argc; i++){
      ifstream inputFile;
      inputFile.open(argv[i]);
      if (!inputFile){
        errorMessage();
      }else{
        while(getline(cin, rawInput)){
          parsedInput = parse(rawInput);
          if (parsedInput.size() == 0){
            errorMessage();
          }else{
            executeCmd(parsedInput);
          }
        }
      }
    }
  }
  return 0;
}

void errorMessage(){
  cout<<"Error"<<endl;
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
void executeCmd(vector<string> parsedInput){
  if (parsedInput[0] == "logout"){
    // logout
  }else if (parsedInput[0] == "auth"){
    if (parsedInput.size() != 4){
      errorMessage();
      // cout <<"size"<<endl;
      return;
    }
    auth(parsedInput);
  }else if (parsedInput[0] == "balance"){
    if (parsedInput.size() != 1){
      errorMessage();
      return;
    }
    // cout<<"balance"<<endl;
  }else if (parsedInput[0]== "deposit"){
    if (parsedInput.size() != 6){
      errorMessage();
      return;
    }
    // cout<<"deposit"<<endl;
  }else if (parsedInput[0] == "send"){
    if (parsedInput.size() != 3){
      errorMessage();
      return;
    }
     // cout<<"send"<<endl;
  }
}

bool serverError(HTTPClientResponse *response){
  if (response->success()){
    return false;
  }else{
    return true;
  }
}

void printBalance(int balance) {
  float printBal =  balance / 100;
  cout << "Balance: $" << setprecision(2) << fixed << printBal << endl;
}

void auth(vector<string> parsedInput){
    string username = parsedInput[1];
    string password = parsedInput[2];
    string email = parsedInput[3];
    HTTPClientResponse *response;

    WwwFormEncodedDict body;
    body.set("username", username);
    body.set("password", password);
    string encoded_body = body.encode();
    
    int hostSize = API_SERVER_HOST.length();
    char host[hostSize + 1];
    strcpy(host, API_SERVER_HOST.c_str());

    HttpClient authCall(host, API_SERVER_PORT, false);
    response = authCall.post("/auth-tokens", encoded_body);
    if (serverError(response)){
      errorMessage();
      // cout <<"auth call "<<endl;
    }else{
      Document *d = response->jsonBody();
      string authTokenFromResponse = (*d)["auth_token"].GetString();
      string userIDFromResponse = (*d)["user_id"].GetString();
      delete d; 

      if (auth_token.length() != 0){
          // delete user and input new stuffs
          HttpClient deleteCall(host, API_SERVER_PORT, false);
          deleteCall.set_header("x-auth-token", auth_token);
          response = deleteCall.del("/auth-token/" + auth_token);
          if (serverError(response)){
            errorMessage();
            // cout<<"delete call"<<endl;
            return;
          }
      }
      auth_token = authTokenFromResponse;
      user_id = userIDFromResponse;

      WwwFormEncodedDict emailUpdate;
      emailUpdate.set("email", email);
      encoded_body = emailUpdate.encode();

      HttpClient emailClient(host, API_SERVER_PORT, false);
      emailClient.set_header("x-auth-token", auth_token);
      response = emailClient.put("/users/" + user_id, encoded_body);
      if (serverError(response)){
        errorMessage();
        // cout <<"email client"<<endl;
        return;
      }

      Document *e = response->jsonBody();
      int balance = (*e)["balance"].GetInt();
      delete e; 

      printBalance(balance);
    }
}