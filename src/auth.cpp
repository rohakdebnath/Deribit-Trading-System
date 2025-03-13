#include "utils.hpp"
#include "auth.hpp"
#include <iostream>
#include <sstream>

size_t expires_in = 0;

array<string, 2> gettoken(const string client_id, const string client_secret) {
    array<string, 2> ret = {"", ""};
    string url = "https://test.deribit.com/api/v2/public/auth?grant_type=client_credentials&client_id=" + client_id + "&client_secret=" + client_secret;
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL\n";
        return ret;
    }
    string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "CURL error: " << curl_easy_strerror(res) << '\n';
        curl_easy_cleanup(curl);
        return ret;
    }
    curl_easy_cleanup(curl);

    Json::CharReaderBuilder reader;
    Json::Value jsonData;
    string errs;

    istringstream ss(response);
    if (!Json::parseFromStream(reader, ss, &jsonData, &errs)) {
        cerr << "Failed to parse JSON: " << errs << '\n';
        return ret;
    }

    if (jsonData.isMember("result")) {
        ret[0] = jsonData["result"]["access_token"].asString();
        ret[1] = jsonData["result"]["refresh_token"].asString();
        expires_in = jsonData["result"]["expires_in"].asInt();
    } else {
        cerr << "Failed to refresh token (missing result).\n";
    }

    return ret;
}
