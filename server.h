#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <memory>
#include "connection.h"
#include <set>
#include "requestconfig.h"


namespace Team15 {
namespace server {

class server
{
 public:
  //Constructor
  server(const server&) = delete;
  server& operator=(const server&) = delete;
  ~server();
  explicit server(const std::string& address, const std::string& port, std::vector<requestconfig>& handlerRoots);

  // Check if server is valid
  bool is_valid(const std::string& address, const std::string& port);
  
  // Run the server's io loop
  void run();
  void connection_done(connection* connection);
  boost::asio::io_service& getService();
  const std::vector<requestconfig>& getRequestConfigVector() const;
private:
  void start_accepting();
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::set<connection*> connections_;
  std::vector<requestconfig> requestConfigVector_;
};
}
}
#endif
