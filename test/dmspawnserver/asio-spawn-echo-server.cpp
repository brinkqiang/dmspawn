
#include "asio-spawn-echo-server.h"
#include "dmlog.h"

session_t::session_t(io_context_t& io_context)
    : socket(io_context)
{
}

session_t::~session_t()
{
    LOG_INFO("Session destroyed");
}

void echo(shared_ptr<session_t> session, yield_context_t& yield)
{
    auto& socket = session->socket;
    error_code_t ec;
    vector<char> buffer(1024, '\0');
    auto size = socket.async_read_some(asio::buffer(buffer), yield[ec]);
    if (ec)
    {
        LOG_INFO("Read error: {0}", ec.message());
        LOG_INFO("Connection closed");
        return;
    }
    else
    {
        LOG_INFO("Received {0} bytes from client", size);

        LOG_INFO("Hex data: {0}", fmt::format("[{:02x}]", std::string(buffer, size)));
        LOG_INFO("Text data: {0}", buffer.data());
        auto write_size = socket.async_write_some(asio::buffer(buffer.data(), size), yield[ec]);
        if (ec)
        {
            LOG_INFO("Write error: {0}", ec.message());
            LOG_INFO("Connection closed");
            return;
        }
        else
        {
            LOG_INFO("Sent {0} bytes to client", write_size);
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
            auto& socket = session->socket;
            acceptor.async_accept(socket, yield[ec]);
            if (ec)
            {
                LOG_INFO("{0}", ec.message());
                continue;
            }
            else
            {
                error_code_t ec;
                auto remote_endpoint = socket.remote_endpoint(ec);
                if (ec) {
                    LOG_INFO("Failed to get client endpoint: {0}", ec.message());
                    socket.close(ec);
                    continue;
                }
                auto address = remote_endpoint.address().to_string();
                auto port = remote_endpoint.port();
                LOG_INFO("New connection from {0}:{1}", address, (int)port);
                LOG_INFO("Connection established");

                session->go();
            }
        }
    });

    io_context.run();
    return 0;
}
