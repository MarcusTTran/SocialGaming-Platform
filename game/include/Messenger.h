#pragma once
#include "Server.h"
#include <deque>
#include <string>
#include <vector>

class IServer {
public:
  virtual ~IServer() = default;
  virtual void broadcast(const std::string &message, const std::vector<networking::Connection> &connections) = 0;
  virtual void sendToConnection(const std::string &message, const networking::Connection &connection) = 0;
  virtual void sendMessages(const std::deque<networking::Message> &messages) = 0;
};

class Messenger : public IServer {

public:
  Messenger(networking::Server &server);
  void broadcast(const std::string &message, const std::vector<networking::Connection> &connections) override;
  void sendToConnection(const std::string &message, const networking::Connection &connection) override;
  void sendMessages(const std::deque<networking::Message> &messages) override;

private:
  networking::Server &server;
};
