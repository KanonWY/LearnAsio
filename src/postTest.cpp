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

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << "End.\n";
    }
}

size_t fac(size_t n) {
    if (n <= 1)
    {
        return n;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return n * fac(n - 1);
}


void CalculateFactorial(size_t n)
{
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << "Calculating " << n << "! factorial" << std::endl;
    }
    size_t f = fac(n);
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << n << "! = " << f << std::endl;
    }
}

int main()
{
    auto io_svc = std::make_shared<boost::asio::io_service>();
    auto woker_sp = std::make_shared<boost::asio::io_service::work>(*io_svc);
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << "The program will exit once all work has finished." << std::endl;
    }

    boost::thread_group threads;
    for (int i = 0; i < 5; ++i)
    {
        threads.create_thread(std::bind(&WorkThread, io_svc, i));
    }

    io_svc->post(std::bind(CalculateFactorial, 5));
    io_svc->post(std::bind(CalculateFactorial, 6));
    io_svc->post(std::bind(CalculateFactorial, 7));

    woker_sp.reset();

    threads.join_all();
    return 0;
}