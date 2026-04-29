# servic.cpp：std::coroutine与asio的无缝衔接

Appweb的续作，基于C++20的`coroutine`与`boost::asio`的服务端。

路由器是另一个小组件：[yauntyour/router: 原生C++实现的路由器：Router](https://github.com/yauntyour/router)

## Example

```c++
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

        // 标准路由
        ros.on("/", [](std::string &input, std::string &output, const std::map<std::string, std::string> &params) -> int
               {
            output = "HTTP/1.1 200 OK\r\n\r\nHello World!";
            return 0; });

        // 流式路由 — 分块写入响应
        ros.on_stream("/stream", [](std::string &req, rt::WriteCallback write, const std::map<std::string, std::string> &params)
               {
            write("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            write("chunk1");
            write("chunk2");
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
```

## Stream Route

流式路由通过 `on_stream` 注册，处理器接收一个 `WriteCallback`，可多次调用来分块写入数据。适用于 SSE、大文件传输、实时数据推送等场景。

```c++
ros.on_stream("/chat", [](std::string &req, rt::WriteCallback write, const std::map<std::string, std::string> &params) {
    write("data: hello\n\n");
    write("data: world\n\n");
});
```

