#include <boost/asio/buffer.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <iostream>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/system/error_code.hpp>
#include <memory>

class Connection {

public:
  Connection(boost::asio::io_context &ioc) : socket_{ioc}, buf_{2048} {}
  void Start() {
    boost::beast::http::async_read(
        socket_, buf_, request_,
        [this](boost::system::error_code ec, std::size_t sz) {
          if (ec) {
            std::cout << "read error: " << ec.message() << std::endl;
            return;
          }
          std::cout << "recv size: " << sz << std::endl;
          std::cout << "body: " << request_.body() << std::endl;
          std::cout << "url: " << request_.target() << std::endl;
          std::cout << "http version: " << request_.version() << std::endl;
          std::cout << "requuest method: " << request_.method() << std::endl;
          std::cout << "base: " << request_.base() << std::endl;
          std::cout << "host: " << request_.at("Host") << std::endl;
        });
  }

  boost::asio::ip::tcp::socket &GetSocket() { return socket_; }

private:
  boost::asio::ip::tcp::socket socket_;
  boost::beast::http::request<boost::beast::http::string_body> request_;
  boost::beast::flat_buffer buf_;
};

class CServer {
public:
  CServer(boost::asio::io_context &ioc)
      : ioc_{ioc}, acceptor_{ioc_, boost::asio::ip::tcp::endpoint{
                                       boost::asio::ip::tcp::v4(), 8889}} {}

  void Start() {
    std::shared_ptr<Connection> conn = std::make_shared<Connection>(ioc_);
    acceptor_.async_accept(
        conn->GetSocket(), [this, conn](boost::system::error_code ec) {
          if (ec.value()) {
            std::cout << "accept err: " << ec.message() << std::endl;
          
          } else {
            std::cout << "accept one connection" << std::endl;
            conn->Start();
          }
          // this->Start(); // 接收新连接
        });
  }

private:
  boost::asio::io_context& ioc_;
  boost::asio::ip::tcp::acceptor acceptor_;
};

int main() {
  boost::asio::io_context ioc{1};
  CServer server{ioc};
  server.Start();
  ioc.run();

  return 0;
}