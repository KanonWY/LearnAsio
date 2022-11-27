#include <iostream>
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>



int main()
{
    boost::asio::io_context io_service;
    using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
    ctx.set_default_verify_paths();

    ssl_socket  sock(io_service,ctx);

    boost::asio::ip::tcp::resolver resolver(io_service);
    std::string host = "www.yahoo.com";

    boost::asio::ip::tcp::resolver::query query(host, "https");
    boost::asio::connect(sock.lowest_layer(), resolver.resolve(query));

    sock.set_verify_mode(boost::asio::ssl::verify_none);

    sock.set_verify_callback(boost::asio::ssl::rfc2818_verification(host));

    sock.handshake(ssl_socket::client);

    std::string req = "GET inedex.html HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: keep-alive\r\n\r\n";

    write(sock, boost::asio::buffer(req.c_str(),req.length()));

    char buff[512] = {};

    boost::system::error_code ec;
    while(!ec)
    {
        int bytes = boost::asio::read(sock, boost::asio::buffer(buff), ec);
        std::cout << "Content = " << std::endl;
        std::cout << std::string(buff, bytes);
    } 
    return 0;
}