#define RAPIDJSON_HAS_STDSTRING 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "AccountService.h"
#include "ClientError.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;
void rapidJSONResponse(string email, int balance, HTTPResponse *response);

AccountService::AccountService() : HttpService("/users") {
  
}

void AccountService::get(HTTPRequest *request, HTTPResponse *response) {
    std::map<string,User*>::iterator it;
    User *currUserWithID; 
    string email;
    int balance;
    User *requestUser = getAuthenticatedUser(request);
    if (requestUser != NULL){
        vector<string> path = request->getPathComponents();
        string getUserID = path.back(); // last in parameter list
        for(it = m_db->users.begin(); it != m_db->users.end(); ++it){ // loop through database
            if (it->second->user_id == getUserID){ // user_id check
                currUserWithID = it -> second;
            }
        }
        if (currUserWithID == requestUser){ 
            email = currUserWithID->email;
            balance = currUserWithID->balance;
            rapidJSONResponse(email, balance, response);
        }
        else{
            throw ClientError::forbidden();
        }
    }else{
        throw ClientError::badRequest();
    }
}

void AccountService::put(HTTPRequest *request, HTTPResponse *response) {
    int balance;
    User *currUserwithAuthToken = getAuthenticatedUser(request);
    User *checkUserwithUserID;
    string userIDfromURL;
    std::map<string, User*>::iterator iter;
    vector<string> path = request->getPathComponents();

    if (request->hasAuthToken()){
        // pull email, balance, and user from request
        WwwFormEncodedDict fullRequest = request->formEncodedBody();
        string email = fullRequest.get("email");
        userIDfromURL = path.back();
        if (email == ""){  // no email
            throw ClientError::badRequest();
        }else{
            for(iter = m_db->users.begin(); iter != m_db->users.end(); ++iter){ // loop through database
                if (iter->second->user_id == userIDfromURL){  // find user_id in User 
                    checkUserwithUserID = iter->second;
                }
            }
            if (checkUserwithUserID == currUserwithAuthToken){
                currUserwithAuthToken->email = email;
                balance = currUserwithAuthToken->balance;
                rapidJSONResponse(email, balance, response);
            }
            else{ // auth_token is not for correct user_id
                throw ClientError::unauthorized();
            }
        }
    }else{ // no auth token
        throw ClientError::badRequest();
    }
}


void rapidJSONResponse(string email, int balance, HTTPResponse *response){
    Document document;
    Document::AllocatorType& a = document.GetAllocator();
    Value o;
    o.SetObject();
    o.AddMember("balance", balance, a);
    o.AddMember("email", email, a);
    document.Swap(o);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);
    response->setContentType("application/json");
    response->setBody(buffer.GetString() + string("\n"));
}