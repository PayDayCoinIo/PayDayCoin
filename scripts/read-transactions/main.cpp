#include <stdio.h>
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

string curl_make_request(
    const char* method,
    const char* params
    )

{
    string result="";
    CURL *curl; //объект типа CURL
    curl = curl_easy_init(); //инициализация
    struct curl_slist *headers = NULL;
    //curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);

    if (curl) {
        char data[256];
        strcpy(data,"");
        strcat(data,"{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"");
        strcat(data,method);
        strcat(data,"\", \"params\": [\"");
        strcat(data,params);
        strcat(data,"\"] }");

        //std::cout << "Data: " << data << std::endl;

        headers = curl_slist_append(headers, "content-type: text/plain;");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:7215/");

        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        curl_easy_setopt(curl, CURLOPT_USERPWD,
                         "PayDayrpc:2A9xXtyJ2u7n34teGJfAJDUwzc5gQm5GQZGUMK8SxrGQ");

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


bool check_transaction( const char* trhash)
{
    bool status = false;

    string request = curl_make_request("gettransaction",trhash);
    if (request.empty()) return false;
    Json::Value jsonData;
    Json::Value jsonResult;
    Json::Reader jsonReader;

    if (jsonReader.parse(request, jsonData))
    {
        //std::cout << "Error: " << jsonData["error"].asBool() << std::endl;
        if (jsonData["error"].asBool() == false && jsonReader.parse(jsonData["result"].toStyledString(), jsonResult))
        {

            //const std::string valueString(jsonResult["confirmations"].asString());
            //std::cout << "Confirmations: " << valueString << std::endl;

            status = jsonResult["confirmations"].asBool();

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
    return status;
}

int main(int argc, char *argv[])
{
    string trline;
    string delim = ",";
    size_t prev = 0;
    size_t next;
    size_t delta = delim.length();
    regex trs(".*\"[truefals]*\".*");

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

    if (argc != 2)
    {
        fprintf(stderr,
                "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

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

        /*for (map<string,float>::iterator it = adresses.begin(); it != adresses.end(); ++it)
        {
            std::cout << "Address: " << it->first << " and amount: " << fixed << std::setprecision(6) << it->second << std::endl;
        }*/
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

    }

    return 0;
}
