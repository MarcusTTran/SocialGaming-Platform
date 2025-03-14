# Social Gaming Platform

> Based on work in SFU's CMPT373 course on Software Development Methods in Fall 2024.

A platform for developing and playing social games similar to game collections like the Jackbox Party Pack or social games like Coup, Werewolf, Loveletter, or Citadels. 
It allows developers to easily and simply write social games in a simple scripting language called SocialGaming. 

This repo runs a game server that users connect to over the internet to play games written in the custom scripting language that are interpreted at runtime. 

When a user connects to the game server, they may either join a game or choose a new game amongst those offered. When a new game is created, it may be configured by the creating player, who is designated as the "admin" of the game session. This is analogous to the main screen in Jackbox, and is not a player. The admin will receive an invite code to share with players, who may join the game by specifying the invite code. 

## Playing a game

You may imagine that a typical session works as follows, with this game server already running: 
- Several friends get together and decide to play a game. 
- One person connects to the game server via their computer and selects a game. 
- The game runs on that main computer, with the primary screen observable by all players. 
- Separately, each player connects to the server and joins the game on their phones via invite link. 
  - Each individual player can interact with the game solely through their phones. 
- Any player specific instructions or actions will be sent to specific players on their phones. 
- Global information will be displayed to all players on the main game screen. 

## Contributors

- Jason Spence
- Kuan Ming Wu
- Nicholas Tait 
- Michael Best
- Marcus Tran
- David Mulej

## Building the Game Server

In the commandline from the home directory:

### First time only

Make a build directory
``` bash
mkdir build
cd build
```

Generate CMake files
``` bash
cmake ../
```

### Every time

Build the executables (the number after `-j` specifies the number of threads to run in parallel)
``` bash
make -j12
```

Run the server (the number refers to the port where the server will be hosted)
``` bash
./bin/gameserver 8000 ../server/webchat.html
```

Access the server from local browsers
```
http://localhost:8000/
```

## Technologies

- [C++](https://isocpp.org/)
- [cmake](http://www.cmake.org/) for build management
- JSON for server configuration (not implemented)
- [SocialGaming](https://github.com/nsumner/tree-sitter-socialgaming) for scripting games
- [Treesitter](https://github.com/nsumner/cpp-tree-sitter) parsing interface
- [Chatroom Interface](https://github.com/nsumner/web-socket-networking) 
- [googletest](https://code.google.com/p/googletest/) for testing
- [spdlog](https://github.com/gabime/spdlog) for logging (not implemented)
- [Sphinx](http://sphinx-doc.org/) for generating documentation (not implemented)

## SocialGaming scripts

See the [SocialGaming README](./SocialGaming.md)

## Development Details

### Using Linux in Windows

#### Connecting to CSIL at SFU

A special Python virtual environment can be used to automatically enable and disable access to additional software by default that does not come by default with a CSIL connection.

To start the virtual environment from within CSIL, run:

`source /usr/shared/CMPT/faculty/wsumner/base/env373/bin/activate`

This activates the underlying virtual environment thats makes more recent software for the game platform available. You can exit the virtual environment by running:

`deactivate`

You can add the source line to the end of your ~/.bashrc configuration file to enable the environment automatically when you log in.

#### Working within Docker

A docker container provides the required versions of all software that are used in this project. It is possible to connect IDEs like VS Code to an appropriate docker image.

To pull the latest version of the docker image for the class, you can use:

`docker pull nsumner/cmpt373:fall2024`

You can start a container with this image and access files on your filesystem using a command like:

`docker run -ti --name=socialGame --ulimit='stack=-1:-1' -v << path on your system >>:/opt/work nsumner/cmpt373:fall2024`

For instance, this maps the given `<< path on your system >>` to the location `/opt/work` in the container.

If you exit from the container, you can restart it again later with:

`docker start -ai socialGame`

### Data and Variables

All game-time information is of one of the following forms:
- Bool
- Int
- String
- List (of any type)
- Map (of String to any type)

When the game is running, data is stored in a map accessed via the `NameResolver`

### Phases of code

1. Compile
   - Static code, type checking, etc.
1. Run
   - Dynamic code execution, hosting the server
1. Parse
   - Reading in a SocialGaming specification and constructing a tree of Rules
1. Execute
   - Running the rules, handling calculations as they arise
