#include "Messenger.h"
#include <cassert>

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

void Messenger::sendMessageToPlayerMap(const std::string &message, const DataValue::OrderedMapType &playerMap) {
  std::string connectionKey = "connection";

  // Find the connection in the map
  auto connectionIt = std::find_if(playerMap.begin(), playerMap.end(),
                                   [connectionKey](const auto &pair) { return pair.first == connectionKey; });

  // Should always be found
  if (connectionIt == playerMap.end()) {
    assert(false && "Connection not found in player map");
  }

  // Get the connection from the map
  auto connection = connectionIt->second.asConnection();

  // Send the message to the connection
  server.send({{connection, message}});
}