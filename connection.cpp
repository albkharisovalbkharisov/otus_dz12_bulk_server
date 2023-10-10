#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <array>
#include "async.h"
#include "connection.h"

void Connector::do_accept()
{
    acceptor.async_accept(sock,
            [this] (const error_code& ec) {
                if (!ec) {
                        std::make_shared<Session>(std::move(sock))->start();
                        do_accept();
                } else {
                    std::cerr << "Connector: error: Close() " << std::endl;
                }
            });
}

void Session::do_read() {
    auto self(shared_from_this());
    s.async_read_some(ba::buffer(readbuf),
            [this, self](const error_code& ec, std::size_t bytes_transferred) {
                if (!ec) {
                    async::receive(async_bulk_handle, readbuf.data(), bytes_transferred);
                    do_read();
                } else {
                    async::disconnect(async_bulk_handle);
                    s.close();
                }
            });
}

