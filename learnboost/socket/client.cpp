#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <vector>


void sync_connect() {
  boost::asio::io_service io_service;

  boost::asio::ip::tcp::socket socket_(io_service);

  auto server_address_ = boost::asio::ip::address::from_string("127.0.0.1");

  boost::asio::ip::tcp::endpoint service_endpint(server_address_, 8889);
  boost::system::error_code ec = boost::system::error_code();

  (void)socket_.connect(service_endpint, ec);
  if(0 != ec.value()) {
    std::cout << "error code:" << ec.value()
              << ", error message: " << ec.message() << std::endl;
  }

  std::string request = "hello server";
  int send_len = socket_.write_some(boost::asio::buffer(request), ec);
  std::cout << "send size: "<< send_len << std::endl;
  if(0 != ec.value()) {
    std::cout << "error code:" << ec.value()
              << ", error message: " << ec.message() << std::endl;
  }

  (void)socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

  // std::string response;
  // response.resize(1024); // info 用string做buffer必须先开辟缓冲区
  char response[1024];
  std::size_t recv_len = socket_.read_some(boost::asio::buffer(response), ec);
  // response[recv_len] = '\n';
  std::cout << "recv msg: "<< response << std::endl;
  (void)socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_receive, ec);

}


int main() {
  sync_connect();
  return 0;
}