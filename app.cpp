#include "servic.hpp"
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
namespace asio = boost::asio;
int main()
{
    try
    {
        rt::router ros;
        asio::io_context io_context;
        servic::Server server(io_context, 8080, 300000);

        ros.on("/", [](std::string &input, std::string &output, const std::map<std::string, std::string> &params) -> int
               {
            std::cout<<"root"<<std::endl;
            output = "HTTP/1.1 200 OK\r\n\r\n Hello World!";
            return 0; });
        ros.on("/test", [](std::string &input, std::string &output, const std::map<std::string, std::string> &params) -> int
               {
            std::cout<<"test"<<std::endl;
            output = "HTTP/1.1 200 OK\r\n\r\n Test.";
            return 0; });

        // 流式路由 — 分块写入
        ros.on_stream("/stream", [](std::string &req, rt::WriteCallback write, const std::map<std::string, std::string> &params)
               {
            write("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            write("chunk1\n");
            write("chunk2\n");
            write("chunk3"); });

        server.run(ros);
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}