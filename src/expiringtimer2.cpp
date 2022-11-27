#include <iostream>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

std::mutex global_stream_lock;

#define AUTOLOCK \
    std::lock_guard<std::mutex> lock(global_stream_lock);

void WorkThread(std::shared_ptr<boost::asio::io_service> iosvc, int counter)
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
        {
            AUTOLOCK;
            std::cout << "Thread: " << counter << "End." << std::endl;
        }
    }
}

void TimerHandler(
    const boost::system::error_code &ec,
    std::shared_ptr<boost::asio::deadline_timer> tmr
)
{
    if (ec)
    {
        AUTOLOCK;
        std::cout << "Error Message: " << ec << std::endl;
    }
    else
    {
        {
            AUTOLOCK;
            std::cout << "You see this every three seconds.\n";
        }
        tmr->expires_from_now(boost::posix_time::seconds(3));
        tmr->async_wait(std::bind(&TimerHandler, std::placeholders::_1,tmr));
    }
}

int main()
{
    auto iosvc = std::make_shared<boost::asio::io_service>();
    auto worker_sp = std::make_shared<boost::asio::io_service::work>(*iosvc);
    {
        AUTOLOCK;
        std::cout << "Press ENTER to exit!\n";
    }
    boost::thread_group threads;
    for(int i=1; i<=5; i++)
        threads.create_thread(boost::bind(WorkThread, iosvc, i));

    auto time_sp = std::make_shared<boost::asio::deadline_timer>(*iosvc);

    time_sp->expires_from_now(boost::posix_time::seconds(3));
    time_sp->async_wait(std::bind(TimerHandler, std::placeholders::_1, time_sp));
    std::cin.get();
    iosvc->stop();
    threads.join_all();
    return 0;
}