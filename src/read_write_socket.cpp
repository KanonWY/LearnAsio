#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <mutex>
#include <functional>
#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <boost/lexical_cast.hpp>

std::mutex global_stream_lock;

#define AUTOLOCK \
    std::lock_guard<std::mutex> lock(global_stream_lock);

void WorkThread(std::shared_ptr<boost::asio::io_service> iosec, int counter)
{
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << " Start.\n";
    }

    while(true)
    {
        try {
            boost::system::error_code ec;
            iosec->run(ec);
            if (ec)
            {
                AUTOLOCK;
                std::cout << "Message: " << ec << ".\n";
            }
            break;
        }
        catch(std::exception& ec)
        {
            AUTOLOCK;
            std::cout << "Message: " << ec.what() << ".\n";
        }
    }
    {
        AUTOLOCK;
        std::cout << "Thread " << counter << " End.\n";
    }
}

struct ClientContext :public std::enable_shared_from_this<ClientContext> {
    boost::asio::ip::tcp::socket m_socket;
    using buffer_t = std::vector<std::uint8_t>;
    buffer_t m_recv_buffer;
    size_t m_recv_buffer_index;
    std::list<buffer_t> m_send_buffer_list;
    
    //default construct
    ClientContext(boost::asio::io_service& io_service)
    :m_socket(io_service), m_recv_buffer_index(0)
    {
        m_recv_buffer.resize(4096);
    }
    ~ClientContext() {}

    void Close()
    {
        boost::system::error_code ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close();
    }

    void OnSend(const boost::system::error_code &ec,
        std::list<std::vector<std::uint8_t>>::iterator itr
    )
    {
        if (ec)
        {
            AUTOLOCK;
            std::cout << "OnSendError: " << ec << std::endl;
            Close();
        }
        else
        {
            AUTOLOCK;
            std::cout << "Sent " << (*itr).size() << "bytes!" << std::endl; 
        }
        m_send_buffer_list.erase(itr);
        if (!m_send_buffer_list.empty())
        {
            boost::asio::async_write(m_socket, boost::asio::buffer(m_send_buffer_list.front()),
                std::bind(&ClientContext::OnSend, 
                            shared_from_this(),
                            std::placeholders::_1,
                            m_send_buffer_list.begin()
                        )
            );
        }
    }

    void Send(const void *buffer, size_t length) 
    {
        //是否立即发送
        bool can_send_now =false;
        using buffer_t = std::vector<std::uint8_t>;
        buffer_t output;
        //将输入的buffer传入到output
        std::copy((const std::uint8_t *)buffer, (const std::uint8_t*)buffer + length, 
        std::back_inserter(output));
        
        //
        can_send_now = m_send_buffer_list.empty();

        m_send_buffer_list.push_back(output);

        if (can_send_now)
        {
            boost::asio::async_write(m_socket,
                boost::asio::buffer(m_send_buffer_list.front()),
                std::bind(
                    &ClientContext::OnSend,
                    shared_from_this(),
                    std::placeholders::_1,
                    m_send_buffer_list.begin()
                )
            );
        }

    }

    void OnRecv(const boost::system::error_code &ec, size_t bytes_transferred)
    {
        if (ec)
        {
            {
                AUTOLOCK;
                std::cout << "OnRecv Error: " << ec << ".\n";
            }
            Close();
        }
        else
        {
            m_recv_buffer_index += bytes_transferred;

            {
                AUTOLOCK;
                std::cout << "Recv " << bytes_transferred << " bytes." << std::endl;
            }
            for(size_t x = 0; x < m_recv_buffer_index; ++x)
            {
                std::cout << (char)m_recv_buffer[x] << " ";
                if ((x + 1) % 16 == 0)
                {
                    std::cout << std::endl;
                }
            }
            {
                AUTOLOCK;
                std::cout << std::endl << std::dec;
            }
            m_recv_buffer_index = 0;
            Recv();
        }
    }

    void Recv()
    {
        m_socket.async_read_some(
            boost::asio::buffer(
                &m_recv_buffer[m_recv_buffer_index],
                m_recv_buffer.size() - m_recv_buffer_index),
                std::bind(
                    &ClientContext::OnRecv,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2
                )
            );         
    }
};


void OnAccept(const boost::system::error_code &ec, std::shared_ptr<ClientContext> clt)
{
    if (ec)
    {
        AUTOLOCK;
        std::cout << "OnAccept Error" << std::endl;
    }
    else 
    {
        {
            AUTOLOCK;
            std::cout << "Accept" << std::endl;
        }
        clt->Send("Hi Thread", sizeof("Hi Thread"));
        clt->Recv();
    }
}

int main()
{
    auto io_svc = std::make_shared<boost::asio::io_service>();

    auto woker = std::make_shared<boost::asio::io_service::work>(*io_svc);

    auto strand = std::make_shared<boost::asio::io_service::strand>(*io_svc);

    {
        AUTOLOCK;
        std::cout << "Press ENTER to exit!\n";
    }

    //仅仅创建一个线程处理

    boost::thread_group threads;
    threads.create_thread(std::bind(&WorkThread, io_svc, 1));


    auto acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*io_svc);

    auto client = std::make_shared<ClientContext>(*io_svc);

    try
    {
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
        acceptor->async_accept(client->m_socket,
            std::bind(
                OnAccept,
                std::placeholders::_1,
                client
            )
        );
        {
            AUTOLOCK;
            std::cout << "async_accept" << std::endl;
        }        

        {
            AUTOLOCK;
            std::cout << "Listening on: " << endpoint << std::endl;
        }
    }
    catch (std::exception& ex)
    {
        AUTOLOCK;
        std::cout << "Message: " << ex.what() << ".\n";
    }

    std::cin.get();
    boost::system::error_code ec;

    acceptor->close(ec);

    io_svc->stop();
    threads.join_all();
    return 0;
}