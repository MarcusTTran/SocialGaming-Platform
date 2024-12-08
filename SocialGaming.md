# SocialGaming scripts

All game scripts are stored in the [config](./config/) folder.

Example scripts:
- [Empty](./config/empty.game)
- [Minimal](./config/minimal.game) (prints output to all players)
- [Rock Paper Scissors](./config/rockPaperScissors.game) (full game)

## Data and Variables

All game-time information is of one of the following forms:
- Bool
- Int
- String
- List (of any type)
- Map (of String to any type)

## Structure

An empty game looks like this:
```
configuration {
  name: ""
  player range: (0, 0)
  audience: false
  setup: {}
}
constants {}
variables {}
per-player {}
per-audience {}
rules {}
```

If this were saved to a file in the `config` top-level folder, it will be selectable to play, and upon starting will immediately end.

# Raw Specification (needs re-writing)

## Configuration

The configuration of a game controls general settings that may assist in game creation and game management for the owner. This includes such things as (1) the name of the game type to show when selecting the type of game to create (2) specifying any data that may be uploaded by the owner, e.g. questions and answers for quiz based games, (3) specifying minima and maxima for the number of players as a closed range, and (4) specifying whether additional game joiners beyond the players can participate as audience members.

"setup" defines settings that can or must be configured by the owner of the game upon game creation. This can include such things as the number of rounds to play. For example, I might define a numerical configuration followed by boolean using:
```
num_rounds {
  kind: integer,
  prompt: "Number of rounds to play",
  range: (1, 10),
  default: 3,
}
public_voting {
  kind: boolean,
  prompt: "Player votes are public",
}
```
Notice that num_rounds has a default value and need not be set explicitly.

Configuration kinds can be 'boolean', 'integer', 'string', 'enum', 'question-answer', 'multiple-choice', 'json'. The first three are straightforward. An enum requires an owner to select an option from a list of options with descriptions for the user:
```
mode {
  kind: enum,
  prompt: "Game Style",
  choices: {
    'fast':     'A quick round with friends',
    'standard': 'Standard play',
    'long':     'A marathon battle against former friends',
  }
}
```
The latter three kinds indicate custom JSON file formats or general JSON files whose data can be uploaded by the game owner upon creation.

### Constants, Variables, Players, and the Audience

The constants, variables, per-player, and per-audience sections define state that may be accessed by the game rules during play. The constants and variables sections contain maps from names to values. The per-player and per-audience sections define maps that exist for each player of a game and each audience member. Top level names must be unique. Values may themselves be (1) maps from names to values, (2) lists of values, or (3) literal strings, numbers, or booleans. In the initial version of the language, new top level names/keys within these sections cannot be added by rules. They must be predetermined within the specification.

Note that some additional forms of game state must be accessible via rules, as well. In particular, the "players" name specifies a list of the player data maps, and the "audience" name specifies a list of audience data maps. Every player map also implicitly has a "name" key that maps to the unique name that the player selected upon joining the game.

### Rules

Most of the actual game behavior is controlled by the rules section and the semantics of the rules therein. Essentially, these rules define a custom embedded domain specific language for actions that may be taken within a game. Note that it may be possible to validate some basic correctness properties of rules. These rules are subject to change and will be refined via discussion. They do, however, provide an appropriate jumping off point for planning and designing a good solution.

Lists of rules define a sequence of operations in which each rule must be performed in sequential order. Note that apparent parallelism will exist, but no use of explicit parallelism or threads is required as a part of this project. Indeed, I heavily encourage avoiding the use of threads for implementing the interpretation of rules. The different forms of rules are explored within the remaining sections of this document.

In descriptions of the rules below, double angle brackets indicate a description of the content at that place in a rule, for instance << fresh variable >> indicates creating a new variable at a particular location in a rule. Optional syntax will typically be designated with square brackets like [ extra option ].

## Control Structures

Control structures determine when and how different groups of instructions may execute. This includes concepts like sequencing, branching, and parallelism. Note that parallelism in this project is only from the perspective of the players. No parallel control structures actually require hardware level parallelism in software design or concurrency level primitives like threads.

### For (each element of a list)
```
  for << fresh variable >> in << expression evaluating to a list >> {
    << statement list >>
  }
```

### General looping
```
  while << boolean expression >> {
    << statement list >>
  }
```

### Parallel for (each)
```
  parallel for << fresh variable >> in << expression evaluating to a list >> {
    << statement list >>
  }
```

Executes the list of statements once for each element in the list, but it does not wait for the processing of a previous element to finish before starting the next element of the list.

### Inparallel
```
  in parallel {
    << statement list >>
  }
```

This starts executing each statement in a list without waiting for the previous statements in the list to finish. Note that this creates the potential for bugs even in a single threaded scenario depending on your design. It may be refined in the future.

### Match
```
  match << expression >> {
    << expression candidate 1 >> => { << statement list >> }
    << expression candidate 2 >> => { << statement list >> }
    ...
  }
```

## List Operations

List operations define actions on lists. In some cases, an operation may require a named list variable like constants.cards. In other cases, they may simply use list expressions.

### Extend
```
extend << named list >> with << list expression >> ;
```

This appends the contents of the expression to the named list.

### Reverse
```
reverse << named list >> ;
```

### Shuffle
```
shuffle << named list >> ;
```

### Sort
```
sort << named list >> [by << key>>];
```

By default, sorting is in ascending order. If a key is provided, then elements of the list will be treated as maps, and the key will select an entry of the map to use for sorting.

### Deal
```
deal << integer expression >> to << player list >> from << list expression >>;
```

The list of all players can be specified via the shorthand all.

### Discard
```
discard << integer expression >> from << named list >>;
```

## Timing

### Timer
```
time for << integer expression >> [ << "at most" or "exactly"] [in << named variable ] {
  << statement list >>
}
```

A timing block sets a timer in seconds for a list of statements.

If the at most mode is used, then the list of statements will stop executing after time expires. If the exactly mode is used, then the list of statements will both stop executing at the given time and pad the time taken to fill the expected duration.

If in << named variable >> is specified then the given variable will be set to false before the timer has expired and true after the timer has expired.

## Human Input

### Input choice
```
  input choice to << player >> {
    prompt: << prompt string >>
    choices: << list expression >>
    target: << named variable in which to store the selection >>
    [timeout: << integer expression >>]
  }
```
This allows the player to make a multiple choice selection.

### Input text
```
  input text to << player >> {
    prompt: << prompt string >>
    target: << named variable in which to store the selection >>
    [timeout: << integer expression >>]
  }
```

### Input range
```
  input range to << player >> {
    prompt: << prompt string >>
    range: (<< minimum value >>, << maximum value >>)
    target: << named variable in which to store the selection >>
    [timeout: << integer expression >>]
  }
```

### Input vote
```
  input vote to << player >> {
    prompt: << prompt string >>
    choices: << list expression >>
    target: << named variable in which to store the selection >>
    [timeout: << integer expression >>]
  }
```

## Output

### Message
```
message << player list >> << string expression >>;
```

all can be used as a shorthand for the list of all players.

### Scores
```
scores << list of keys >>;
```

Prints a scoreboard on the global display using the given attribute(s) of each player defined by the key list.

## Variables and Expressions

Standard arithmetic operations apply. They may even apply to types other than integers! Literals for lists and maps can be defined in a style similar to JSON.

### Assignment

Assignment uses the <- or "stores to" operator, e.g.
```
x <- [ "a", "literal", "list" ];
```

### Numerical Attributes

Given a number, I can access attributes of the number similar to attributes of a list. For instance, I can treat a number as a list by saying "count.upfrom(1)". That defines the list of numbers 1, 2, …, count.

### List Attributes

When given a list name, attributes of a list can be referred to and elements of a list can be sliced to represent new lists. For example, suppose that the following list is defined in the constants:
```
  roles: [
    { name: "Duke", action: "Tax. The duke may take 3 coins from the treasury."},
    { name: "Assassin", action: "Assassinate. The assassin may force another player to give up an influence."},
    ...
  ]
```

The name of the list is roles and can be referred to within the rules. The size of the list may be accessed through the attribute roles.size(). Lists may be implicitly created, as well. If the elements of the list are maps, then the keys of the maps define additional lists. For instance, roles.elements.name defines the list of names contained within the above list. Additional useful attributes are contains and collect that perform "any of" checks and "filtering" respectively.