//#include "pch.h"
#include "asio-spawn-echo-server.h"

session_t::session_t(io_context_t &io_context)
	: socket(io_context)
{
}

session_t::~session_t()
{
	INFO("log");
}

void echo(shared_ptr<session_t> session, yield_context_t& yield)
{
	auto &socket = session->socket;
	error_code_t ec;
	vector<char> buffer(1024, '\0');
	auto size = socket.async_read_some(asio::buffer(buffer), yield[ec]);
	if (ec)
	{
		INFO("log", "{0}", ec.message());
		return;
	}
	else
	{
		INFO("log", "async_read_some:{0}", size);
		INFO("log", "hex:{0}", to_hex(buffer, size));
		INFO("log", buffer.data());
		auto write_size = socket.async_write_some(asio::buffer(buffer.data(), size), yield[ec]);
		if (ec)
		{
			INFO("log", "{0}", ec.message());
			return;
		}
		else
		{
			INFO("log", "async_read_some:{0}", write_size);
			echo(session, yield);
		}
	}
}

void session_t::go()
{
	auto self(shared_from_this());
	asio::spawn(socket.get_io_context(),
		[this, self](yield_context_t yield)
	{
		echo(self, yield);
	});
}

int main()
{
	auto logger = spdlog::stdout_color_mt("log");
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
				INFO("log", "{0}", ec.message());
				continue;
			}
			else
			{
				auto remote_endpoint = socket.remote_endpoint();
				auto address = remote_endpoint.address().to_string();
				auto port = remote_endpoint.port();
				INFO("log", "address:{0} port:{1}", address, port);

				session->go();
			}
		}
	});

	io_context.run();
	return 0;
}
