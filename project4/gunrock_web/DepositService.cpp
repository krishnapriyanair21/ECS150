#define RAPIDJSON_HAS_STDSTRING 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "DepositService.h"
#include "Database.h"
#include "ClientError.h"
#include "HTTPClientResponse.h"
#include "HttpClient.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;


void rapidJSONResponse(vector<Deposit*> deposit, HTTPResponse *response, User *currUser);
bool errorCheck(HTTPRequest *request, HTTPResponse *response);

DepositService::DepositService() : HttpService("/deposits") { }

void DepositService::post(HTTPRequest *request, HTTPResponse *response) {
    if (errorCheck(request, response)){ // no errors
        WwwFormEncodedDict fullRequest = request->formEncodedBody();
        int amount = stoi(fullRequest.get("amount"));
        string stripeToken = fullRequest.get("stripe_token");
        User *currUser = getAuthenticatedUser(request);
        
        WwwFormEncodedDict body;
        body.set("amount", amount);
        body.set("source", stripeToken);
        body.set("currency", "usd");
        string encoded_body = body.encode();

        HttpClient client("api.stripe.com", 443, true);
        client.set_basic_auth(m_db->stripe_secret_key, "");
        HTTPClientResponse *client_response = client.post("/v1/charges", encoded_body);
        if (client_response->success()){
            Document *d = client_response->jsonBody();

            Deposit *currDeposit = new Deposit();
            currDeposit->amount = (*d)["amount"].GetInt();
            currUser->balance += currDeposit->amount;
            currDeposit->to = currUser;
            currDeposit->stripe_charge_id = (*d)["id"].GetString(); 
            delete d;

            m_db->deposits.push_back(currDeposit);
            rapidJSONResponse(m_db->deposits, response, currUser);
            response->setStatus(200);
        }else{
            throw ClientError::badRequest(); // invalid stripe token
        }
    }
}

void DepositService::get(HTTPRequest *request, HTTPResponse *response) {

}

void rapidJSONResponse(vector<Deposit*> deposit, HTTPResponse *response, User *currUser){
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
    for(unsigned i = 0; i < deposit.size(); i++){ 
        if (deposit[i]->to == currUser){ // current user deposit
            Value to;
            to.SetObject();
            to.AddMember("to", deposit[i]->to->username, a);
            to.AddMember("amount", deposit[i]->amount , a);
            to.AddMember("stripe_charge_id", deposit[i]->stripe_charge_id , a);
            array.PushBack(to, a);
        }
    }

    // and add the array to our return object
    o.AddMember("deposits", array, a);

    // now some rapidjson boilerplate for converting the JSON object to a string
    document.Swap(o);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);

    // set the return object
    response->setContentType("application/json");
    response->setBody(buffer.GetString() + string("\n"));
}

bool errorCheck(HTTPRequest *request, HTTPResponse *response){ // if error return false else return true
    WwwFormEncodedDict fullRequest = request->formEncodedBody();
    string amountString = fullRequest.get("amount"); 
    string stripeToken = fullRequest.get("stripe_token");
    if (stripeToken == "" || amountString == ""){  // no amount or stripe token
        throw ClientError::badRequest();
        return false;
    }
    std::string::size_type sz;
    int amount = std::stoi(amountString, &sz); // convert to int (stoi works in CSIF)

    if (amount < 50){ // amount is < 50
        throw ClientError::badRequest();
        return false; 
    }
    if (!(request->hasAuthToken())){ // no auth token
        throw ClientError::badRequest();
    }

    return true;
}
/* Error Checks */
// 1 amount is less than 50 cents
// 2 no amount provided or no stripe_token provided
// 3 deposit amount negative