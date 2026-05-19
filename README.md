# Simple Game Engine

A lightweight, 2D game engine heavily inspired by [The Cherno's Hazel Engine](https://github.com/thecherno/hazel).

This project is architected with modularity in mind, cleanly separating platform-specific windowing and rendering from the core game logic.

## Architecture

The engine is fundamentally split into two distinct parts:
*   **The Window:** Handles all platform-level operations, including rendering and capturing raw user input.
*   **The Core:** Manages the actual game logic, state, and interaction.

The Window does not directly access the Core. Instead, it communicates input updates via **event callbacks**.

## Usage

To build a game or application with this engine, your primary workflow revolves around creating and managing custom layers.

### 1. Creating Layers
You implement your game logic by creating layers. Each layer requires you to set up your event dispatchers and define three core methods:
*   `onUpdate(dt)`: Handles per-frame logic and state changes, updated using a delta time (`dt`).
*   `onEvent(event)`: Receives and processes incoming input/window events.
*   `onRender()`: Handles all drawing operations for that specific layer.

### 2. Layer Communication
Every layer holds a pointer to the Core. This pointer serves as the primary bridge for layers to communicate with the engine and access shared resources.

### 3. Pushing to the Stack
Once your custom layer is created, you push it onto the engine's Layer Stack. The Core takes over from there, automatically calling your update, render, and event methods based on the stack hierarchy.

### 4. The Main Loop
The main game loop is housed entirely within the Core, abstracting the heavy lifting away from the user. The loop runs automatically; the only configuration you can (and need to) modify is the **target FPS**.

## Layer Stack System

The core logic operates on a **Layer Stack Architecture**.
*   **Logic Updates:** Processed from **Top to Bottom** (topmost layers get priority for consuming events).
*   **Rendering:** Processed from **Bottom to Top** (backgrounds draw first, UI draws last).

**Current Constraints & Features:**
*   Only **one** instance of a specific layer class type is permitted on the stack at any given time.
*   Layers can be manipulated using four primary methods: `push()`, `focus()`, `get()`, and `delete()`.

## Event System

Raw inputs are captured by the Window, converted into standard events, and sent to the Core. These events are decoded at the Layer level using an `EventDispatcher`.

Supported events include:
*   **Keyboard:** Key Press, Key Release
*   **Mouse:** Mouse Move, Mouse Button Press, Mouse Button Release
*   **Window:** Window Resize

## UI Components

The engine features a built-in, highly flexible UI system designed to work **without the need for inheritance**. You can fully customize these objects—from their physical shape to their behavioral callbacks.

### Clickables
Interactive UI elements that respond to various mouse states. You can bind custom actions to the following states:
*   `Hover`
*   `Click`
*   `Held`
*   `Click Released`
*   `Click Canceled`

### Draggables
An extension of the `Clickable` concept. They share all the same customizable properties and states but include built-in logic for click-and-drag functionality. Holding a click on a `Draggable` and moving the mouse will automatically translate the object's position across the screen.