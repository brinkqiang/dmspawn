//#include "pch.h"
#include "asio-spawn-echo-server.h"
#include "dmlog.hpp"
session_t::session_t(io_context_t &io_context)
	: socket(io_context)
{
}

session_t::~session_t()
{
	LOG_INFO("log");
}

void echo(shared_ptr<session_t> session, yield_context_t& yield)
{
	auto &socket = session->socket;
	error_code_t ec;
	vector<char> buffer(1024, '\0');
	auto size = socket.async_read_some(asio::buffer(buffer), yield[ec]);
	if (ec)
	{
		LOG_INFO("{0}", ec.message());
		return;
	}
	else
	{
		LOG_INFO("async_read_some:{0}", size);
		LOG_INFO("hex:{0}", to_hex(buffer, size));
		LOG_INFO("{0}", buffer.data());
		auto write_size = socket.async_write_some(asio::buffer(buffer.data(), size), yield[ec]);
		if (ec)
		{
			LOG_INFO("{0}", ec.message());
			return;
		}
		else
		{
			LOG_INFO("async_read_some:{0}", write_size);
			echo(session, yield);
		}
	}
}

void session_t::go()
{
	auto self(shared_from_this());
	asio::spawn(socket.get_executor(),
		[this, self](yield_context_t yield)
	{
		echo(self, yield);
	});
}

int main()
{
	io_context_t io_context;

	acceptor_t acceptor(io_context,
		endpoint_t(address_t::from_string("0.0.0.0"), 12500));

	asio::spawn(io_context,
		[&acceptor, &io_context](yield_context_t yield)
	{
		while (true)
		{
			error_code_t ec;
			auto session = make_shared<session_t>(io_context);
			auto &socket = session->socket;
			acceptor.async_accept(socket, yield[ec]);
			if (ec)
			{
				LOG_INFO("{0}", ec.message());
				continue;
			}
			else
			{
				auto remote_endpoint = socket.remote_endpoint();
				auto address = remote_endpoint.address().to_string();
				auto port = remote_endpoint.port();
				LOG_INFO("address:{0} port:{1}", address, port);

				session->go();
			}
		}
	});

	io_context.run();
	return 0;
}
