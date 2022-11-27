#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <mutex>

#define autolock(m) \
    std::lock_guard<std::mutex> lock(m);


std::mutex g_mutex;

void WorkThread(std::shared_ptr<boost::asio::io_service> iosvc, int counter)
{
    {
        autolock(g_mutex);
        std::cout << "Thread " << counter << std::endl;
    }
    try {
        iosvc->run();
        {
            autolock(g_mutex);
            std::cout << "Thread " << counter << std::endl;
        }
    } catch (std::exception &ex) {
        autolock(g_mutex);
        std::cout << "Message " << ex.what() << std::endl;
    }
}

void ThrowAnExeception(std::shared_ptr<boost::asio::io_service> iosvc, int counter) {
    {
        autolock(g_mutex);
        std::cout << "Throw Execption: " << counter << std::endl; 
    }
    throw(std::runtime_error("The Execption!!!"));
}


int main()
{
    auto io_sp = std::make_shared<boost::asio::io_service>();
    auto worker_sp = std::make_shared<boost::asio::io_service::work>(*io_sp);
    {
        autolock(g_mutex);
        std::cout << "The program will exit once all work has finished \n";
    }

    //Thread group
    boost::thread_group threads;
    for (int i = 0; i < 5; ++i)
    {
        threads.create_thread(std::bind(WorkThread, io_sp, i));
    }

    io_sp->post(std::bind(ThrowAnExeception, io_sp, 1));            //push task to io_service
    io_sp->post(std::bind(ThrowAnExeception, io_sp, 2));
    io_sp->post(std::bind(ThrowAnExeception, io_sp, 3));
    io_sp->post(std::bind(ThrowAnExeception, io_sp, 4));
    io_sp->post(std::bind(ThrowAnExeception, io_sp, 5));

    threads.join_all();
    return 0;
}