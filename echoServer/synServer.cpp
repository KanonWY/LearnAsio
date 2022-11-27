#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <boost/thread.hpp>

size_t read_complete(char *buf, const boost::system::error_code& ec, size_t bytes)
{
    if (ec)
    {
        std::cout << "read_complete error" << std::endl;
        return 0;
    }
    bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
    return found ? 0 : 1;
}


void handle_connections(
    std::shared_ptr<boost::asio::io_service> service,
    std::shared_ptr<boost::asio::ip::tcp::acceptor> ac) 
{
    try
    {
        char buf[512] = {0};
        boost::asio::ip::tcp::socket sock(*service);
        ac->accept(sock);

        while (true)
        {
            int bytes = read(sock, boost::asio::buffer(buf),
                std::bind(
                    read_complete,
                    buf,
                    std::placeholders::_1,
                    std::placeholders::_2
                )
            );
            std::string msg(buf, bytes);
            std::cout << "msg = " << msg << std::endl;
            sock.write_some(boost::asio::buffer(msg));
        }
    }
    catch (std::exception& ec)
    {
        std::cout << "Message = " << ec.what() << std::endl;
        return;
    }
}

int main()
{
    auto service_ptr = std::make_shared<boost::asio::io_service>();
    auto acceptor_ptr = std::make_shared<boost::asio::ip::tcp::acceptor>(
        *service_ptr,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 4444)
    );

    while(true)
    {
        std::cout << "Enter Loop" << std::endl;
        handle_connections(service_ptr, acceptor_ptr);
    }
    service_ptr->stop();
    return 0;
}