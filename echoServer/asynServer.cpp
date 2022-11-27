#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

const constexpr int max_msg = 1024;

class talk_to_svr;



using service_ptr = std::shared_ptr<boost::asio::io_service>;
using talk_ptr = std::shared_ptr<talk_to_svr>;
using error_code = boost::system::error_code;


size_t read_complete(char *buf, const boost::system::error_code& ec, size_t bytes)
{
    if (ec)
    {
        std::cout << "read_complete error" << std::endl;
        return 0;
    }
    bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
    return found ? 0 : 1;
}


class talk_to_svr: public std::enable_shared_from_this<talk_to_svr>, boost::noncopyable {
private:
    talk_to_svr(const std::string& message, service_ptr service)
        :m_start(true), m_sock(boost::asio::ip::tcp::socket(service)), m_service(service)
        {

        }

    void start(boost::asio::ip::tcp::endpoint ep)
    {
        m_sock.async_connect(ep, std::bind(&self_type::c));
    }

public:
    using self_type = talk_to_svr;
    static talk_ptr start(service_ptr service, boost::asio::ip::tcp::endpoint ep, const std::string& message)
    {
        auto ptr = std::make_shared<talk_to_svr>(message, service);
        ptr->start(ep);
    }

    void stop()
    {
        if (!m_start) return;
        m_start = false;
        m_sock.close(); 
    }

    bool started()
    {
        return m_start;
    }

    void do_read()
    {
        boost::asio::async_read(m_sock,
            boost::asio::buffer(m_read_buffer),
            std::bind(
                read_complete,
                std::placeholders::_1,
                std::placeholders::_2
            ),
            std::bind(
                on_read,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
    }

    void on_read(const error_code& err, size_t bytes)
    {
        if (!err)
        {
            std::string copy_str(m_read_buffer, bytes - 1);
            std::cout << "recv buffer = [" << copy_str.c_str() << "]" << std::endl;
            stop();
        }
    }

    void on_connect(const error_code& err)
    {
        if (!err)
        {
            do_write(m_message + "\n");
        }
        else
        {
            stop();
        }
    }

    void do_write(const std::string& msg)
    {
        if (!started())
        {
            return;
        }
        std::copy(msg.begin(), msg.end(), m_write_buffer);
        m_sock.async_write_some(boost::asio::buffer(m_write_buffer, msg.size()),
            std::bind(
                &self_type::on_write,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
    }

    void on_write(const error_code& err, size_t bytes)
    {
        do_read();
    }

private:
    boost::asio::ip::tcp::socket m_sock;
    service_ptr m_service;
    char m_read_buffer[max_msg];
    char m_write_buffer[max_msg];
    bool m_start;
    std::string m_message;
};



int main()
{
    auto server_ptr = std::make_shared<boost::asio::io_service>();
    boost::asio::ip::tcp::endpoint ep(
        boost::asio::ip::address::from_string("127.0.0.1"),
        8001
    );

    char* messages[] = { "John says hi", "so does James", "Lucy got home", 0 };

    for(char** msg = messages; *msg; ++msg)
    {
        
    }

    return 0;
}