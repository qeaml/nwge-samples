# Sample Snake app

A simple implementation of the classic 'Snake' game.

## Instructions

This package comes with a binary version of the game, `nwgesnake.exe` along with
the required `snake.bndl`, contains the game's resources.

Once in game, use the arrow keys to navigate. The rules of the snake game should
be already well-known to you. Have fun!

## Code structure

All of the definitions are present in [`snake.hpp`](snake.hpp). It is
recommended to read it before diving deeper into the code.

Each state defined in `snake.hpp` has its own implementation file. For example,
the [`Credits`](snake.hpp#L74) state is implemented in
[`credits.cpp`](credits.cpp).

Additionally the game uses some [common resources](snake.hpp#L15) shared between
states, implemented in [`common.cpp`](common.cpp)
