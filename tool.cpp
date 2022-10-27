#include <cstdlib>
#include <chrono>
#include <iostream>
#include <string>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

namespace posix = boost::asio::posix;

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_context io_context;
    boost::minstd_rand rng;
    boost::random::uniform_int_distribution<> coin(0,1);
    posix::stream_descriptor output(io_context, ::dup(STDOUT_FILENO));

    if (argc == 3 && strcmp(argv[1], "generate") == 0) {
      int bits = atoi(argv[2]);
      for (int i = 0; i < bits; i++)
      {
        boost::array<char, 1> buf;
        boost::system::error_code error;
        buf[0] = coin(rng) ? '1' : '0';
        boost::asio::write(output, boost::asio::buffer(buf), error);
      }
    } else if (argc == 4 && strcmp(argv[1], "compare") == 0) {
      posix::stream_descriptor in1(io_context), in2(io_context);
      int fin1 = open(argv[2], O_RDONLY);
      if (fin1 == -1)
        throw std::runtime_error("failed to open in1");
      in1.assign(fin1);
      int fin2 = open(argv[3], O_RDONLY);
      if (fin2 == -1)
        throw std::runtime_error("failed to open in2");
      in2.assign(fin2);

      bool eof1 = false, eof2 = false;
      int size_bit = 0;
      int error_bit = 0;
      for (;;)
      {
        boost::array<char, 1> buf1, buf2;
        boost::system::error_code error;
        size_t len1, len2;
        do
        {
          len1 = in1.read_some(boost::asio::buffer(buf1), error);
          if (error == boost::asio::error::eof) {
            eof1 = true;
            break;
          }
          else if (error)
            throw boost::system::system_error(error);
        } while (len1 != 1);
        do
        {
          len2 = in2.read_some(boost::asio::buffer(buf2), error);
          if (error == boost::asio::error::eof) {
            eof2 = true;
            break;
          }
          else if (error)
            throw boost::system::system_error(error);
        } while (len2 != 1);
        if (eof1 && eof2) break;
        else if (eof1) error_bit++;
        else if (eof2) error_bit++;
        else if (buf1[0] != buf2[0]) error_bit++;
        size_bit++;
      }
      std::cout << "Size: " << size_bit << " bit\n";
      std::cout << "Errors: " << error_bit << " bit\n";
    } else {
      std::cerr << "Usage: tool (generate <bits>, compare <in1> <in2>)" << std::endl;
      return 1;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
