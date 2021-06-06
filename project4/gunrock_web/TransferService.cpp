#define RAPIDJSON_HAS_STDSTRING 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "TransferService.h"
#include "ClientError.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

void rapidJSONResponse(vector<Transfer*> transfer, HTTPResponse *response, User *currUser);
bool errorCheck(User* transferFromUser, HTTPRequest *request, HTTPResponse *response, map<string, User*> users);

TransferService::TransferService() : HttpService("/transfers") { }


void TransferService::post(HTTPRequest *request, HTTPResponse *response) {
    User *transferFromUser = getAuthenticatedUser(request);
    if (errorCheck(transferFromUser, request,response, m_db->users)){ // no errors

    }
}

void TransferService::get(HTTPRequest *request, HTTPResponse *response) {

}

void rapidJSONResponse(vector<Transfer*> transfer, HTTPResponse *response, User *currUser){
    // use rapidjson to create a return object
    Document document;
    Document::AllocatorType& a = document.GetAllocator();
    Value o;
    o.SetObject();

    // add a key value pair directly to the object
    o.AddMember("balance", currUser->balance, a);
    
    // create an array
    Value array;
    array.SetArray();

    // add an object to our array
    for(unsigned i = 0; i < transfer.size(); i++){ 
        if (transfer[i]->to == currUser){ // current user deposit
            Value to;
            to.SetObject();
            to.AddMember("from", transfer[i]->from->username, a);
            to.AddMember("to", transfer[i]->to->username , a);
            to.AddMember("amount", transfer[i]->amount , a);
            array.PushBack(to, a);
        }
    }

    // and add the array to our return object
    o.AddMember("transfers", array, a);

    // now some rapidjson boilerplate for converting the JSON object to a string
    document.Swap(o);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);

    // set the return object
    response->setContentType("application/json");
    response->setBody(buffer.GetString() + string("\n"));
}

bool errorCheck(User *transferFromUser, HTTPRequest *request, HTTPResponse *response,  map<string, User*> users){ // return true if no errors false otherwise
    WwwFormEncodedDict fullRequest = request->formEncodedBody();
    std::map<string, User*>::iterator iter;
    string transferToUsername = fullRequest.get("to");
    string amountToTransferStr = fullRequest.get("amount");
    if (transferToUsername == "" || amountToTransferStr == ""){ //missing agruements
        throw ClientError::badRequest();
        return false;
    }
    std::string::size_type sz;
    int amount = stoi(amountToTransferStr, &sz);
    User *transferToUser = NULL;

    if ((transferFromUser->balance - amount < 0) || amount < 0 ){ // negative amount or greater than balance
        throw ClientError::badRequest();
        return false;
    }
    for(iter = users.begin(); iter != users.end(); ++iter){ // loop through database
      if (iter->first == transferToUsername){ 
        transferToUser = iter->second;
      }
    }
    if (transferToUser == NULL){ // to User does not exist
        throw ClientError::notFound(); 
        return false;
    }
    return true;
}