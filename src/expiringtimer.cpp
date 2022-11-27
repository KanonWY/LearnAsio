#include <iostream>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/thread.hpp>


std::mutex global_stream_lock;


#define AUTOLOCK \
    std::lock_guard<std::mutex> lock(global_stream_lock);

void WorkerThread(std::shared_ptr<boost::asio::io_service> iosvr, int counter)
{
    {
        AUTOLOCK
        std::cout << "Thread " << counter << std::endl;
    }
    while (true)
    {
        try {
            boost::system::error_code ec;
            iosvr->run(ec);
            if (ec)
            {
                AUTOLOCK;
                std::cout << "Message :" << ec << std::endl;
            }
            break;
        }
        catch(std::exception& e)
        {
            AUTOLOCK;
            std::cout << "Message: " << e.what() << std::endl;
        }
    }

    {
        AUTOLOCK;
        std::cout << "Thread" << counter << "End." <<std::endl;
    }
}


void TimerHandler(const boost::system::error_code& ex) {
    if (ex)
    {
        AUTOLOCK;
        std::cout << "Error Message "  << ex << std::endl;
    }
    else
    {
        std::cout << "You see this line because you have waited for 10 seconds\n";
        std::cout << "Now press ENTER to exit.\n";
    }
}

int main()
{
    auto io_svc = std::make_shared<boost::asio::io_service>();
    auto worker_ptr = std::make_shared<boost::asio::io_service::work>(*io_svc);


    {
        AUTOLOCK;
        std::cout << "Wait for ten seconds to see what happen, ";
        std::cout << "otherwise press ENTER to exit!\n";
    }
    boost::thread_group threads;
    for(int i=1; i<=5; i++)
    {
        threads.create_thread(std::bind(&WorkerThread, io_svc, i));
    }

    boost::asio::deadline_timer timer(*io_svc);
    timer.expires_from_now(boost::posix_time::seconds(10));
    timer.async_wait(TimerHandler);             //异步等待Timer

    std::cin.get();
    io_svc->stop();
    threads.join_all();
    return 0;
}