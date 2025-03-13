#include <iostream>
#include <string>
#include "auth.hpp"
#include "rest_api.hpp"
#include "websocket_client.hpp"
#include "order_tracker.hpp"
#include "global_vars.hpp"

using namespace std;

string access_token;
vector<json> order_log;
map<string, json> open_orders;
thread ws_thread;
connection_hdl global_hdl;
ws_client_t ws_client;
atomic<bool> websocket_ready(false);
atomic<bool> processing_last_message(false);


int main() {
    string client_id = "ksZ7z6qT";
    string client_secret = "WScAz-vem8DII2VD8DpAQbEc9TdmE2SYLAf0mIsB-DM";

    array<string, 2> tokens = gettoken(client_id, client_secret);
    access_token = tokens[0];
    // map<string, vector<string>> instruments = fetch_all_instruments();
    
    // cout << "\n[Available Instruments]:\n";
    // for (const auto& [kind, list] : instruments) {
    //     cout << "\n>> " << kind << " instruments:" << endl;
    //     for (const auto& name : list) {
    //         cout << "   - " << name << endl;
    //     }
    // }

    placebuyorder(access_token, "ETH-PERPETUAL", 40, "market", 0, "WS");
    
    start_websocket_connection();
    while (!websocket_ready.load()) {
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    
    send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "WS");

    // cout << "[Main] Ready to send orders.\n";

    // string command;
    // while (true) {
    //     cout << "\nCommands: buy, sell, modify, cancel, view_orders, view_log, subscribe_public, subscribe_private, exit, unsubscribe_public, unsubscribe_private\n";
    //     cout << "Enter command: ";
    //     cin >> command;

    //     if (command == "buy") {
    //         string instrument, type, label;
    //         int amount;
    //         double price;
    //         cout << "Instrument: "; cin >> instrument;
    //         cout << "Amount: "; cin >> amount;
    //         cout << "Order Type (limit/market): "; cin >> type;
    //         if (type == "limit") {
    //             cout << "Price: "; cin >> price;
    //         } else {
    //             price = 0;
    //         }
    //         cout << "Label: "; cin >> label;
    //         send_ws_buy_order(access_token, instrument, amount, type, price, label);
    //     }
    //     else if (command == "sell") {
    //         string instrument, type, label, trigger;
    //         int amount;
    //         double price, trigger_price;
    //         cout << "Instrument: "; cin >> instrument;
    //         cout << "Amount: "; cin >> amount;
    //         cout << "Order Type (limit/market/stop_limit): "; cin >> type;
    //         if (type == "limit" || type == "stop_limit") {
    //             cout << "Price: "; cin >> price;
    //         } else {
    //             price = 0;
    //         }
    //         if (type == "stop_limit") {
    //             cout << "Trigger Type (index_price/mark_price/last_price): "; cin >> trigger;
    //             cout << "Trigger Price: "; cin >> trigger_price;
    //         } else {
    //             trigger = ""; trigger_price = 0;
    //         }
    //         cout << "Label: "; cin >> label;
    //         send_ws_sell_order(access_token, instrument, amount, type, price, label, trigger, trigger_price);
    //     }
    //     else if (command == "modify") {
    //         string order_id;
    //         int new_amount;
    //         double new_price;
    //         cout << "Order ID: "; cin >> order_id;
    //         cout << "New Amount: "; cin >> new_amount;
    //         cout << "New Price: "; cin >> new_price;
    //         send_ws_modify_order(access_token, order_id, new_amount, new_price);
    //     }
    //     else if (command == "cancel") {
    //         string order_id;
    //         cout << "Order ID: "; cin >> order_id;
    //         send_ws_cancel_order(access_token, order_id);
    //     }
    //     else if (command == "view_orders") {
    //         cout << "\n[Open Orders]:\n";
    //         for (const auto& [id, order] : open_orders) {
    //             cout << order.dump(2) << endl;
    //         }
    //     }
    //     else if (command == "view_log") {
    //         cout << "\n[Order Log]:\n";
    //         for (const auto& log : order_log) {
    //             cout << log.dump(2) << endl;
    //         }
    //     }
    //     else if (command == "subscribe_public") {
    //         string symbol;
    //         cout << "Symbol to subscribe (e.g., BTC-PERPETUAL): ";
    //         cin >> symbol;
    //         subscribe_public_channel(symbol);
    //     }
    //     else if (command == "subscribe_private") {
    //         string symbol;
    //         cout << "Symbol to subscribe (e.g., BTC-PERPETUAL): ";
    //         cin >> symbol;
    //         subscribe_private_channel(symbol, access_token);
    //     }
    //     else if (command == "unsubscribe_public") {
    //         string symbol;
    //         cout << "Symbol to unsubscribe (e.g., BTC-PERPETUAL): ";
    //         cin >> symbol;
    //         unsubscribe_public_channel(symbol);
    //     }
    //     else if (command == "unsubscribe_private") {
    //         string symbol;
    //         cout << "Symbol to unsubscribe (e.g., BTC-PERPETUAL): ";
    //         cin >> symbol;
    //         unsubscribe_private_channel(symbol, access_token);
    //     }
    //     else if (command == "exit") {
    //         cout << "Exiting application...\n";
    //         stop_websocket_connection();
    //         break;
    //     }
    //     else {
    //         cout << "Unknown command.\n";
    //     }
    // }
    this_thread::sleep_for(chrono::milliseconds(3000));
    stop_websocket_connection();
    this_thread::sleep_for(chrono::milliseconds(3000));
    return 0;
}
