#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>         //-l boost_thread



boost::asio::io_service io_src;
int a;


void WorkThread()
{
    ++a;
    std::cout << a << std::endl;
    io_src.run();
    std::cout << "end. \n";
}


int main()
{
    auto woker_sp = std::make_shared<boost::asio::io_service::work>(io_src);

    std::cout << "please ENTER key to exit" << std::endl;
    boost::thread_group threads;
    for(int i = 0; i < 5; ++i)
    {
        threads.create_thread(WorkThread);
    }

    std::cin.get();

    io_src.stop();
    threads.join_all();
    return 0;
}