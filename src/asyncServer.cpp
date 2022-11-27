#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <mutex>
#include <functional>


std::mutex global_stream_lock;

#define AUTOLOCK \
    std::lock_guard<std::mutex> lock(global_stream_lock);


void WorkerThread(std::shared_ptr<boost::asio::io_service> iosvc, int counter)
{
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << std::endl;
    }
    while(true)
    {
        try {
            boost::system::error_code ec;
            iosvc->run(ec);
            if (ec)
            {
                AUTOLOCK;
                std::cout << "Message: " << ec << std::endl;
            }
            break;
        }
        catch (std::exception &ex)
        {
            AUTOLOCK;
            std::cout << "Message: " << ex.what() << std::endl;
        }
    }
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << std::endl;
    }
}
void OnAccept(const boost::system::error_code &ec)
{
    if (ec)
    {
        AUTOLOCK;
        std::cout << "OnAccept Error" << std::endl;
    }
    else
    {
        AUTOLOCK;
        std::cout << "Accepted" << std::endl;
    }
}

int main(void)
{
    auto io_svc = std::make_shared<boost::asio::io_service>();
    auto worker_ptr = std::make_shared<boost::asio::io_service::work>(*io_svc);

    auto strand = std::make_shared<boost::asio::io_service::strand>(*io_svc);

    {
        AUTOLOCK;
        std::cout << "Press ENTER to exit!\n";
    }

    boost::thread_group threads;
    for(int i = 1; i <= 2; i++)
    {
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));
    }
    auto acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*io_svc);

    auto sckt = std::make_shared<boost::asio::ip::tcp::socket>(*io_svc);


    try {
        boost::asio::ip::tcp::resolver resolver(*io_svc);
        boost::asio::ip::tcp::resolver::query query(
            "127.0.0.1",
            boost::lexical_cast<std::string>(4444)
        );
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor->open(endpoint.protocol());
        acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor->bind(endpoint);
        acceptor->listen(boost::asio::socket_base::max_connections);
        acceptor->async_accept(*sckt, std::bind(OnAccept, std::placeholders::_1));

        {
            AUTOLOCK;
            std::cout << "Listen ON..." << endpoint <<  std::endl;
        }
    }
    catch(std::exception &ec)
    {
        AUTOLOCK;
        std::cout << "Message: " << ec.what() << std::endl;
    }
    std::cin.get();

    boost::system::error_code ec;
    acceptor->close(ec);
    sckt->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    sckt->close(ec);
    io_svc->stop();
    threads.join_all();
    return 0;
}