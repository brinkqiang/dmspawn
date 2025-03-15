﻿
#include "asio-spawn-echo-client.h"
#include "dmlog.h"

session_t::session_t(io_context_t& io_context)
    : socket(io_context)
{
}

session_t::~session_t()
{
    LOG_INFO("log");
}

void session_t::go()
{
    auto self(shared_from_this());
    asio::spawn(socket.get_executor(),
        [this, self](yield_context_t yield)
    {
        string message = "test@123456789";
        while (true)
        {
            LOG_INFO("{0}", message);
            error_code_t ec;
            auto size = socket.async_write_some(asio::buffer(message), yield[ec]);

            if (ec)
            {
                LOG_INFO("Write error: {0}", ec.message());
                break;
            }
            
            LOG_INFO("async_write_some:{0}", size);

            vector<char> buff(1024, '\0');
            bool read_complete = false;

            while (!read_complete)
            {
                auto read_size = socket.async_read_some(asio::buffer(buff), yield[ec]);
                if (ec)
                {
                    if (ec == asio::error::eof || ec == asio::error::connection_reset)
                    {
                        LOG_INFO("Connection closed by server");
                    }
                    else
                    {
                        LOG_ERROR("Read error: {0}", ec.message());
                    }
                    read_complete = true;
                }
                else
                {
                    copy_n(buff.begin(), read_size, back_inserter(buffer));
                    if (read_size < buff.size())
                    {
                        read_complete = true;
                    }
                }
            }

            if (!buffer.empty())
            {
                buffer.push_back('\0');
                LOG_INFO("async_read_some:{0}", buffer.data());
                buffer.clear();
            }

            // Add delay between iterations
            asio::steady_timer timer(socket.get_executor());
            timer.expires_after(std::chrono::milliseconds(500));
            timer.async_wait(yield[ec]);
        }
    });
}

int main()
{
    io_context_t io_context;

    asio::spawn(io_context,
        [&io_context](yield_context_t yield)
    {
        error_code_t ec;

        auto session = make_shared<session_t>(io_context);
        auto& socket = session->socket;

        while (true)
        {
            socket.async_connect(endpoint_t(address_t::from_string("127.0.0.1"), 12500), yield[ec]);
            if (ec)
            {
                LOG_ERROR("Connection error: {0}", ec.message());
                
                // Wait before retrying
                asio::steady_timer timer(socket.get_executor());
                timer.expires_after(std::chrono::seconds(5));
                timer.async_wait(yield[ec]);
            }
            else
            {
                LOG_INFO("Connected to server");
                session->go();
                break;
            }
        }
    });

    io_context.run();
}
