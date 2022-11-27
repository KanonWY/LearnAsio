#include <boost/asio.hpp>
#include <iostream>
#include <boost/array.hpp>




int main()
{
    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);

        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 4444);

        boost::asio::ip::tcp::socket sock(io_context);

        sock.connect(ep);

        for(;;)
        {
            boost::array<char, 128> buff{};
            boost::system::error_code error;

            size_t len = sock.read_some(boost::asio::buffer(buff), error);
            if (error == boost::asio::error::eof)
            {
                std::cout << "peer closed" << std::endl;
            }
            else if(error)
            {
                throw  boost::system::system_error(error);
            }
            std::cout.write(buff.data(), len);
        }
    }
    catch(std::exception& e)
    {
        std::cout << "err" << e.what() << std::endl;
    }
    return 0;
}