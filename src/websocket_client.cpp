#include "websocket_client.hpp"
#include "websocket_handler.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio/ssl/context.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "global_vars.hpp"
#include "latency_tracker.hpp"

using json = nlohmann::json;
using namespace std;

void send_via_event_loop(const std::string& payload) {
    ws_client.get_io_service().post([payload]() {
        websocketpp::lib::error_code ec;
        ws_client.send(global_hdl, payload, websocketpp::frame::opcode::text, ec);
        if (ec) {
            string error_msg = "[Send Error]: " + ec.message();
            cerr << error_msg << endl;
            log_to_file(error_msg);
        }
    });
}

void on_open(connection_hdl hdl) {
    cout << "[Connected to Websocket]\n";
    global_hdl = hdl;
    websocket_ready.store(true);
    json heartbeat = { {"method", "public/set_heartbeat"}, {"params", { {"interval", 60} }}, {"id", 171} };
    send_via_event_loop(heartbeat.dump());
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "1");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "2");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "3");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "4");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "5");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "6");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "7");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "8");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "9");
    // send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, "10");
    for (int i = 0; i < 10; ++i) {
        string label = "WS_" + to_string(i);
        send_ws_buy_order(access_token, "ETH-PERPETUAL", 40, "market", 0, label);
    }
    
}

void start_websocket_connection() {
    ws_client.init_asio();
    ws_client.clear_access_channels(websocketpp::log::alevel::all);
    ws_client.clear_error_channels(websocketpp::log::elevel::all);
    ws_client.set_message_handler(&on_message);
    ws_client.set_open_handler(&on_open);
    ws_client.set_tls_init_handler([](connection_hdl) -> shared_ptr<websocketpp::lib::asio::ssl::context> {
        auto ctx = make_shared<websocketpp::lib::asio::ssl::context>(websocketpp::lib::asio::ssl::context::tlsv12_client);
        ctx->set_options(websocketpp::lib::asio::ssl::context::default_workarounds |
                         websocketpp::lib::asio::ssl::context::no_sslv2 |
                         websocketpp::lib::asio::ssl::context::no_sslv3 |
                         websocketpp::lib::asio::ssl::context::single_dh_use);
        return ctx;
    });
    websocketpp::lib::error_code ec;
    client::connection_ptr con = ws_client.get_connection("wss://test.deribit.com/ws/api/v2", ec);
    if (ec) {
        cerr << "WebSocket connection error: " << ec.message() << endl;
        return;
    }
    ws_client.connect(con);
    // ws_thread = thread([] {
    //     try {
    //         ws_client.run();
    //     } catch (const exception& e) {
    //         log_to_file(string("[WebSocket Run Error]: ") + e.what());
    //     }
    // });
    // ws_thread.detach();
    
    ws_client.run();
}

void subscribe_public_channel(const string& symbol) {
    json msg = {
        {"method", "public/subscribe"},
        {"params", { {"channels", {"book." + symbol + ".100ms"} } }},
        {"id", 42}
    };
    send_via_event_loop(msg.dump());
}

void subscribe_private_channel(const string& symbol, const string& access_token) {
    json msg = {
        {"method", "private/subscribe"},
        {"params", {
            {"channels", {"user.orders." + symbol + ".raw"}},
            {"access_token", access_token}
        }},
        {"id", 43}
    };
    send_via_event_loop(msg.dump());
}

void unsubscribe_public_channel(const string& symbol) {
    json msg = {
        {"method", "public/unsubscribe"},
        {"params", { {"channels", {"book." + symbol + ".100ms"} } }},
        {"id", 52}
    };
    send_via_event_loop(msg.dump());
}

void unsubscribe_private_channel(const string& symbol, const string& access_token) {
    json msg = {
        {"method", "private/unsubscribe"},
        {"params", {
            {"channels", {"user.orders." + symbol + ".raw"}},
            {"access_token", access_token}
        }},
        {"id", 53}
    };
    send_via_event_loop(msg.dump());
}

void send_ws_buy_order(const string& access_token, const string& instrument, int amount, const string& type, double price, const string& label) {
    json msg = {
        {"method", "private/buy"},
        {"params", {
            {"instrument_name", instrument},
            {"amount", amount},
            {"type", type},
            {"access_token", access_token},
            {"label", label}
        }},
        {"id", 101}
    };
    if (type == "limit") msg["params"]["price"] = price;
    string payload = msg.dump();
    latency_tracker.start(label); 
    ws_client.get_io_service().post([payload, label]() {
        websocketpp::lib::error_code ec;
        ws_client.send(global_hdl, payload, websocketpp::frame::opcode::text, ec);
        if (ec) {
            string error_msg = "[Send Error]: " + ec.message();
            cerr << error_msg << endl;
            log_to_file(error_msg);
        }
    });
    // send_via_event_loop(msg.dump());
}

void send_ws_sell_order(const string& access_token, const string& instrument, int amount, const string& type, double price, const string& label, const string& trigger, double trigger_price) {
    json msg = {
        {"method", "private/sell"},
        {"params", {
            {"instrument_name", instrument},
            {"amount", amount},
            {"type", type},
            {"access_token", access_token},
            {"label", label}
        }},
        {"id", 102}
    };
    if (type == "limit" || type == "stop_limit") msg["params"]["price"] = price;
    if (!trigger.empty()) {
        msg["params"]["trigger"] = trigger;
        msg["params"]["trigger_price"] = trigger_price;
    }
    send_via_event_loop(msg.dump());
}

void send_ws_cancel_order(const string& access_token, const string& order_id) {
    json msg = {
        {"method", "private/cancel"},
        {"params", {
            {"order_id", order_id},
            {"access_token", access_token}
        }},
        {"id", 103}
    };
    send_via_event_loop(msg.dump());
}

void send_ws_modify_order(const string& access_token, const string& order_id, int new_amount, double new_price) {
    json msg = {
        {"method", "private/edit"},
        {"params", {
            {"order_id", order_id},
            {"amount", new_amount},
            {"price", new_price},
            {"access_token", access_token}
        }},
        {"id", 104}
    };
    send_via_event_loop(msg.dump());
}

void send_ws_change_stop_limit(const string& access_token, const string& order_id, const string& instrument, int new_amount, double new_price, double new_trigger_price, const string& label, const string& trigger) {
    send_ws_cancel_order(access_token, order_id);
    send_ws_sell_order(access_token, instrument, new_amount, "stop_limit", new_price, label, trigger, new_trigger_price);
}

void stop_websocket_connection() {
    websocketpp::lib::error_code ec;
    ws_client.close(global_hdl, websocketpp::close::status::normal, "Client requested close", ec);
    if (ec) cerr << "[WebSocket Close Error]: " << ec.message() << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
