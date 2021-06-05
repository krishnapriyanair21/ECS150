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

// Error-checking:
// - missing amount or stripe_token arguments
// - deposit amount is negative
// - charge amount less than 50 cents
// - potential stripe API errors?
void rapidJSONResponse(int balance , string transfer[], HTTPResponse *response);
int errorCheck(int amount);

DepositService::DepositService() : HttpService("/deposits") { }

void DepositService::post(HTTPRequest *request, HTTPResponse *response) {
    WwwFormEncodedDict fullRequest = request->formEncodedBody();
    // pull amount and Stripe token
    string amountString = fullRequest.get("amount"); 
    std::string::size_type sz;
    int amount = std::stoi (amountString, &sz); // convert to int (stoi works in CSIF)
    string stripeToken = fullRequest.get("stripe_token");


}

void DepositService::get(HTTPRequest *request, HTTPResponse *response) {

}

void rapidJSONResponse(int balance , string transfer[], HTTPResponse *response){
    // use rapidjson to create a return object
    Document document;
    Document::AllocatorType& a = document.GetAllocator();
    Value o;
    o.SetObject();

    // add a key value pair directly to the object
    o.AddMember("balance", balance, a);
    
    // create an array
    Value array;
    array.SetArray();

    // add an object to our array
    Value to;
    to.SetObject();
    to.AddMember("to", transfer[0], a);
    array.PushBack(to, a);

    Value to1;
    to.SetObject();
    to.AddMember("amount", transfer[1], a);
    array.PushBack(to1, a);

    Value to2;
    to.SetObject();
    to.AddMember("stripe_charge_id", transfer[2], a);
    array.PushBack(to2, a);

    // and add the array to our return object
    o.AddMember("transfer", array, a);

    // now some rapidjson boilerplate for converting the JSON object to a string
    document.Swap(o);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);

    // set the return object
    response->setContentType("application/json");
    response->setBody(buffer.GetString() + string("\n"));
}

int errorCheck(int amount){
    if (amount < 50){
        return 1; // error amount is < 50
    }
    return 0;
}