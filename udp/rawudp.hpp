#ifndef RAWUDP_HPP
#define RAWUDP_HPP

#include <boost/asio.hpp>

extern "C" {
    #include <netinet/ip.h>
    #include <netinet/udp.h>
}

class rawudp
{
public:
    rawudp() {
	proto = 0;
    }
    int type() const
    {
	return BOOST_ASIO_OS_DEF(SOCK_RAW);
    }
    int protocol() const
    {
	return IPPROTO_UDP;
    }
    int family() const
    {
	return PF_INET;
    }
    
    typedef boost::asio::ip::basic_endpoint<rawudp> endpoint;
    typedef boost::asio::ip::basic_resolver<rawupd> resolver;
    typedef boost::asio::basic_raw_socket<rawudp> socket;
};

#endif
