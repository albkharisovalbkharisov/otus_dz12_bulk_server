#include <iostream>
#include <boost/asio.hpp>
//#include <boost/asio/io_service.hpp>

namespace ba = boost::asio;


void do_read_again(const boost::system::error_code & ec, std::size_t bytes_transferred = 0)
{
	if (!ec) {
		std::cout << "async_read error: " << ec << std::endl;
		return;
	}
	std::

	ba::async_read(socket_,	ba::buffer(buf, sizeof(buf), do_read_again);
}

void read_handler(const boost::system::error_code & ec,
		  std::size_t bytes_transferred)
{
	std::cout << buf[0];
	do_read_again();
}

class connection_server
{
	const ba::ip::tcp::socket &socket_;

public:
	connection_server(const ba::ip::tcp::socket &socket) : socket_(socket)
	{
	}


};


void connect_handler(const boost::system::error_code & ec,
		     const ba::tcp::endpoint & ep)
{
	uint8_t buf[256];
	if (ec) {
		ba::async_read(socket_,	ba::buffer(buf, sizeof(buf), do_read_again);
	} else {
		std::cout << "error occured, connect_handler" << std::endl;
	}
}

int main(int argc, char ** argv)
{
	(void) argc, (void) argv;

	ba::io_service ioservice;

	ba::ip::tcp::endpoint ep(ba::ip::tcp::v4(), 9999);
	ba::ip::tcp::socket sock(ioservice);

	ba::async_connect(sock, ep, connect_handler);

	str::thread t([&io_service](){ io_service.run(); });

	return 0;
}

