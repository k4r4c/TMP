#include <chrono>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace posix = boost::asio::posix;

int cutoff = 0;
bool print = false;

long int get_nanos()
{
  using namespace std::chrono;
  auto now = high_resolution_clock::now();
  auto nanos = duration_cast<nanoseconds>(now.time_since_epoch()).count();
  return nanos;
}

int read_bit(tcp::socket &socket)
{
  for (;;) {
    boost::array<char, 1> buf;
    boost::system::error_code error;
    size_t len = socket.read_some(boost::asio::buffer(buf), error);
    if (error == boost::asio::error::eof)
      return -1; // Connection closed cleanly by peer.
    else if (error)
     throw boost::system::system_error(error); // Some other error.
    if (len == 1) {
      auto nanos = get_nanos();
      nanos /= cutoff; // cutoff
      if (print) {
        std::cout << nanos << "\n";
      }
      if (nanos % 2 == 0) {
        return 0;
      } else {
        return 1;
      }
    }
  }
}

int read_symbol(tcp::socket &socket)
{
  int b = read_bit(socket);
  if (b == 0) return '0';
  if (b == 1) return '1';
  return -1;
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 3)
    {
      std::cerr << "Usage: ./client <host> <cutoff> [-p]" << std::endl;
      return 1;
    }
    cutoff = atoi(argv[2]);
    if (argc == 4 && strcmp(argv[3], "-p") == 0) {
      print = true;
    }

    boost::asio::io_context io_context;

    posix::stream_descriptor output(io_context, ::dup(STDOUT_FILENO));

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(argv[1], "6666");

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    tcp::no_delay option(true);
    socket.set_option(option);

    for (;;)
    {
      boost::array<char, 1> buf;
      boost::system::error_code error;

      int b = read_symbol(socket);
      if (b == -1) break; // Connection closed cleanly by peer.
      if (!print) {
        buf[0] = (char) b;
        boost::asio::write(output, boost::asio::buffer(buf), error);
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

