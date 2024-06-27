#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <iostream>
#include <string>



int sync_accept() {
  boost::asio::io_service io_service;
  boost::system::error_code ec;
  // boost::asio::ip::tcp::endpoint server_endpoint(
  //     boost::asio::ip::address_v4::any(), 8889);
  boost::asio::ip::tcp::endpoint server_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8889);
  boost::asio::ip::tcp::acceptor acceptor(io_service);
  
  (void )acceptor.open(server_endpoint.protocol(), ec);
  if(ec.value() != 0) {
    std::cout << "open error: " << ec.message() << std::endl;
    return -1;
  }

  (void)acceptor.bind(server_endpoint, ec);

  if (ec.value() != 0) {
    std::cout << "bind error: " << ec.message() << std::endl;
    return -1;
  }

  acceptor.listen();
  auto client_socket = acceptor.accept();

  char char_buf[256];
  std::size_t recv_len = client_socket.read_some( boost::asio::buffer(char_buf), ec);
  if (ec.value() != 0) {
    std::cout << "read error: " << ec.message() << std::endl;
    return -1;
  }
  char_buf[recv_len] = '\0';
  std::cout << "recv len: " << recv_len << " msg: " << char_buf << std::endl;
  (void)client_socket.shutdown(
      boost::asio::ip::tcp::socket::shutdown_type::shutdown_receive, ec);

  std::string send_buf = "hello client";
  client_socket.send(boost::asio::buffer(send_buf), 0, ec);
  if(ec) {
    std::cout << "send err: " << ec.message() << std::endl;
    return -1;
  }
  (void)client_socket.shutdown(
      boost::asio::ip::tcp::socket::shutdown_type::shutdown_send, ec);

  return 0;
}

int main() {

  return sync_accept();
}
