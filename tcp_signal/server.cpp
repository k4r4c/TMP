#include <cstdlib>
#include <chrono>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace posix = boost::asio::posix;

bool print = false;
int cutoff = 0;

long int get_nanos()
{
  using namespace std::chrono;
  auto now = high_resolution_clock::now();
  auto nanos = duration_cast<nanoseconds>(now.time_since_epoch()).count();
  return nanos;
}

long int block_cutoff_mod2(int i)
{
  for (;;) {
    auto nanos = get_nanos();
    if (print)
      return nanos;
    nanos /= cutoff; // cutoff
    if (nanos % 2 == i) // mod2
      return nanos;
  }
}

void send_bit(tcp::socket &socket, int bit)
{
  auto nanos = block_cutoff_mod2(bit);
  boost::system::error_code error;
  boost::asio::write(socket, boost::asio::buffer(""), error);
  if (print) {
    std::cout << nanos << "\n";
  }
}

void send_symbol(tcp::socket &socket, char sym)
{
  if (sym == '0') {
    send_bit(socket, 0);
  } else if (sym == '1') {
    send_bit(socket, 1);
  }
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: ./server <cutoff> [-p]" << std::endl;
      return 1;
    }
    cutoff = atoi(argv[1]);
    if (argc == 3 && strcmp(argv[2], "-p") == 0) {
      print = true;
    }

    boost::asio::io_context io_context;

    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 6666));

    posix::stream_descriptor input(io_context, ::dup(STDIN_FILENO));

    tcp::socket socket(io_context);
    acceptor.accept(socket);
    tcp::no_delay option(true);
    socket.set_option(option);

    while (std::cin.good())
    {
      boost::array<char, 1> buf;
      boost::system::error_code error;
      size_t len = input.read_some(boost::asio::buffer(buf), error);
      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

      if (len == 1) {
        send_symbol(socket, buf[0]);
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

