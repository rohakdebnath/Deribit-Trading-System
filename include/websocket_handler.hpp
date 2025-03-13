#ifndef WEBSOCKET_HANDLER_HPP
#define WEBSOCKET_HANDLER_HPP

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <string>

using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using connection_hdl = websocketpp::connection_hdl;

void on_message(connection_hdl hdl, client::message_ptr msg);

// Optionally, if you keep logging here temporarily:
void log_to_file(const std::string& message);
void orderbook_store(const std::string& message);

#endif // WEBSOCKET_HANDLER_HPP
