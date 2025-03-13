#include "websocket_handler.hpp"
#include "websocket_client.hpp"
#include "order_tracker.hpp"
#include "global_vars.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
using json = nlohmann::json;

ofstream log_file("logs.txt", ios::app);
ofstream orderbook_stream("live_orderbook.txt", ios::app);

void log_to_file(const string& message) {
    log_file << message << endl;
}

void orderbook_store(const string& message) {
    orderbook_stream << message << endl;
}

void on_message(connection_hdl, client::message_ptr msg) {
    try {
        json parsed = json::parse(msg->get_payload());
        log_to_file("[WebSocket Message Received]:\n" + parsed.dump(2));

        if (parsed.contains("error")) {
            string error_msg = "[WebSocket Error]:\n" + parsed.dump(2);
            cerr << error_msg << endl;
            log_to_file(error_msg);
            return;
        }

        if (parsed.contains("method")) {
            const string method = parsed["method"];

            if (method == "heartbeat") {
                auto params = parsed.value("params", json::object());
                if (params.value("type", "") == "test_request") {
                    json reply = {{"method", "public/test"}, {"id", 0}};
                    send_via_event_loop(reply.dump());
                    log_to_file("[Responded to test_request]");
                } else {
                    log_to_file("[Heartbeat Received]");
                }
                return;
            }

            if (method == "subscription" && parsed["params"].contains("data")) {
                const string channel = parsed["params"].value("channel", "unknown_channel");
                const json& data = parsed["params"]["data"];
                log_to_file("[Subscription Update] Channel: " + channel);

                if (channel.find("book.") != string::npos) {
                    if (data.contains("asks")) {
                        orderbook_store("Top 10 Asks:\n");
                        for (size_t i = 0; i < min<size_t>(10, data["asks"].size()); ++i) {
                            orderbook_store(data["asks"][i].dump());
                        }
                    }
                    if (data.contains("bids")) {
                        orderbook_store("Top 10 Bids:\n");
                        for (size_t i = 0; i < min<size_t>(10, data["bids"].size()); ++i) {
                            orderbook_store(data["bids"][i].dump());
                        }
                    }
                }

                if (channel.find("user.orders.") != string::npos && data.contains("order_id")) {
                    const string order_id = data["order_id"];
                    const string state = data.value("order_state", "unknown");
                    log_order_event(order_id, state, data);

                    if (state == "open") add_open_order(order_id, data);
                    else if (state == "filled" || state == "cancelled" || state == "rejected" || state == "untriggered") close_order(order_id, data);
                    else update_open_order(order_id, data);
                }
                return;
            }
        }

        if (parsed.contains("result") && parsed["result"].contains("order")) {

            const json& order = parsed["result"]["order"];
            const string order_id = order.value("order_id", "unknown");
            const string state = order.value("order_state", "unknown");

            log_order_event("order_result", state, order);
            if (state == "open") add_open_order(order_id, order);
            else if (state == "filled" || state == "cancelled" || state == "rejected" || state == "untriggered") close_order(order_id, order);
            else {
                string msg = "[Unhandled Order State]: " + state;
                cerr << msg << endl;
                log_to_file(msg);
            }
            return;
        }

        if (parsed.contains("params") && parsed["params"].contains("data")) {
            const json& data = parsed["params"]["data"];
            if (data.contains("order_id")) {
                const string order_id = data["order_id"];
                const string method = parsed.value("method", "order_update");
                const string state = data.value("order_state", "unknown");

                log_order_event(method, state, data);
                if (state == "open") add_open_order(order_id, data);
                else if (state == "filled" || state == "cancelled" || state == "rejected" || state == "untriggered") close_order(order_id, data);
                else {
                    string msg = "[Unhandled Order State]: " + state;
                    cerr << msg << endl;
                    log_to_file(msg);
                }
                return;
            }
        }

        if (parsed.contains("id")) {
            const int id = parsed["id"];
            switch (id) {
                case 0: log_to_file("[Test Request Response Received]"); break;
                case 42: log_to_file("[Public Subscription Acknowledged]"); break;
                case 43: log_to_file("[Private Subscription Acknowledged]"); break;
                case 52: log_to_file("[Public Unsubscription Acknowledged]"); break;
                case 53: log_to_file("[Private Unsubscription Acknowledged]"); break;
                default: log_to_file(parsed.dump(2));
            }
        }
    } catch (const exception& e) {
        string error_msg = string("[WebSocket Message Parse Error]: ") + e.what();
        cerr << error_msg << endl;
        log_to_file(error_msg);
    }
}
