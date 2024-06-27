#include <iostream>
#include <memory>

#include "boost/asio/write.hpp"
#include "boost/system/detail/error_code.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/system/error_code.hpp>
#include <string>

using namespace boost::beast;
int main() {
  try {
    // 定义主机名、端口和目标路径
    auto const host = "127.0.0.1";
    auto const port = "8889";
    auto const target = "/1234";
    int version = 11; // 使用 HTTP 1.1 版本

    boost::asio::io_context ioc;

    tcp_stream stream(ioc);
    boost::asio::ip::tcp::resolver resolver(ioc);
    // 查找域名
    auto const results = resolver.resolve(host, port);
    stream.connect(results);

    // 设置 HTTP GET 请求消息
    http::request<http::string_body> req{http::verb::get, target, version};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // 发送HTTP请求到远程主机
    http::write(stream, req);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}
