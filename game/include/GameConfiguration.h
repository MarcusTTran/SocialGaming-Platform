// author: kwa132, mtt8

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#pragma once

class GameName {
private:
  const std::string &name;

public:
  // We will not allow an instance to be create without an input string
  GameName(const std::string &name) : name(name) {}
};

template <typename T> class SetupRules {
public:
  class SetupRuleKind {
    // Configuration kinds can be 'boolean', 'integer', 'string', 'enum',
    // 'question-answer', 'multiple-choice', 'json'.
    std::string name;
    SetupRuleKind(const std::string &name) : name(name) {}
    // TODO: create a validation function that checks if the Kind is valid (ie.
    // 'integer')
    // TODO: perhaps make this a template? Would that work?
  };

  SetupRuleKind kindGetter();
  std::string &promptGetter() const;
  std::optional<std::pair<int, int>> &rangeGetter() const;
  std::optional<std::map<std::string, std::string>> &choicesGetter() const;
  std::optional<std::string> &defaultsGetter() const;
  SetupRuleKind kind;
  std::string prompt;
  std::optional<std::pair<int, int>> range;
  std::optional<std::map<std::string, std::string>> choices;
  std::optional<std::string> defaults;
  // TODO: Perhaps add a default? It is used for range in professors example of
  // RPS
};

class GameConfiguration {
private:
  const GameName gameName;

  std::pair<size_t, size_t> playerRange;
  bool audience;
  // TODO: change
  std::vector<SetupRules> setup; // Contains a list of SetupRules (like the
                                 // number of rounds to start from)

public:
  ~GameConfiguration() = default;

  std::string getGameName() const;
  std::pair<size_t, size_t> getPlayerRange() const;
  bool hasAudience() const;
  std::vector<SetupRules> getSetup();
  void setupSetter(
      const SetupRuleKind &kind, const string &prompt,
      const std::optional<std::pair<int, int>> &range = {1, 10},
      const std::optional<std::map<std::string, std::string>> &choices = {},
      const std::optional<std::string> &defaults = "");
};
