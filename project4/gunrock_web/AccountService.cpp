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

// Error-checking:
// user_id doesn't exist in the users database
// user_id doesn't match the authenticated user
// missing email argument
// email is in the wrong format?
AccountService::AccountService() : HttpService("/users") {
  
}

void AccountService::get(HTTPRequest *request, HTTPResponse *response) {
    std::map<string,User*>::iterator it;
    User *currUser; 
    string email;
    int balance;
    User *requestUser = getAuthenticatedUser(request);
    if (requestUser != NULL){
        vector<string> path = request->getPathComponents();
        string getAuthToken = path.back(); // last in parameter list
        // find user with getAuthToken in auth_token database
        for(it = m_db->auth_tokens.begin(); it != m_db->auth_tokens.end(); ++it){ // loop through database
            if (it->first == getAuthToken){
                currUser = it -> second;
                break;
            }
        }
        if (currUser == requestUser){ // ISSUE HERE (if get before put)
            email = currUser->email;
            balance = currUser->balance;
            rapidJSONResponse(email, balance, response);
            response->setStatus(200);
        }
        else{
            throw ClientError::forbidden();
            response->setStatus(403);
        }
    }else{
        response->setStatus(400);
    }
}

void AccountService::put(HTTPRequest *request, HTTPResponse *response) {
    /// create error check function?
    int balance;
    User *currUser = getAuthenticatedUser(request);
    User *checkUser;
    string checkAuthToken;
    std::map<string, User*>::iterator iter;

    if (request->hasAuthToken()){
        // pull email, balance, and user from request
        WwwFormEncodedDict fullRequest = request->formEncodedBody();
        string email = fullRequest.get("email");
        checkAuthToken = request->getAuthToken();
        if (email == ""){  // no email
            throw ClientError::badRequest();
            response-> setStatus(400);
        }else{
            for(iter = m_db->auth_tokens.begin(); iter != m_db->auth_tokens.end(); ++iter){ // loop through database
                if (iter->first == checkAuthToken){ 
                    checkUser = iter->second;
                }
            }
            if (checkUser == currUser){
                currUser->email = email;
                balance = currUser->balance;
                // set up response object
                rapidJSONResponse(email, balance, response);
                response->setStatus(200);
            }
            else{ // auth_token is not for correct user_id
                throw ClientError::unauthorized();
                response->setStatus(401);
            }
        }
    }else{ // no auth token
        response->setStatus(400);
    }
}


void rapidJSONResponse(string email, int balance, HTTPResponse *response){
    // use rapidjson to create a return object
    Document document;
    Document::AllocatorType& a = document.GetAllocator();
    Value o;
    o.SetObject();

    // add a key value pair directly to the object
    o.AddMember("balance", balance, a);
    o.AddMember("email", email, a);

    // now some rapidjson boilerplate for converting the JSON object to a string
    document.Swap(o);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);

    // set the return object
    response->setContentType("application/json");
    response->setBody(buffer.GetString() + string("\n"));
}