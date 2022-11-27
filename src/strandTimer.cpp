#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <functional>
#include <mutex>

std::mutex global_stream_lock;

#define AUTOLOCK \
    std::lock_guard<std::mutex> lock(global_stream_lock);

void WorkerThread(std::shared_ptr<boost::asio::io_service> iosvc, int counter)
{
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << std::endl; 
    }

    while (true) {
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
        catch (std::exception &ex) {
            AUTOLOCK;
            std::cout << "Message: " << ex.what() << std::endl;
        }
    }
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << "End.\n";
    }
}


void TimerHandler(
    const boost::system::error_code &ec,
    std::shared_ptr<boost::asio::deadline_timer> tmr,
    std::shared_ptr<boost::asio::io_service::strand> strand
) {
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
        tmr->expires_from_now(boost::posix_time::seconds(1));
        tmr->async_wait(strand->wrap(std::bind(&TimerHandler, std::placeholders::_1, tmr, strand)));
    }
}

void Print(int number)
{
    std::cout << "Number: " << number << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

int main()
{
    auto iosvc = std::make_shared<boost::asio::io_service>();
    auto worker_sp = std::make_shared<boost::asio::io_service::work>(*iosvc);

    auto strand_sp = std::make_shared<boost::asio::io_service::strand>(*iosvc);
    {
        AUTOLOCK;
        std::cout << "Press ENTER to exit!\n";
    }
    boost::thread_group threads;
    for(int i=1; i<=5; i++)
        threads.create_thread(boost::bind(&WorkerThread, iosvc, i));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    strand_sp->post(std::bind(Print, 1));
    strand_sp->post(std::bind(Print, 2));
    strand_sp->post(std::bind(Print, 3));
    strand_sp->post(std::bind(Print, 4));
    strand_sp->post(std::bind(Print, 5));

    auto timer_sp = std::make_shared<boost::asio::deadline_timer>(*iosvc);

    timer_sp->expires_from_now(boost::posix_time::seconds(1));
    timer_sp->async_wait(
        strand_sp->wrap(std::bind(&TimerHandler, std::placeholders::_1, timer_sp, strand_sp))
    );

    std::cin.get();
    iosvc->stop();
    threads.join_all();

    return 0;
}