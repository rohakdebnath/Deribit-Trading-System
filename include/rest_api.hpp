#pragma once
#include <string>
#include <map>
#include <vector>

using namespace std;

void placebuyorder(const string& access_token, const string& instrument_name, int amount, const string& order_type, double price = 0.0, const string& label = "cpp_test_order");
void placesellorder(const string& access_token, const string& instrument_name, int amount, const string& order_type, double price = 0.0, const string& label = "cpp_sell_order", const string& trigger = "", double trigger_price = 0.0);
void cancelorder(const string& access_token, const string& order_id);
void modifyorder(const string& access_token, const string& order_id, int new_amount, double new_price);
void modifystoplimit(const string& access_token, const string& order_id, const string& instrument_name, int new_amount, double new_limit_price, double new_trigger_price, const string& new_label = "cpp_replaced_stop", const string& trigger = "last_price");
void getorderbook(const string& instrument_name);
void accountsummary(const string& access_token);
map<string, vector<string>> fetch_all_instruments();