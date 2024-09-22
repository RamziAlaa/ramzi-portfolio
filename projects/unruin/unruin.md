# un»ruin

> You can find the full code for the project in `/projects/unruin/cpp-code/`

![gif](../../.md-media/ezgif-3-39ef053b3b.gif)

*un»ruin* is a very early project I'm working on, running in the Godot engine. A majority of the code is written in C++ (via GDExtension) rather than GDScript or C#. Unlike the latter languages, coding a Godot game in C++ requires much more experience with both the language and the engine itself.

There are two parts of this project I would like to talk about: **Smart Properties,** and the **Player Controller.**

## Smart Properties

In standard GDScript and C#, you can expose a variable to Godot's editor with the following code:

```bash
# GDScript
@export var player_speed: int = 5 
```

```c#
// C#
[Export]
public int PlayerSpeed = 5;
```

However, in C++, it's much more complicated due to how GDExtension works. Since C++ is a lower level language that the engine *links* rather than *parses,* it takes more effort to bind variables.

```cpp
// C++

// Declare a variable
int player_speed = 5;

// ...Now declare a *public* getter and *public* setter
public:
void set_player_speed(int p_player_speed){player_speed = p_player_speed;}
int get_player_speed(int p_player_speed) const {return player_speed;}

// And now bind all the functions and set the inspector stuff
protected:
void _bind_methods()
{
    // About 2 lines of function calls for method binding.
}
```

I had implemented a way to complete the process with an efficient, simple macro system, usable anytime through the `smart_properties.hpp` header I made.

```cpp
//C++

//Declare a variable;
int player_speed = 5;

//And thats it, just put it in the macro list;

SP_BEGIN_PROPERTIES
    SP_PROPERTY(player_speed, PropertyInfo(Variant::INT, "player_speed"))
SP_END_PROPERTIES
```

The macros use lambda functions to create the getters and setters, privatizing them and letting the editor access them through the implementable `_get` and `_set` functions. This had greatly increases productivity and sanity when working on Godot games in a language I love.

## The Player Controller

Player movement is delegated to the class `PlayerMovement`, a state machine that neatly manages input handling and movement states. Many player controllers become a cluttered mess as a project goes on. This one doesn't.

Putting it simply, there are movement states in `PlayerMovement` that are contained as nested classes. Only `PlayerMovement` can access them, enforcing delegation.

```cpp
class MovementState
{
public:
    MovementState(PlayerMovement *p_player_movement);

protected:
    PlayerMovement *pm;

    void apply_gravity(double delta);

public:
    virtual MovementType get_type() const = 0;
    virtual void enter(){};
    virtual void exit(){};
    virtual void get_input(const Ref<InputEvent> &event){};
    virtual void process_movement(double delta) = 0;
};

lass DeadState : public MovementState
	{
	public:
		MovementType get_type() const { return MovementType::Dead; };
		void process_movement(double delta);
	};

	class NormalState : public MovementState { /* ... */ };

	// Functionally the same as NormalState, but other states
	// need to account for the halved height when transitioning.
	class CrouchingState : public MovementState { /* ... */ };

	class RollingState : public MovementState { /* ... */ };

	class SlidingState : public MovementState { /* ... */ };
```

States can transition to another state at anytime. During `exit()`, they are able to view the next state waiting to be entered and can transition accordingly.

```cpp
template <typename T>
void queue_movement_state();

MovementState *queued_state = nullptr;
```

You can view the entire state machine in `projects/unruin/cpp-code/classes/player_movement.h` and `projects/unruin/cpp-code/classes/player_movement.cpp`.