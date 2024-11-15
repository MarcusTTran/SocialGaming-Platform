#include "Messenger.h"

Messenger::Messenger(networking::Server &server) : server(server) {}

void Messenger::broadcast(const std::string &message, const std::vector<networking::Connection> &connections) {
  for (const auto &connection : connections) {
    server.send({{connection, message}});
  }
}

void Messenger::sendToConnection(const std::string &message, const networking::Connection &connection) {
  server.send({{connection, message}});
}

void Messenger::sendMessages(const std::deque<networking::Message> &messages) { server.send(messages); }