#pragma once
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

using std::string;
using std::vector;
using std::map;
using nlohmann::json;

extern vector<json> order_log;
extern map<string, json> open_orders;

string get_timestamp();
void log_order_event(const string& action, const string& status, const json& order_info);
void add_open_order(const string& order_id, const json& order_data);
void update_open_order(const string& order_id, const json& updated_data);
void close_order(const string& order_id, const json& final_data);