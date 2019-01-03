#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/system/error_code.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog_easy.hpp>

using namespace std;

namespace asio = boost::asio;
using io_context_t = asio::io_context;
using tcp_t = asio::ip::tcp;
using socket_t = tcp_t::socket;
using acceptor_t = tcp_t::acceptor;
using endpoint_t = tcp_t::endpoint;
using address_t = asio::ip::address;
using error_code_t = boost::system::error_code;
using resolver_t = tcp_t::resolver;
using socket_t = tcp_t::socket;
using yield_context_t = asio::yield_context;

class session_t : public std::enable_shared_from_this<session_t>
{
public:
	session_t(io_context_t &io_context);
	~session_t();

	socket_t socket;
	string address;
	uint16_t port;
	vector<char> buffer;

	void go();

private:

};
