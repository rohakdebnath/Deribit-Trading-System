#ifndef WEBSOCKET_CLIENT_HPP
#define WEBSOCKET_CLIENT_HPP

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <string>
#include <thread>
#include <atomic>

using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using connection_hdl = websocketpp::connection_hdl;

extern client ws_client;
extern connection_hdl global_hdl;
extern std::thread ws_thread;
extern std::atomic<bool> websocket_ready;

void start_websocket_connection();
void send_via_event_loop(const std::string& payload);

void subscribe_public_channel(const std::string& symbol);
void subscribe_private_channel(const std::string& symbol, const std::string& access_token);
void unsubscribe_public_channel(const std::string& symbol);
void unsubscribe_private_channel(const std::string& symbol, const std::string& access_token);

void send_ws_buy_order(const std::string& access_token, const std::string& instrument, int amount, const std::string& type, double price, const std::string& label);
void send_ws_sell_order(const std::string& access_token, const std::string& instrument, int amount, const std::string& type, double price, const std::string& label, const std::string& trigger, double trigger_price);
void send_ws_cancel_order(const std::string& access_token, const std::string& order_id);
void send_ws_modify_order(const std::string& access_token, const std::string& order_id, int new_amount, double new_price);
void send_ws_change_stop_limit(const std::string& access_token, const std::string& order_id, const std::string& instrument, int new_amount, double new_price, double new_trigger_price, const std::string& label, const std::string& trigger);
void stop_websocket_connection();

#endif // WEBSOCKET_CLIENT_HPP
