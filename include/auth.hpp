#pragma once

#include <string>
#include <array>
#include <cstddef>
#include <curl/curl.h>
#include <json/json.h>

using namespace std;

extern size_t expires_in;

array<string, 2> gettoken(const string client_id, const string client_secret);
