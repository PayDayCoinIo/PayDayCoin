#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <iomanip>

using namespace std;

#define bufSize 1024


map<string,string> readConfig();
std::string getEnvVar( std::string const & key );
string curl_make_request(
    const char* method,
    const char* params,
    int treq
    );
string get_block(int num);
vector<map<string,string>> check_transaction( const char* trhash);
unsigned int get_block_count();

map<string, double> pownodes;
map<string, double> masternodes;
map<string, double> missingnodes;
//map<string,string> trdata;

std::size_t callback(
    const char* in,
    std::size_t size,
    std::size_t num,
    std::string* out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}


std::string getEnvVar( std::string const & key )
{
    char * val = getenv( key.c_str() );
    return val == NULL ? std::string("") : std::string(val);
}


map<string,string> readConfig()
{
    map<string,string> _result;
    char cpath[50];
    strcpy(cpath,getEnvVar("HOME").c_str());
    strcat(cpath,"/.PayDay/PayDay.conf");
    ifstream fconf (cpath);
    string strparam;
    string rpc1f = "rpcuser";
    string rpc2f = "rpcpassword";
    //std::cout << cpath << std::endl;
    if (fconf.is_open())
    {

        while (getline(fconf,strparam))
        {
            if (strparam.length() > 0) {

                std::size_t found = strparam.find("#");
                if (found!=std::string::npos) continue;
                found = strparam.find("=");
                if (found!=std::string::npos) {
                    std::size_t found2 = strparam.find(rpc1f);
                    if (found2!=std::string::npos) _result.insert(make_pair(rpc1f,strparam.substr(rpc1f.length()+1,strparam.length()))); //std::cout << strparam.substr(0,rpc1f.length()) << " is " << strparam.substr(rpc1f.length()+1,strparam.length()) << std::endl;
                    found2 = strparam.find(rpc2f);
                    if (found2!=std::string::npos) _result.insert(make_pair(rpc2f,strparam.substr(rpc2f.length()+1,strparam.length())));//std::cout << strparam.substr(0,rpc2f.length()) << " is " << strparam.substr(rpc2f.length()+1,strparam.length()) << std::endl;
                }

            }
        }
    }

    return _result;

}

string curl_make_request(
    const char* method,
    const char* params,
    int treq
    )

{
    char _par[255];
    char credentials[50];
    string result="";
    CURL *curl; //объект типа CURL
    curl = curl_easy_init(); //инициализация
    struct curl_slist *headers = NULL;
    //curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);

    map<string,string> vars = readConfig();
    if (vars.size() == 2) {
        strcpy(credentials,vars["rpcuser"].c_str());
        strcat(credentials,":");
        strcat(credentials,vars["rpcpassword"].c_str());
    } else return result;
    if (treq == 0) {
        strcpy(_par,"\"");
        strcat(_par,params);
        strcat(_par,"\"");
    } else if (treq == 1) {
        strcpy(_par,params);
    }
     //std::cout << "Value: " << params << std::endl;

    if (curl) {
        char data[256];
        strcpy(data,"");
        strcat(data,"{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"");
        strcat(data,method);
        strcat(data,"\", \"params\": [");
        strcat(data,_par);
        strcat(data,"] }");

        //std::cout << "Data: " << data << std::endl;

        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:7215/");

        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        curl_easy_setopt(curl, CURLOPT_USERPWD,
                         credentials);

        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

        long httpCode(0);
        std::unique_ptr<std::string> httpData(new std::string());

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

        curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        if (httpCode == 200) {
            return *httpData.get();
        } else
        {
            //result.push_back("failed");
            return result;
        }
    }

    return result;

}


unsigned int get_block_count()
{
    unsigned int count = 0;
    string status = "";
    string btype = "";
    string trhash;
    map<string, string> tr_res;

    map<string,double>::iterator trit;
    string request = curl_make_request("getinfo","",1);

    if (request.empty()) return 0;
    Json::Value jsonData;
    Json::Value jsonBlock;
    Json::Value jsonTransaction;
    Json::Reader jsonReader;

    if (jsonReader.parse(request, jsonData))
    {
        //std::cout << "Error: " << jsonData["error"].asBool() << std::endl;
        if (jsonData["error"].asBool() == false && jsonReader.parse(jsonData["result"].toStyledString(), jsonBlock))
        {

            //const std::string valueString(jsonResult["confirmations"].asString());
            //std::cout << "Confirmations: " << valueString << std::endl;

            count = jsonBlock["blocks"].asInt();
            //std::cout << "Count: " << count << std::endl;

            /*if (jsonResult["confirmations"].asInt() > 0 ) {
                status = true;
            } else
            {
                status = false;
            }*/
            /*for (Json::Value::const_iterator itr = jsonResult.begin(); itr != jsonResult.end(); itr++)
            {
                const std::string valueString(itr.key().asString());
                std::cout << "Value: " << valueString << std::endl;
            }*/
        }
    }
    else
    {
        std::cout << "Could not parse HTTP data as JSON" << std::endl;
        //std::cout << "TR: " << trhash << std::endl;
    }
    return count;
}

string get_block(int num)
{
    string status = "";
    string btype = "";
    string trhash;
    vector<map<string, string>> tr_res;

    map<string,double>::iterator trit;
    map<string,double>::iterator trit2;
    map<string,double>::iterator trit3;
    stringstream p;
    p << num;
    string request = curl_make_request("getblockbynumber",p.str().c_str(),1);
    if (request.empty()) return status;
    Json::Value jsonData;
    Json::Value jsonBlock;
    Json::Value jsonTransaction;
    Json::Reader jsonReader;

    if (jsonReader.parse(request, jsonData))
    {
        //std::cout << "Error: " << jsonData["error"].asBool() << std::endl;
        if (jsonData["error"].asBool() == false && jsonReader.parse(jsonData["result"].toStyledString(), jsonBlock))
        {

            //const std::string valueString(jsonResult["confirmations"].asString());
            //std::cout << "Confirmations: " << valueString << std::endl;

            btype = jsonBlock["flags"].asString();

            //std::size_t found = btype.find("proof-of-stake");
            //if (found!=std::string::npos) {

                if (jsonReader.parse(jsonBlock["tx"].toStyledString(), jsonTransaction))
                {
                    //std::cout << "Block: " << num << " is POS" << std::endl;
                    for ( unsigned int tr =0; tr < jsonTransaction.size(); tr++)
                    {
                        trhash = jsonTransaction[tr].asString();
                        tr_res = check_transaction(trhash.c_str());
                        for( unsigned int u =0; u < tr_res.size(); u++)
                        {
                            if (tr_res[u]["status"] == "1"){



                                if (tr_res[u]["type"] == "pow") {

                                    trit = pownodes.find(tr_res[u]["address"]);
                                    if (trit != pownodes.end()) {
                                        pownodes[tr_res[u]["address"]]+=stod(tr_res[u]["value"]);
                                    } else {
                                        pownodes.insert(make_pair(tr_res[u]["address"],stod(tr_res[u]["value"])));
                                    }
                                }

                                if (tr_res[u]["type"] == "mn") {

                                    trit = masternodes.find(tr_res[u]["address"]);
                                    if (trit != masternodes.end()) {
                                        masternodes[tr_res[u]["address"]]+=stod(tr_res[u]["value"]);
                                    } else {
                                        masternodes.insert(make_pair(tr_res[u]["address"],stod(tr_res[u]["value"])));
                                    }

                                }

                                if (tr_res[u]["type"] == "pos") {
                                    trit = masternodes.find(tr_res[u]["address"]);

                                    if (trit != masternodes.end()) {
                                        masternodes[tr_res[u]["address"]]+=stod(tr_res[u]["value"]);
                                    } else {
                                        trit2 = pownodes.find(tr_res[u]["address"]);
                                        if (trit2 != pownodes.end()) {
                                            pownodes[tr_res[u]["address"]]+=stod(tr_res[u]["value"]);
                                        }
                                        else
                                        {
                                            std::cout <<  num << "|" << trhash << "|" << tr_res[u]["address"] << "|" << tr_res[u]["type"] << "|" << tr_res[u]["value"] << std::endl;
                                            trit3 = missingnodes.find(tr_res[u]["address"]);
                                            if (trit3 != missingnodes.end()) {
                                                missingnodes[tr_res[u]["address"]]+=stod(tr_res[u]["value"]);
                                            } else {
                                                missingnodes.insert(make_pair(tr_res[u]["address"],stod(tr_res[u]["value"])));
                                            }
                                        }
                                        //missingnodes.insert(make_pair(tr_res["address"],stod(tr_res["value"])));
                                    }
                                }
                            }
                        }
                    }
                }
            //}

            /*if (jsonResult["confirmations"].asInt() > 0 ) {
                status = true;
            } else
            {
                status = false;
            }*/
            /*for (Json::Value::const_iterator itr = jsonResult.begin(); itr != jsonResult.end(); itr++)
            {
                const std::string valueString(itr.key().asString());
                std::cout << "Value: " << valueString << std::endl;
            }*/
        }
    }
    else
    {
        std::cout << "Could not parse HTTP data as JSON" << std::endl;
        //std::cout << "TR: " << trhash << std::endl;
    }
    return status;
}

vector<map<string, string>> check_transaction( const char* trhash)
{
    vector<map<string, string>> result;
    map<string, string> trdata;
    bool status = false;
    int value = 0;
    string address = "";
    string type = "mn";

    string request = curl_make_request("gettransaction",trhash,0);
    if (request.empty()) return result;
    Json::Value jsonData;
    Json::Value jsonTransaction;
    Json::Value jsonTrOut;
    Json::Reader jsonReader;

    if (jsonReader.parse(request, jsonData))
    {
        //std::cout << "Error: " << jsonData["error"].asBool() << std::endl;
        if (jsonData["error"].asBool() == false && jsonReader.parse(jsonData["result"].toStyledString(), jsonTransaction))
        {

            //const std::string valueString(jsonResult["confirmations"].asString());
            //std::cout << "Confirmations: " << valueString << std::endl;

            status = jsonTransaction["confirmations"].asBool();

            if (status) {

                if (jsonReader.parse(jsonTransaction["vout"].toStyledString(), jsonTrOut))
                {
                    for ( unsigned int j = 0; j < jsonTrOut.size(); j++) {

                        trdata.insert(make_pair("status","0"));
                        trdata.insert(make_pair("type",type));
                        trdata.insert(make_pair("address",address));
                        trdata.insert(make_pair("value",""));

                        value = jsonTrOut[j]["value"].asInt();
                        //std::cout << "Value TR: " << value << std::endl;
                        if (value == 15910){
                            trdata["status"] = "1";
                            trdata["value"] = jsonTrOut[j]["value"].asString();
                            trdata["type"] = "pow"; //std::cout << "Value TR: " << value << std::endl;
                            trdata["address"] = jsonTrOut[j]["scriptPubKey"]["addresses"][0].asString();
                        }

                        if (value == 20000){
                            trdata["status"] = "1";
                            trdata["value"] = jsonTrOut[j]["value"].asString();
                            trdata["type"] = "mn"; //std::cout << "Value TR: " << value << std::endl;
                            trdata["address"] = jsonTrOut[j]["scriptPubKey"]["addresses"][0].asString();
                        }

                        if (value == 210){

                            trdata["status"] = "1";
                            trdata["value"] = jsonTrOut[j]["value"].asString();
                            trdata["type"] = (jsonTrOut[0]["scriptPubKey"]["type"] == "nonstandard" ? "pos" : "tx"); //std::cout << "Value TR: " << value << std::endl;
                            trdata["address"] = jsonTrOut[j]["scriptPubKey"]["addresses"][0].asString();

                            //std::cout << "Value TR: " << value << std::endl;
                        }
                        result.push_back(trdata);
                    }

                }

            }
            /*if (jsonResult["confirmations"].asInt() > 0 ) {
                status = true;
            } else
            {
                status = false;
            }*/
            /*for (Json::Value::const_iterator itr = jsonResult.begin(); itr != jsonResult.end(); itr++)
            {
                const std::string valueString(itr.key().asString());
                std::cout << "Value: " << valueString << std::endl;
            }*/
        }
    }
    else
    {
        std::cout << "Could not parse HTTP data as JSON" << std::endl;
        std::cout << "TR: " << trhash << std::endl;
    }
    return result;
}

int main(int argc, char *argv[])
{
    //string trline;
    //string delim = ",";
    //size_t prev = 0;
    //size_t next;
    //size_t delta = delim.length();
    //regex trs(".*\"[truefals]*\".*");
    readConfig();
/*
    map<string,double> adresses;
    map<string,double>::iterator tr_it;
    vector<string> transactions;
    vector<string> trdata;

    map<string,double> t_amounts;
    //map<string, double> f_amounts;
    map<string,double>::iterator tait;

    //double true_amount =0;
    //double all_sented = 0;
    //double all_mined =0;
    //double all_received=0;
    //double all_other =0;
    double false_amount=0;
*/
    if (argc != 2)
    {
        fprintf(stderr,
                "Usage: %s <start-block>\n", argv[0]);
        return 1;
    }
    int block_pos = atoi(argv[1]);
    unsigned int bcount = get_block_count();

    std::cout << "Block|Transaction|Address|Type|Amount" << std::endl;

    if (block_pos > 0) {
        for (unsigned int i = block_pos; i < bcount; i++)
        {
            get_block(i);
            /*if (i % 5000 == 0) {
                std::cout << "Block: " << i << std::endl;
                std::cout << "All MN: " << masternodes.size() << std::endl;
                std::cout << "All POW: " << pownodes.size() << std::endl;
                std::cout << "All MISS: " << missingnodes.size() << std::endl;
            }*/
        }
    }
    double reward = 0.0;

    for (map<string,double>::iterator it = missingnodes.begin(); it != missingnodes.end(); ++it)
    {
        reward += it->second;
    }

    std::cout << "All Masternodes: " << masternodes.size() << std::endl;
    std::cout << "Count Missing Masternodes: " << missingnodes.size() << std::endl;
    std::cout << "All Missing Masternodes reward: " << reward << std::endl;
/*
    ifstream myfile (argv[1]);

    if (myfile.is_open())
    {

        while (getline(myfile,trline))
        {
            if(regex_search(trline,trs))
            {
                prev = 0;
                while( ( next = trline.find( delim, prev ) ) != string::npos ) {
                    trdata.push_back( trline.substr( prev, next-prev ) );
                    prev = next + delta;
                }
                trdata.push_back( trline.substr( prev ) );


                // 0 - true/false
                // 1 - date
                // 2 - type
                // 3 - label
                // 4 - address
                // 5 - amount
                // 6 - tr id

                if ( trdata.size() == 7) {

                    for (unsigned int i = 0; i < trdata.size(); i++)
                    {
                        trdata[i].erase(remove(trdata[i].begin(),trdata[i].end(), '\"'), trdata[i].end());
                        //std::cout << "String: " << trdata[i] << std::endl;
                    }
                    trdata[6].erase(trdata[6].size() -1);

                    transactions.push_back(trdata[6]);

                    if ( trdata[0] == "true" ) {

                        if ( trdata[2] == "Mined") {
                            //all_mined+=stod(trdata[5]);

                            tait = t_amounts.find("mined");
                            if (tait != t_amounts.end()) {
                                t_amounts["mined"]+=stod(trdata[5]);
                            } else {
                                t_amounts.insert(make_pair("mined",stod(trdata[5])));
                            }

                            tr_it = adresses.find(trdata[4]);
                            if (tr_it != adresses.end()) {
                                tr_it->second+=stod(trdata[5]);
                            } else {
                                adresses.insert(make_pair(trdata[4],stod(trdata[5])));
                                //std::cout << "Mined: " << trdata[6] << std::endl;
                            }
                            //std::cout << "Mined: " << trdata[6] << std::endl;
                        }else if (trdata[2] == "Sent to" ){

                            tait = t_amounts.find("sented");
                            if (tait != t_amounts.end()) {
                                t_amounts["sented"]+=stod(trdata[5]);
                            } else {
                                t_amounts.insert(make_pair("sented",stod(trdata[5])));
                            }
                            //all_sented+=stod(trdata[5]);

                        } else if (trdata[2] == "Received with" ){

                            tait = t_amounts.find("received");
                            if (tait != t_amounts.end()) {
                                t_amounts["received"]+=stod(trdata[5]);
                            } else {
                                t_amounts.insert(make_pair("received",stod(trdata[5])));
                            }

                            //all_received+=stod(trdata[5]);
                        } else {

                            tait = t_amounts.find("other");
                            if (tait != t_amounts.end()) {
                                t_amounts["other"]+=stod(trdata[5]);
                            } else {
                                t_amounts.insert(make_pair("other",stod(trdata[5])));
                            }
                            //all_other+=stod(trdata[5]);
                        }

                        tait = t_amounts.find("all");
                        if (tait != t_amounts.end()) {
                            t_amounts["all"]+=stod(trdata[5]);
                        } else {
                            t_amounts.insert(make_pair("all",stod(trdata[5])));
                        }
                        //true_amount+=stod(trdata[5]);


                    }

                    if (trdata[0] == "false") {
                        if ( trdata[2] == "Mined") {
                            //std::cout << "Mined: " << trdata[6] << std::endl;

                        } else if (trdata[2] == "Sent to" ){


                        }
                        bool trstatus = check_transaction(trdata[6].c_str());
                        if (trstatus) {
                            tait = t_amounts.find("all");
                            if (tait != t_amounts.end()) {
                                t_amounts["all"]+=stod(trdata[5]);
                            } else {
                                t_amounts.insert(make_pair("all",stod(trdata[5])));
                            }
                        } else
                        {
                            false_amount+=stod(trdata[5]);
                        }

                    }




                    //vector<string> result = curl_make_request("getinfo", "");
                    //vector<string> result = curl_make_request("getinfo", "\"one\", \"two\"");
                    //for (unsigned int i=0; i < result.size(); i++) {
                    //    std::cout << "Status: " << result[i] << std::endl;
                    //}

                    //std::cout << "Count: " << result.size() << std::endl;
                    //std::cout << "." << std::endl;
                } else {
                    printf(trline.c_str(),"short array: %s");
                }

                trdata.erase(trdata.begin(),trdata.end());
            }
        }
        myfile.close();


        for (map<string,float>::iterator it = adresses.begin(); it != adresses.end(); ++it)
        {
            std::cout << "Address: " << it->first << " and amount: " << fixed << std::setprecision(6) << it->second << std::endl;
        }
        std::cout << "Address count: " << adresses.size() << std::endl;
        std::cout << "All transactions: " << transactions.size() << std::endl;
        std::cout << "Statistics for confirmed transactions: " << std::endl;
        std::cout << "All mined money: " << fixed << std::setprecision(4) << t_amounts["mined"] << std::endl;
        std::cout << "All sented money: " << fixed << std::setprecision(4) << t_amounts["sented"] << std::endl;
        std::cout << "All received money: " << fixed << std::setprecision(4) << t_amounts["received"] << std::endl;
        std::cout << "All unknown money: " << fixed << std::setprecision(4) << t_amounts["other"] << std::endl;
        std::cout << "All real money: " << fixed << std::setprecision(4) << t_amounts["all"] << std::endl;
        std::cout << "Statistics for failed transactions: " << std::endl;
        std::cout << "All failed money: " << fixed << std::setprecision(4) << false_amount << std::endl;

    }*/

    return 0;
}
