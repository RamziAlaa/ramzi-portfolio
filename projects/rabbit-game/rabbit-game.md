# Hare

> You can look in the subrepository `/projects/rabbit-game/Hare-Senior-Project` if you want to poke around with the codebase. The executable is in the *"Releases"* section in the Github repo.

Hare was my senior project back in high school. It's a game where you move a square in a level. There's no goal, but you can have fun moving very fast.

As for the *engine* it's running on, it's *completely made from the ground up.* It uses [SDL2](https://www.libsdl.org/) as a rendering engine, but that's about it when it comes to third-party libraries.

## Original Premise

Hare was supposed to be a 2D action game with destructive environments powered by cellular-automata. The player was supposed to play as a rabbit that destroyed an oppressive machine-dominated regime. The plan was to create a story-heavy game with complex math and design in the span of 2 months, as well as an **engine from scratch.**

As you might expect, this was an over-ambitious project that couldn't be completed in time. Despite everything, my teacher had told me that "this was the most complex and advanced projects he had received in all his time teaching."

## The Engine

At a glance, the *Hare Engine* has the following capabilities:

- Sprite Rendering
- World Simulation
  - Frame based updating. (Mostly for rendering.)
  - Tick based updating. (Mostly for physics and game logic.)
  - State machine for application handling.
- Collision
- Input Handling

Although rudimentary, I was told that this project was quite feature rich given the amount of time I was given. I was even told that I could follow up on this project, although I never followed up on it.

The entire project was made in C++. Being a high school student at the time, I had learned the following lessons about making anything with such a language:

- Check for memory leaks.
- Have every class serve a purpose.
- Don't have multiple classes serve a similar purpose.

## The `main` Function

The `main` function is responsible for the creation and handling of the `Application` class, updating the application in a `while` loop. Delta time and basic frame management is handled in the same `while` loop.

```cpp
// === Main.cpp ===

while (app.gameState != GameState::Closing)
{
    Mouse::UpdateMouse();
    Timer::UpdateFixedDeltaTime();

    //Get ticks right now to calculate Delta Time for framerate
    Uint64 currentTicks = SDL_GetTicks();
    app.UpdatePhysics();

    if (currentTicks > startTicks + ticksPerFrame)
    {
        app.ticks++;
        app.UpdateFixed();
        
        startTicks = currentTicks;
    }

    timeTillNextRender += Timer::fixedDt;

    // [Game updating code]
    // [Game rendering code]
}
```

<!-- There's a recurring theme in my early code where the same code is copied. This is evident in the end of the `main` function's `while` loop.

```cpp
if (!app.isCapped || Timer::fps == 0)
{
    // First instance of code
    Timer::UpdateDeltaTime();
    timeTillNextRender = 0;
    app.Update();
    app.Render();
}

else if (app.isCapped && Timer::fps != 0)
{
    if (timeTillNextRender >= (double)((double)(1000 / Timer::fps) / 1000))
    {
        // Same Code Again
        Timer::UpdateDeltaTime();
        timeTillNextRender = 0;
        app.Update();
        app.Render();
    }
}
```

It's typically more efficient for both the programmer **and** the program to not repeat code like this. -->

## Structure of The Engine

Below is a general overview of the structure of the engine. Static classes are outlined with dashes:

![diagram](../../.md-media/hare-diagram-portfolio.png)

Every class below `PlayState` is used for game logic. `PlayState` requires `TextureManager` to render its sprites, `TextureManager` needs `Application`, etc.

## Physics and the Player Controller

`PlayerController` preforms as its name suggests. It handles pushing the `Player` class around.

`Entity`, and `Player` by extension, handles the physics and collision every game tick.

```cpp
// virtual Entity::Update()

if (health < 0)								//If the entities health is 0...
    this->Die();							//Kill the entity.

//Speed calculations and conditions
if (isGrounded)
{
    if (speed > 0)
        speed -= abs(speed / 8);
    else if (speed < 0)
        speed += abs(speed / 8);
}

if (!isGrounded)
{
    if (speed > 0)
        speed -= abs(speed / 16);
    else if (speed < 0)
        speed += abs(speed / 16);
}

if (this->isAffectedByGravity)
{
    //Gravity calculations and conditions
    gravity += 1;

    if (gravity > 100)
        gravity = 100;
    else if (gravity < -100)
        gravity = -100;
}

pos.x += speed;
pos.y += gravity;

UpdateHitbox();

if (entityFlags & ENTITYSTATE_DEAD)			//If the entity is dead...
    return;									//Don't run the rest of the method.
```

Evidently, the collision system for entities is incredibly simple. Only a handful of conditional statements and math is executed every tick. I had gone through great effort to make sure the collision logic hadn't 