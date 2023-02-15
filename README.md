# Classic Roguelike Graphics API

WARNING: Currently in early development and still feature-incomplete. There will be breaking changes as development moves forward. While you are encouraged to try it out and tell me what you think, do not integrate it yet.

## Built for Roguelikes
CRGA is a graphics API designed for classic roguelike developers.

Classic roguelikes are defined by a focus on systems and simulation (including procedural generation), a grid-based layout, and other things depending on who you ask. Classic roguelikes often have very simple graphics to make room (both in development time and CPU cycles) for the systems and interactions that define the genre.

CRGA is built to help developers spend as little time as possible on drawing things on screen, and as much time adding features to the game. It offers a reasonable interface and minimal code to get up and running. It is designed to be useful throughout the development process, from the very beginning when you just want a few lines to get things on screen, to the end where you want the API to be using your personal memory manager and custom pipelines.

Want to get started? Read the [documentation](https://crga.readthedocs.io/).

## Focused on Graphics, Powered by Raylib
CRGA is a graphics API, and does not seek to provide utilities for other parts of rogelike development like procedural generation, path-finding, or AI. However, CRGA's integration with Raylib means the extensive toolset accessible through CRGA. 2d and 3d rendering tools, sound, and input are all available through Raylib!

## Easy and Fast
CRGA is designed to be quick easy to get up and running, but permits more and more control as you need it. It's not designed for beginners, but why should beginners be the only ones afforded easy to use API's?

## Development Status

CRGA is still in very early development. Expect breaking changes.

### Current Feature List
* Tile Rendering
* Entity Rendering
* Font Rendering with support for utf-8
* Tileset Rendering
* Movable camera with grid-snapping
* Layers
..* World and UI Layers
* Masking

### Planned Feature List
* Coloration, Color Palettes, and Tint
* Terminal Rendering
* UI Helper Functions
..* Rectangles
..* Lines of Text
..* Text Wrapping
..* Alignment/Anchors
* Shaders
* Tile Caching, Occlusion culling, and other rendering optimizations
* UI Elements
..* Scroll Window
..* Progress Bar
..* Radio Buttons
..* Tabs
..* Tree
* Support for both immediate and retain-mode rendering
* runtime loading and unloading of assets
* More options for doing the same/similar things
* Further simplification and ease-of-use improvements to the user-facing parts of the API

If there is anything you would like to see, or any problems you experience at all, please submit an issue here on github.
