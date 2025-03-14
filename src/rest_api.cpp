#include "rest_api.hpp"
#include "utils.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <global_vars.hpp>
#include "latency_tracker.hpp"

using json = nlohmann::json;
using namespace std;

void accountsummary(const string& access_token) {
    string url = "https://test.deribit.com/api/v2/private/get_account_summary?currency=BTC";
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "CURL init failed (account summary)." << endl;
        return;
    }

    string response;
    struct curl_slist* headers = nullptr;
    string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "CURL error (account summary): " << curl_easy_strerror(res) << endl;
    } else {
        cout << "[Account Summary Response]:\n" << response << endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void placebuyorder(const string& access_token, const string& instrument_name, int amount, const string& order_type, double price, const string& label) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "CURL init failed (placeOrderPOST)" << endl;
        return;
    }

    string url = "https://test.deribit.com/api/v2/private/buy";
    url += "?amount=" + to_string(amount);
    url += "&instrument_name=" + instrument_name;
    url += "&type=" + order_type;
    url += "&label=" + label;

    if (order_type == "limit") {
        url += "&price=" + to_string(price);
    }

    string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    latency_tracker.start("REST");
    CURLcode res = curl_easy_perform(curl);
    latency_tracker.stop("REST");
    if (res != CURLE_OK) {
        cerr << "CURL error (placeOrderPOST): " << curl_easy_strerror(res) << endl;
    } else {
        cout << "[Order POST Response]:" << endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void placesellorder(const string& access_token, const string& instrument_name, int amount, const string& order_type, double price, const string& label, const string& trigger, double trigger_price) {
    string url = "https://test.deribit.com/api/v2/private/sell";
    url += "?amount=" + to_string(amount);
    url += "&instrument_name=" + instrument_name;
    url += "&type=" + order_type;
    url += "&label=" + label;

    if (order_type == "limit" || order_type == "stop_limit") {
        url += "&price=" + to_string(price);
    }

    if (!trigger.empty()) {
        url += "&trigger=" + trigger;
        url += "&trigger_price=" + to_string(trigger_price);
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "CURL init failed (Sell)" << endl;
        return;
    }

    string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        cerr << "CURL error: " << curl_easy_strerror(res) << endl;
    else
        cout << "[Sell Order Response]:\n" << response << endl;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void cancelorder(const string& access_token, const string& order_id) {
    string url = "https://test.deribit.com/api/v2/private/cancel?order_id=" + order_id;

    CURL* curl = curl_easy_init();
    if (!curl) return;

    string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        cerr << "CURL error: " << curl_easy_strerror(res) << endl;
    else
        cout << "[Cancel Order Response]:\n" << response << endl;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void modifyorder(const string& access_token, const string& order_id, int new_amount, double new_price) {
    string url = "https://test.deribit.com/api/v2/private/edit";
    url += "?order_id=" + order_id;
    url += "&amount=" + to_string(new_amount);
    url += "&price=" + to_string(new_price);

    CURL* curl = curl_easy_init();
    if (!curl) return;

    string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        cerr << "CURL error: " << curl_easy_strerror(res) << endl;
    else
        cout << "[Edit Order Response]:\n" << response << endl;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void modifystoplimit(const string& access_token, const string& order_id, const string& instrument_name, int new_amount, double new_limit_price, double new_trigger_price, const string& new_label, const string& trigger) {
    cancelorder(access_token, order_id);
    placesellorder(access_token, instrument_name, new_amount, "stop_limit", new_limit_price, new_label, trigger, new_trigger_price);
}

void getorderbook(const string& instrument_name) {
    string url = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument_name;

    CURL* curl = curl_easy_init();
    if (!curl) return;

    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        cerr << "CURL error: " << curl_easy_strerror(res) << endl;
    else
        cout << "[Order Book for " << instrument_name << "]:\n" << response << endl;

    curl_easy_cleanup(curl);
}


map<string, vector<string>> fetch_all_instruments() {
    map<string, vector<string>> instrument_map;
    const vector<string> kinds = {"spot", "future", "option"};
    
    for (const auto& kind : kinds) {
        string url = "https://test.deribit.com/api/v2/public/get_instruments?currency=BTC&kind=" + kind + "&expired=false";
        CURL* curl = curl_easy_init();
        if (!curl) {
            cerr << "[CURL init failed]: " << kind << endl;
            continue;
        }
        
        string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "[CURL error - " << kind << "]: " << curl_easy_strerror(res) << endl;
            curl_easy_cleanup(curl);
            continue;
        }
        
        try {
            json parsed = json::parse(response);
            if (parsed.contains("result") && parsed["result"].is_array()) {
                for (const auto& instrument : parsed["result"]) {
                    if (instrument.contains("instrument_name")) {
                        instrument_map[kind].push_back(instrument["instrument_name"]);
                    }
                }
            }
        } catch (const exception& e) {
            cerr << "[JSON Parse Error - " << kind << "]: " << e.what() << endl;
        }
        
        curl_easy_cleanup(curl);
    }
    
    return instrument_map;
}