#include "HttpRequest.h"
#include "HttpResponse.h"
#include "connection.h"
#include "server.h"
#include <boost/bind.hpp>
namespace Team15 {
namespace server {
  connection::connection(boost::asio::ip::tcp::socket socket,server* server):requestmgr_(server->getRequestConfigVector()),socket_(std::move(socket)),buffer_(),server_(server) {
  }
  void connection::start() {
    start_reading();
  }
  void connection::stop() {
    socket_.close();
  }

  void connection::start_reading() {
    socket_.async_read_some(boost::asio::buffer(buffer_),
			    boost::bind(&connection::read_handler,
					this,boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
  }
  void connection::read_handler(const boost::system::error_code& ec,std::size_t bytes_transferred) {
    if (!ec) {
      parse_request(bytes_transferred);
      start_writing();
    }
    else {
      server_->connection_done(this);
    }
  }
  void connection::parse_request(std::size_t request_length) {
    std::vector<unsigned char> wire;
    std::size_t counter = 0;
    while (counter < request_length) {
      wire.push_back(buffer_[counter++]);
    }
    requestmgr_.handleRequest(wire);
  }
  void connection::start_writing() {
    std::unique_ptr<HttpResponse> response = requestmgr_.generateResponse();
    const std::string response_buf(response->toText());
    boost::asio::async_write(socket_,boost::asio::buffer(response_buf),
			    boost::bind(&connection::write_handler,
					this,boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

  }
  void connection::write_handler(const boost::system::error_code& ec,std::size_t bytes_transferred) {
    if (!ec) {
      boost::system::error_code ignored_ec;
      socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }
    server_->connection_done(this);
  }
  void connection::setBuffer(std::string& str) {
    std::array<char,max_length> newBuff;
    for (unsigned int i = 0; i < str.length(); ++i) {
      newBuff[i] = str[i];
    }
    buffer_ = newBuff;
  }
}
}
