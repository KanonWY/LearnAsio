#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <memory>
#include <mutex>


std::mutex g_mutex;


void WorkThread(std::shared_ptr<boost::asio::io_service>io_svc, int counter)
{
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << counter << std::endl;
    }
    io_svc->run();
    std::cout << "End.\n";
}

int main()
{
    auto io_svc = std::make_shared<boost::asio::io_service>();
    auto woker_sp = std::make_shared<boost::asio::io_service::work>(*io_svc);

    std::cout << "按下Enter键退出" << std::endl;

    boost::thread_group threads;
    for(int i = 0; i < 5; ++i)
    {
        threads.create_thread(std::bind(&WorkThread,io_svc, i));
    }

    std::cin.get();
    io_svc->stop();
    threads.join_all();
    return 0;
}