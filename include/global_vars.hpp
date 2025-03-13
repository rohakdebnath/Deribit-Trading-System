#ifndef GLOBAL_VARS_HPP
#define GLOBAL_VARS_HPP

#include <string>
#include <vector>
#include <map>
#include <future>
#include <thread>
#include <nlohmann/json.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <atomic>

using std::string;
using std::vector;
using std::map;
using std::promise;
using std::thread;
using json = nlohmann::json;
using websocketpp::connection_hdl;
typedef websocketpp::client<websocketpp::config::asio_tls_client> ws_client_t;

extern string access_token;
extern vector<json> order_log;
extern map<string, json> open_orders;
extern thread ws_thread;
extern connection_hdl global_hdl;
extern ws_client_t ws_client;
extern std::atomic<bool> websocket_ready;

#endif