
#include <boost/asio.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <iostream>
#include <memory>
#include <array>

#include "async.h"
#include "connection.h"

namespace ba = boost::asio;
using tcp = ba::ip::tcp;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage:\n\t./cli_server <port> <bulk_size>" << std::endl;
        exit(-1);
    }

    auto port_num = static_cast<ba::ip::port_type>(std::atoi(argv[1]));
    async::set_default_bulk_size(std::atoi(argv[2]));

    ba::io_service io_service;
    Connector con(io_service, tcp::endpoint{ba::ip::tcp::v4(), port_num});
    io_service.run();

    return 0;
}

