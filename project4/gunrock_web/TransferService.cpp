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

void rapidJSONResponse(vector<Transfer*> transfer, HTTPResponse *response, User *transferFromUser, User *transferToUser);
bool errorCheck(User* transferFromUser, HTTPRequest *request, HTTPResponse *response, map<string, User*> users);

TransferService::TransferService() : HttpService("/transfers") { }


void TransferService::post(HTTPRequest *request, HTTPResponse *response) {
    std::map<string,User*>::iterator it;
    User *transferFromUser = getAuthenticatedUser(request);
    if (errorCheck(transferFromUser, request,response, m_db->users)){ // no errors
        WwwFormEncodedDict fullRequest = request->formEncodedBody();
        int amount = stoi(fullRequest.get("amount"));
        string userToTransfer = fullRequest.get("to");

        User *transferToUser;
        for(it = m_db->users.begin(); it != m_db->users.end(); ++it){ // loop through database
            if (it->first == userToTransfer){
                transferToUser = it -> second;
            }
        }
        // create transfer obj
        Transfer *currTransfer = new Transfer();
        currTransfer->amount = amount;
        currTransfer->from = transferFromUser;
        currTransfer->to = transferToUser;
        
        // update balances
        transferFromUser->balance -= amount;
        transferToUser->balance += amount;
        m_db->transfers.push_back(currTransfer);
        rapidJSONResponse(m_db->transfers, response, transferFromUser, transferToUser);
    }
}

void TransferService::get(HTTPRequest *request, HTTPResponse *response) {

}

void rapidJSONResponse(vector<Transfer*> transfer, HTTPResponse *response, User *transferFromUser, User *transferToUser){
    Document document;
    Document::AllocatorType& a = document.GetAllocator();
    Value o;
    o.SetObject();
    Value array;
    array.SetArray();
    Value to;

    o.AddMember("balance", transferFromUser->balance, a);
    unsigned size = transfer.size();
    for(unsigned i = 0; i < size; i++){ 
        if (transfer[i]->from->username == transferFromUser->username && transfer[i]->to->username == transferToUser->username){ // current user deposit
            to.SetObject();
            to.AddMember("from", transfer[i]->from->username, a);
            to.AddMember("to", transfer[i]->to->username , a);
            to.AddMember("amount", transfer[i]->amount , a);
            array.PushBack(to, a);
        }
    }

    o.AddMember("transfers", array, a);
    document.Swap(o);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);
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
    if (transferFromUser == transferToUser){ // transfer to self
        throw ClientError::badRequest();
        return false;
    }
    return true;
}