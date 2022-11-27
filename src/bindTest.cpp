#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <functional>
#include <thread>


using namespace boost::asio;


int syncConnectTest()
{
    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 4444);
    boost::asio::ip::tcp::socket sock(service);

    try
    {
        sock.connect(ep);
        std::cout << "hello world" << std::endl;
    }
    catch (std::exception& ec)
    {
        std::cout << "Message :" << ec.what() << std::endl;
    }
    return 0;
}


void client_session(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{
    while (true)
    {
        char data[512];
        size_t len = sock->read_some(boost::asio::buffer(data));
        if (len > 0)
        {
            boost::asio::write(*sock, boost::asio::buffer("ok", 2));
        }
    }
}



int sync_server(){
    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), 2001);
    boost::asio::ip::tcp::acceptor acc(service, ep);
    
    while(true)
    {
        auto sock = std::make_shared<boost::asio::ip::tcp::socket>(service);
        try 
        {
            acc.accept(*sock);
            std::thread(std::bind(client_session, sock));
        }
        catch (std::exception& ec)
        {
            std::cout << "Message " << ec.what() << std::endl;
        }
    }
}


void connect_handler()
{
    std::cout << "client connect" << std::endl;
}

void asynclient()
{
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket sock(service);

    sock.async_connect(ep, std::bind(connect_handler));
    std::cout << "after async_connect" << std::endl;
    service.run();
}

using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;


void handle_accept(socket_ptr sock, const boost::system::error_code& ec)
{
    if(ec)
    {
        std::cout << "Message: " << ec << std::endl; 
    }
    else
    {
        std::cout << "connect ok" << std::endl;
    }
}


void async_server()
{
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(), 2001);  //listen 2001

    ip::tcp::acceptor acc(service, ep);
    auto sock = std::make_shared<boost::asio::ip::tcp::socket>(service);

    try
    {
        acc.async_accept(*sock, std::bind(handle_accept, sock, std::placeholders::_1));
    }
    catch (std::exception& ec)
    {
        std::cout << "Message:" << ec.what() << std::endl;
    }
    service.run();
}


void timeout_handler()
{
    std::cout << "timeOut" << std::endl;
}

void connect_handler2()
{
    std::cout << "handler2 " << std::endl;
}

void testOneServiceOneThread()
{
    io_service service;

    ip::tcp::socket socket1(service);
    ip::tcp::socket socket2(service);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    socket1.async_connect(ep, std::bind(connect_handler2));
    socket2.async_connect(ep, std::bind(connect_handler2));

    boost::asio::deadline_timer t(service, boost::posix_time::seconds(5));

    t.async_wait(std::bind(timeout_handler));

    service.run();
}


void run_service(std::shared_ptr<boost::asio::io_service> service_ptr)
{
    service_ptr->run();
}


void OneServiceMulThread()
{
    auto service_ptr = std::make_shared<boost::asio::io_service>();
    ip::tcp::socket sock1(*service_ptr);
    ip::tcp::socket sock2(*service_ptr);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    sock1.async_connect(ep, std::bind(connect_handler));
    sock2.async_connect(ep, std::bind(connect_handler));

    boost::asio::deadline_timer t(*service_ptr, boost::posix_time::seconds(10));
    t.async_wait(std::bind(timeout_handler));

    boost::thread_group threads;

    for(int i = 0; i < 5; ++i)
    {
        threads.create_thread(std::bind(run_service, service_ptr));
    }

    std::cin.get();
    service_ptr->stop();
    threads.join_all();
}



void WorkThread(std::shared_ptr<boost::asio::io_service>& service)
{
    service->run();
}



void multiServiceMultithread()
{
    auto service_ptr_1 = std::make_shared<boost::asio::io_service>();
    auto service_ptr_2 = std::make_shared<boost::asio::io_service>();
    ip::tcp::socket sock1(*service_ptr_1);
    ip::tcp::socket sock2(*service_ptr_2);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    sock1.async_connect(ep, std::bind(connect_handler2));
    sock2.async_connect(ep, std::bind(connect_handler2));
    
    boost::asio::deadline_timer t(*service_ptr_1, boost::posix_time::seconds(10));
    t.async_wait(std::bind(timeout_handler));


    boost::thread_group threads;
    threads.create_thread(std::bind(WorkThread,service_ptr_1));
    threads.create_thread(std::bind(WorkThread,service_ptr_2));

    std::cin.get();


    service_ptr_1->stop();
    service_ptr_2->stop();

    threads.join_all();
    std::cout << "end." << std::endl;
}

//在接收缓冲区特性工作之前，套接字要被打开。否则，会抛出异常]
void GetRecvBufSize()
{
    try 
    {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 2001);
        boost::asio::io_service service;
        boost::asio::ip::tcp::socket sock(service);
        sock.connect(ep);
        ip::tcp::socket::receive_buffer_size    rbs;
        sock.get_option(rbs);
        std::cout << "接收缓冲区的大小 = " << rbs.value() << std::endl;
    }
    catch (std::exception& ec)
    {
        std::cout << "Message : " << ec.what() << std::endl;
    }
}


int main()
{
    return 0;
}