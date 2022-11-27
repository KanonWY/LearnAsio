#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

std::mutex global_stream_lock;

#define AUTOLOCK \
    std::lock_guard<std::mutex> lock(global_stream_lock);


void WorkerThread(std::shared_ptr<boost::asio::io_service> iosvc, int counter)
{
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << std::endl;
    }

    while(true) {
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
        catch (std::exception &ec)
        {
            AUTOLOCK;
            std::cout << "Message: " << ec.what() << std::endl;
        }
    }
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << "End. \n";
    }
}

int main()
{
    auto iosvc = std::make_shared<boost::asio::io_service>();
    auto work = std::make_shared<boost::asio::io_service::work>(*iosvc);
    auto strand = std::make_shared<boost::asio::io_service::strand>(*iosvc);
    {
        AUTOLOCK;
        std::cout << "Press ENTER to exit!\n";
    }
    boost::thread_group threads;
    for (int i = 0; i < 2; ++i)
    {
        threads.create_thread(std::bind(&WorkerThread, iosvc, i));
    }

    boost::asio::ip::tcp::socket sckt(*iosvc);

    try {
        boost::asio::ip::tcp::resolver resolver(*iosvc);
        boost::asio::ip::tcp::resolver::query query("www.baidu.com", boost::lexical_cast<std::string>(80));
        auto iterator = resolver.resolve(query);

        boost::asio::ip::tcp::endpoint endpoint = *iterator;

        {
            AUTOLOCK;
            std::cout << "Connection to" << endpoint << std::endl;
        }
        sckt.connect(endpoint);
        std::cout << "Connected!\n";
    }
    catch(std::exception &ex)
    {
        AUTOLOCK;
        std::cout << "Message: " << ex.what() << std::endl; 
    }
    std::cin.get();
    boost::system::error_code ec;
    sckt.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    sckt.close(ec);

    iosvc->stop();

    threads.join_all();
    return 0;
}