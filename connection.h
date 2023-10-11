#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <cstdint>
#include <memory>
#include <iostream>

#include "async.h"

namespace ba = boost::asio;
using tcp = ba::ip::tcp;
using error_code = boost::system::error_code;

class Connector
{
tcp::socket sock;
tcp::acceptor acceptor;
std::vector<tcp::socket> sockets;
async::handle_t async_bulk_handle;

public:
Connector(ba::io_service& io_service, tcp::endpoint ep)
    : sock(io_service),
      acceptor(io_service, ep),
      // According to task we have to mix inputs -> have only 1 session
      async_bulk_handle(async::connect(async::get_default_bulk_size()))
{
    do_accept();
}

~Connector()
{
    async::disconnect(async_bulk_handle);
}

private:
void do_accept();

};

class Session : public std::enable_shared_from_this<Session>
{
    tcp::socket s;
    std::array<char, 4096> readbuf;
    async::handle_t async_bulk_handle;
 
public:
    Session(tcp::socket socket) : s(std::move(socket)) {}

    void start(async::handle_t handle)
    {
        async_bulk_handle = handle;
        do_read();
    }

    void do_read();
};

