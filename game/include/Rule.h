#pragma once

#include "CommonVariantTypes.h"
#include "Messenger.h"
#include "NameResolver.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Example variables state
// vector([
//   { [configuration, {[name, "RPS"], [player_range, (1, 20)]}], [winners, vector(["Jake", "Mark"])] }, // Parse-time
//   { [rounds, 4] },                                                                                    // Setup-time
//   { [players, vector([...])] },                                                                       // Upon "Start
//   Game" { [round, 0] },                                                                                     //
//   Execution-time - entering for { [player, {[__id__, 23987562], [name, "Jake"], [wins, 0], [weapon, "Rock"]}] } //
//   Execution-time - entering parallel-for
// ])

// fullscope[0] = top level stuff
// fullscope[1] = for rule for RPS

using Scope = Map;

class Rule {
public:
    virtual ~Rule() = default;

    /*
     * Attempt to execute as much of the rule as possible.
     */
    DataValue runBurst(NameResolver &name_resolver) {
        // std::cout << "Running rule" << std::endl;
        if (first_time) {
            _handle_dependencies(name_resolver);
        }
        first_time = false;
        name_resolver.addInnerScope();
        DataValue returnValue = _runBurst(name_resolver);
        name_resolver.removeInnerScope();
        // std::cout << returnValue.getType() << std::endl;
        if (returnValue.isCompleted()) {
            first_time = true;
        }
        return returnValue;
    }

private:
    /*
     * Evaluate execution-time parameters
     */
    virtual void _handle_dependencies(NameResolver &name_resolver) = 0;
    bool first_time = true;

    virtual DataValue _runBurst(NameResolver &name_resolver) = 0;
};

// class NumberRule : public Rule {
// public:
//     NumberRule(int number) : number(number) {}

// private:
//     void _handle_dependencies(NameResolver &name_resolver) override {}
//     DataValue _runBurst(NameResolver &name_resolver) override { return DataValue(number); }
//     const int number;
// };

// class BooleanRule : public Rule {
// public:
//     BooleanRule(bool boolean) : boolean(boolean) {}

// private:
//     void _handle_dependencies(NameResolver &name_resolver) override {}
//     DataValue _runBurst(NameResolver &name_resolver) override { return DataValue(boolean); }
//     const bool boolean;
// };

class NumberRule : public Rule {
public:
    NumberRule(int number) : number(number) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {}
    DataValue _runBurst(NameResolver &name_resolver) override { return DataValue(number); }
    const int number;
};

class BooleanRule : public Rule {
public:
    BooleanRule(bool boolean) : boolean(boolean) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {}
    DataValue _runBurst(NameResolver &name_resolver) override { return DataValue(boolean); }
    const bool boolean;
};

// Note: strings are parsed to look like "{}, choose your weapon!", where the bracket used to have
//       "player.name" inside it. NameResolver will replace these brackets.
class StringRule : public Rule {
public:
    StringRule(std::string_view string_literal,
               std::optional<std::vector<std::unique_ptr<Rule>>> nameResolverRules = std::nullopt)
        : string(string_literal) {
        if (nameResolverRules.has_value()) {
            if (nameResolverRules.value().size() > 0) {
                fresh_variables_maker = std::move(nameResolverRules.value());
                has_fresh_variables = true;
            } else {
                has_fresh_variables = false;
            }
        } else {
            has_fresh_variables = false;
        }
    }

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        // Create NameResolverRules for each fresh variable and add them to found_names in order
        std::cout << "RUNNING HANDLE DEPENDENCIES FOR STRING RULE" << std::endl;
        for (const auto &nameMaker : fresh_variables_maker) {
            DataValue found_name = nameMaker->runBurst(name_resolver);
            std::cout << "Found name: " << found_name << std::endl;

            if (found_name.getType() == "STRING") {
                found_names.push_back(found_name.asString());
            } else if (found_name.getType() == "NUMBER") {
                found_names.push_back(std::to_string(found_name.asNumber()));
            } else {
                // Not number or string so it is an error
                std::cerr << "Error: found name inside StringRule was not a string or int!" << std::endl;
            }
        }
        std::cout << "Found names: " << found_names.size() << std::endl;
        std::cout << "String: " << string << std::endl;
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        if (!has_fresh_variables) {
            return DataValue(string);
        }

        for (const std::string &name : found_names) {
            size_t openBracketPos = string.find('{');
            size_t closeBracketPos = string.find('}');
            if (openBracketPos == std::string::npos || closeBracketPos == std::string::npos) {
                return DataValue(DataValue::RuleStatus::ERROR);
            }
            string.replace(openBracketPos, closeBracketPos - openBracketPos + 1, name);
        }

        std::cout << "String after replacement: " << string << std::endl;
        return DataValue(string);
    }

    std::vector<std::unique_ptr<Rule>> fresh_variables_maker; // Holding fresh variables in brackets
    std::vector<std::string> found_names;                     // Names that will replace the brackets
    std::string string;
    bool has_fresh_variables = false;
};

class SimpleTimerRule : public Rule {
public:
    SimpleTimerRule(long seconds) : seconds(seconds) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        end_time = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    }
    DataValue _runBurst(NameResolver &name_resolver) override {
        if (std::chrono::steady_clock::now() < end_time) {
            return DataValue({DataValue::RuleStatus::NOTDONE});
        }
        return DataValue({DataValue::RuleStatus::DONE});
    }
    long seconds;
    std::chrono::steady_clock::time_point end_time;
};

class SimpleInputRule : public Rule {
public:
    SimpleInputRule(std::shared_ptr<IServer> server, const std::string_view &target_key, const std::string_view &prompt)
        : messager(server), target_key(target_key), prompt(prompt) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        std::cout << "Running handle dependencies FOR input rule" << std::endl;
        auto players = name_resolver.getValue("players").value().asList();

        if (players.empty()) {
            throw std::runtime_error("No players found in global map");
        }

        // Get all the players connections, these are the connections we will be waiting for input from
        std::vector<DataValue> pending_connections;
        for (const DataValue &player : players) {
            pending_connections.push_back(
                DataValue({std::to_string(player.asOrderedMap().at("connection").asConnection().id)}));
        }

        // Add the pending connections to the global map
        name_resolver.addToGlobalScope("pending_connections", DataValue(pending_connections));
        name_resolver.setValue("pending_connections", DataValue(pending_connections));

        auto pending_connections_value = name_resolver.getValue("pending_connections").value().asList();

        for (const DataValue &connection_id : pending_connections_value) {
            networking::Connection connection{std::stoul(connection_id.asString())};
            messager->sendToConnection(formatChoicesMessage(), connection);
        }

        std::cout << "sent message" << std::endl;
    }
    DataValue _runBurst(NameResolver &name_resolver) override {
        // check for message

        std::cout << "Checking for message" << std::endl;

        auto pending_connections = name_resolver.getValue("pending_connections");
        auto incoming_messages = name_resolver.getValue("incoming_messages");

        if (!pending_connections.has_value()) {
            std::cout << "No pending connections" << std::endl;
            return DataValue({DataValue::RuleStatus::NOTDONE});
        }

        if (!incoming_messages.has_value()) {
            std::cout << "No incoming messages" << std::endl;
            return DataValue({DataValue::RuleStatus::NOTDONE});
        }

        auto pending_connections_value = pending_connections.value().asList();
        auto incoming_messages_value = incoming_messages.value().asOrderedMap();

        // print out pending connections

        std::cout << "PENDING CONNECTION SIZE: " << pending_connections_value.size() << std::endl;

        for (const DataValue &connection_id : pending_connections_value) {
            std::cout << "Pending connection: " << connection_id.asString() << std::endl;
        }

        // print out incoming messages

        for (const auto &[connection_id, message] : incoming_messages_value) {
            std::cout << "Incoming message: " << connection_id << " " << message.asString() << std::endl;
        }

        if (pending_connections_value.empty()) {
            return DataValue({DataValue::RuleStatus::DONE});
        }

        if (incoming_messages_value.empty()) {
            return DataValue({DataValue::RuleStatus::NOTDONE});
        }

        // Check if any of the pending connections have a message
        std::vector<DataValue> connections_to_remove;
        std::vector<networking::Connection> connections_to_prompt_again;

        for (const DataValue &connection_id : pending_connections_value) {
            networking::Connection connection{std::stoul(connection_id.asString())};

            auto message = incoming_messages_value.find(connection_id.asString());
            if (message != incoming_messages_value.end()) {
                // Find the player map corresponding to the connection
                auto players = name_resolver.getValue("players").value().asList();
                for (auto &player : players) {
                    auto &player_map = const_cast<DataValue::OrderedMapType &>(player.asOrderedMap());
                    if (player_map["connection"].asConnection().id == connection.id) {

                        // verify the message;
                        if (!verifyMessage(message->second.asString())) {
                            connections_to_prompt_again.push_back(connection);
                            break;
                        }

                        // Set the value in the player map for the specified key
                        player_map[target_key] = message->second;

                        // print out updated player map
                        std::cout << "Updated player map" << std::endl;
                        for (const auto &[key, value] : player_map) {
                            std::cout << key << " " << value << std::endl;
                        }

                        // Remove the connection from the pending connections
                        messager->sendToConnection("You chose " + message->second.asString(), connection);
                        connections_to_remove.push_back(connection_id);
                        break;
                    }
                }
            }
        }

        // Remove the collected connections from the pending connections list
        for (const DataValue &connection_id : connections_to_remove) {
            pending_connections_value.erase(
                std::remove(pending_connections_value.begin(), pending_connections_value.end(), connection_id),
                pending_connections_value.end());
        }

        // Send prompt messages to connections with invalid choices
        for (const networking::Connection &connection : connections_to_prompt_again) {
            messager->sendToConnection("Invalid choice, please try again", connection);
            messager->sendToConnection(formatChoicesMessage(), connection);
        }

        // Update the pending connections in the global map
        name_resolver.setValue("pending_connections", DataValue(pending_connections_value));

        if (pending_connections_value.empty()) {
            return DataValue({DataValue::RuleStatus::DONE});
        }

        return DataValue({DataValue::RuleStatus::NOTDONE});
    }

    bool verifyMessage(const std::string &message) {
        std::string to_lower_message = message;
        std::transform(to_lower_message.begin(), to_lower_message.end(), to_lower_message.begin(), ::tolower);

        return std::find(choices.begin(), choices.end(), message) != choices.end();
    }

    std::string formatChoicesMessage() {
        std::string message = prompt + "\n";
        for (const auto &choice : choices) {
            message += choice + "\n";
        }
        return message;
    }
    std::shared_ptr<IServer> messager;
    std::string target_key;
    std::string prompt;
    std::vector<std::string> choices = {"rock", "paper", "scissors"};
};

class AllPlayersRule : public Rule {
public:
private:
    void _handle_dependencies(NameResolver &name_resolver) override {}

    DataValue _runBurst(NameResolver &name_resolver) override {
        auto playersMap = name_resolver.getValue("players");

        if (playersMap.has_value()) {
            return playersMap.value();
        } else {
            throw std::runtime_error("Players map was not found in global map");
        }
    }
};

class MessageRule : public Rule {
public:
    MessageRule(std::shared_ptr<IServer> server, std::unique_ptr<Rule> recipient_list_maker,
                std::unique_ptr<Rule> string_maker)
        : messager(server), recipient_list_maker(std::move(recipient_list_maker)),
          string_maker(std::move(string_maker)) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        std::cout << "RUNNING HANDLE DEPENDENCIES FOR MESSAGE RULE" << std::endl;

        auto recipients = recipient_list_maker->runBurst(name_resolver);

        if (recipients.getType() == "LIST") {
            this->recipients = recipients.asList();
        } else {
            this->recipients = {recipients};
        }
        auto string = string_maker->runBurst(name_resolver);
        std::cout << "String type: " << string.getType() << std::endl;

        if (string.getType() == "STRING") {
            message = string.asString();
        } else {
            std::cout << "Error: MessageRule string_maker did not return a string" << std::endl;
        }

        // message = string_maker->runBurst(name_resolver).asString();
        std::cout << "Message: from message rule: " << message << std::endl;
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        for (const auto &player : recipients) {
            auto player_map = player.asOrderedMap();
            messager->sendMessageToPlayerMap(message, player_map);
        }

        return DataValue(DataValue::RuleStatus::DONE);
    }

    std::shared_ptr<IServer> messager;
    std::unique_ptr<Rule> recipient_list_maker;
    std::unique_ptr<Rule> string_maker;
    std::vector<DataValue> recipients;
    std::string message;
};

// for round in configuration.rounds.upfrom(1)
// for round in configuration.rounds
class ForRule : public Rule {
public:
    ForRule(std::string fresh_variable_name, std::unique_ptr<Rule> list_maker,
            std::vector<std::unique_ptr<Rule>> contents)
        : fresh_variable_name{std::move(fresh_variable_name)}, list_maker{std::move(list_maker)}, // Move the unique_ptr
          statement_list{std::move(contents)} {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        std::cout << "running for rule for the first time" << std::endl;
        std::cout << "fresh_variable_name: " << fresh_variable_name << std::endl;
        std::cout << "Numver iof rules in the for loop: " << statement_list.size() << std::endl;

        auto list_of_values_generic = list_maker->runBurst(name_resolver);
        std::cout << "list_of_values_generic: " << list_of_values_generic.getType() << std::endl;
        list_of_values = list_of_values_generic.asList();

        // Initialize the iterators
        value_for_this_loop = list_of_values.begin();
        current_statement = statement_list.begin();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // Check for early return
        if (list_of_values.empty() || statement_list.empty()) {
            return DataValue({DataValue::RuleStatus::DONE});
        }

        // Set up fresh variable
        if (list_of_values.size() > 0) {
            name_resolver.addNewValue(fresh_variable_name, *value_for_this_loop);
        }

        while (true) {
            assert(value_for_this_loop != list_of_values.end() && "Iterator for list_of_values is invalid");
            assert(current_statement != statement_list.end() && "Iterator for statement_list is invalid");

            // Run
            auto rule_state = (*current_statement)->runBurst(name_resolver); // Dereference unique_ptr
            if (!rule_state.isCompleted()) {
                return DataValue({DataValue::RuleStatus::NOTDONE});
            }

            // Move to the next statement
            current_statement++;
            if (current_statement != statement_list.end()) {
                continue;
            }

            // Move to the next full iteration
            current_statement = statement_list.begin();
            value_for_this_loop++;
            if (value_for_this_loop != list_of_values.end()) {
                std::cout << "Setting fresh variable" << fresh_variable_name << "To value " << *value_for_this_loop
                          << std::endl;

                name_resolver.setValue(fresh_variable_name, *value_for_this_loop);
                continue;
            }

            // Every iteration complete
            return DataValue({DataValue::RuleStatus::DONE});
        }
    }

    std::string fresh_variable_name;

    std::unique_ptr<Rule> list_maker; // Changed to unique_ptr
    std::vector<DataValue> list_of_values;
    std::vector<DataValue>::iterator value_for_this_loop;

    std::vector<std::unique_ptr<Rule>> statement_list;
    std::vector<std::unique_ptr<Rule>>::iterator current_statement;
};

// - Takes in an integer that represents how many elements we remove from the list
// - Start removal from the last element in the list
// PRECONDITION: list_maker MUST return a list, other data structures like maps are not allowed.
//               integer expression must be <= list size and > 0.
// POSTCONDITION: If integer expression <= 0 or > list size, or list is of incorrect type or empty, return -1.
//                Otherwise, return 1.
class DiscardRule : public Rule {
public:
    DiscardRule(std::unique_ptr<Rule> integer_expr_maker, std::unique_ptr<Rule> list_maker)
        : integer_expr_maker{std::move(integer_expr_maker)}, list_maker{std::move(list_maker)} {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        integerExpression = integer_expr_maker->runBurst(name_resolver).asNumber();
        listToDiscard = list_maker->runBurst(name_resolver).asList();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // Check certain invariants for this functions
        bool listIsIncorrectType = listToDiscard.getType() != "LIST";
        bool numGreaterThanSize = integerExpression > listToDiscard.asList().size();

        // Ensure that list is correct type and not empty
        if (listIsIncorrectType || listToDiscard.asList().empty() || numGreaterThanSize) {
            return DataValue(DataValue::RuleStatus::ERROR);
        }

        // auto& list = listToDiscard.asList();
        auto &list = const_cast<std::vector<DataValue> &>(listToDiscard.asList());
        for (size_t i = 0; i < integerExpression; ++i) {
            list.pop_back();
        }

        return DataValue(DataValue::RuleStatus::DONE);
    }

    std::unique_ptr<Rule> integer_expr_maker; // Some sort of (integer) expression Rule
    std::unique_ptr<Rule> list_maker;         // NameResolverRule
    int integerExpression;
    DataValue listToDiscard;
};

class UpfromRule : public Rule {
public:
    UpfromRule(std::unique_ptr<Rule> number_maker, int starting_value)
        : number_maker(std::move(number_maker)), starting_value(starting_value) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        auto ending_value_generic = number_maker->runBurst(name_resolver);

        std::cout << "ending_value_generic type: " << ending_value_generic.getType() << std::endl;
        std::cout << "ending_value_generic: " << ending_value_generic << std::endl;

        ending_value = ending_value_generic.asNumber();
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        // return empty list, not a decreasing list
        if (ending_value < starting_value) {
            return DataValue(std::vector<DataValue>());
        }
        // construct vector
        std::vector<DataValue> list_of_ints;
        int number_of_values = ending_value - starting_value;
        list_of_ints.reserve(number_of_values);
        for (int val = starting_value; val <= ending_value; val++) {
            list_of_ints.emplace_back(DataValue(val));
        }
        return DataValue(list_of_ints);
    }

    std::unique_ptr<Rule> number_maker;
    int ending_value;

    int starting_value;
};

// class InParallelRule : public Rule {
//     enum class StatementState { NOT_COMPLETED, COMPLETED };

// public:
//     InParallelRule(std::vector<Rule> &statements) {
//         for (auto &statement : statements) {
//             this->statements.emplace_back(statement, StatementState::NOT_COMPLETED);
//         }
//     }

// private:
//     void _handle_dependencies(NameResolver &name_resolver) override {
//         // Initialize all statements to be not completed
//         for (auto &statement : statements) {
//             statement.second = StatementState::NOT_COMPLETED;
//         }
//     }

//     DataValue _runBurst(NameResolver &name_resolver) override {
//         for (auto &statement_pair : statements) {
//             if (statement_pair.second == StatementState::COMPLETED) {
//                 continue;
//             }
//             // run the current statement, and check whether it finished
//             auto rule_state = statement_pair.first->runBurst(name_resolver);
//             if (rule_state.asRuleStatus() != DataValue::RuleStatus::NOTDONE) {
//                 statement_pair.second = StatementState::COMPLETED;
//             }
//         }
//         // check whether any statement are not completed, and if so, return NOTDONE
//         auto it = std::find_if(statements.begin(), statements.end(), [](const auto &statement_pair) {
//             return statement_pair.second == StatementState::NOT_COMPLETED;
//         });
//         if (it != statements.end()) {
//             return DataValue({DataValue::RuleStatus::NOTDONE});
//         }
//         return DataValue({DataValue::RuleStatus::DONE});
//     }

//     std::vector<std::pair<std::unique_ptr<Rule>, StatementState>> statements;
// };

// std::unique_ptr<Rule> temp = handleNameResolver();

//     "configurations.rounds"

class NameResolverRule : public Rule {
public:
    NameResolverRule(std::vector<std::string> &key) : search_keys(key) {}

private:
    void _handle_dependencies(NameResolver &name_resolver) override {
        if (search_keys.empty()) {
            std::cerr << "NameResolverRule called without arguments!" << std::endl;
        }
        std::cout << "search_keys size: " << search_keys.size() << std::endl;

        for (const auto &key : search_keys) {
            std::cout << "search_keys: " << key << std::endl;
        }

        isNested = search_keys.size() > 1;
    }

    DataValue _runBurst(NameResolver &name_resolver) override {
        if (search_keys.empty()) {
            return DataValue({DataValue::RuleStatus::DONE});
        }

        auto valueInTopScope = name_resolver.getValue(search_keys[0]);
        if (valueInTopScope == std::nullopt) {
            return DataValue("ERROR");
        }

        if (isNested && valueInTopScope->getType() == "ORDERED_MAP") {
            auto player = valueInTopScope->asOrderedMap();
            auto playerName = player["name"].asString();
            std::cout << playerName << std::endl;
            auto valueInMap = name_resolver.getNestedValue(search_keys);
            if (valueInMap == std::nullopt) {
                return DataValue("ERROR");
            }
            result = *valueInMap;
        } else {
            result = *valueInTopScope;
        }

        return result;
    }

    // This vector has the individual components of the desired value in order,
    // separated by its periods (ie. configuration.setup -> ["configuration", "setup"]) )
    std::vector<std::string> search_keys;
    bool isNested = false;
    DataValue result;
};

// class ContainsRule : public Rule {
// public:
//     ContainsRule(std::unique_ptr<Rule> list_maker, std::vector<std::string>& search_keys)
//         : list_maker{std::move(list_maker)}, search_keys(search_keys) {}

// private:
//     void _handle_dependencies(NameResolver &name_resolver) override {
//         list = list_maker->runBurst(name_resolver);
//     }

//     DataValue _runBurst(NameResolver &name_resolver) override {
//         if (list.getType() != "LIST") {
//             return DataValue("ERROR");
//         }

//         auto& list_elements = list.asList();
//         it auto std::find(list_elements.begin(), list_elements.end(),

//     }

//     std::unique_ptr<Rule> list_maker; // ElementsRule (returns a vector<DataValue>)
//     std::vector<std::string>& search_keys;
//     DataValue list;
// };
