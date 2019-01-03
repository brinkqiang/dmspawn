#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/system/error_code.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/hex.hpp>

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

	void go();

private:

};

void echo(shared_ptr<session_t> session, yield_context_t& yield);

template<typename Range>
std::string to_hex(Range range)
{
	using namespace std;
	string hex;
	boost::algorithm::hex(begin(range), end(range), back_inserter(hex));
	return std::move(hex);
}

template<typename Range>
std::string to_hex(Range range, std::size_t size)
{
	using namespace std;
	string hex;
	boost::algorithm::hex(begin(range), begin(range) + size, back_inserter(hex));
	return std::move(hex);
}
