#include <iostream>
#include <boost/asio.hpp>
#include <set>
#include <thread>
#include <boost/asio/io_service.hpp>

namespace ba = boost::asio;


using sock_t = ba::ip::tcp::socket;
using ep_t = ba::ip::tcp::endpoint;
using acceptor_t = ba::ip::tcp::acceptor;

#if 1
class potok
{
	std::thread t;
	uint8_t buf[1024];
	ba::ip::tcp::socket s;

	potok(ba::io_service & ios, sock_t && s) : s(s)
	{
		std::cout << "v.capacity=" << v.capacity() << std::endl;
		std::cout << "v.size=" << v.size() << std::endl;
		ba::async_read(socket_,	ba::buffer(buf, sizeof(buf), do_read_again);
		do_read_again();
		t = std::thread(ios.run, &ios);
	}

	void do_read_again(const boost::system::error_code & ec, std::size_t bytes_transferred)
	{
		if (!ec) {
			std::cout << "async_read error: " << ec << std::endl;
			return;
		}
		std::cout << "read: " << bytes_transferred << std::endl;

		ba::async_read(sock, ba::buffer(buf, sizeof(buf)), do_read_again);
	}
};
#endif

int main(int argc, char ** argv)
{
	(void) argc, (void) argv;

	ba::io_service ioservice;

	ep_t ep(ba::ip::tcp::v4(), 9999);
	acceptor_t acc(ioservice, ep);

	std::set<potok> sp;
	while(1) {
		sock_t sock(ioservice);
		acc.accept(sock);
		std::cout << "accept" << std::endl;
		sp.emplace{ioservice.run, std::move(sock)};
	}

	return 0;
}

