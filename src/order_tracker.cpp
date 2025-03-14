#include "order_tracker.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include "global_vars.hpp"

using namespace std;

extern vector<json> order_log;
extern map<string, json> open_orders;

string get_timestamp() {
    auto now = chrono::system_clock::now();
    time_t time_now = chrono::system_clock::to_time_t(now);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time_now));
    return string(buffer);
}

void log_order_event(const string& order_id, const string& event_type, const json& order_data) {
    json log_entry = {
        {"timestamp", get_timestamp()},
        {"order_id", order_id},
        {"event_type", event_type},
        {"details", order_data}
    };
    order_log.push_back(log_entry);
    // cout << "[Order Event Logged] " << order_id << endl;
}

void add_open_order(const string& order_id, const json& order_data) {
    open_orders[order_id] = order_data;
    log_order_event(order_id, "order_opened", order_data);
}

void update_open_order(const string& order_id, const json& updated_data) {
    if (open_orders.count(order_id)) {
        open_orders[order_id] = updated_data;
        log_order_event(order_id, "order_updated", updated_data);
    }
}

void close_order(const string& order_id, const json& final_data) {
    if (open_orders.count(order_id)) {
        open_orders.erase(order_id);
        log_order_event(order_id, "order_closed", final_data);
    }
}
