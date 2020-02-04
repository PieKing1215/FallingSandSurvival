# FallingEverythingSurvival
2D survival game inspired by Noita and slightly Terraria<br>

![](preview_1.gif)<br>
*Preview (2/4/20)*

### Overview:<br>
Inspired by Noita and the Falling Everything Engine, I am replicating the engine in order to create a survival game.<br>
One of the main things is that I don’t want it to get stale like other survival games where there’s a fixed progression and repetitive world generation and items you have to craft to progress. I want it to be very procedurally generated.<br>
It’s vaguely like Terraria in that it’s a 2d side view survival.<br>
It’s more like Noita with its simulated physics, but less “chaotic”, since dying is somewhat more impactful.<br>

### Main ideas:
[Ideas Document](https://docs.google.com/document/d/1SOCFCpsvNiFs13mo8QgG-blD-eoXye1Jaay1aRuqXpI/edit?usp=sharing)
- Physics engine similar to Noita's Falling Everything Engine
- No traditional inventory: hold/wear/move things in the world
- Procedural world generation: including procedural materials, biomes, etc.
- Design/construct objects/tools/mechanisms

### Informational Resources / Inspiration:<br>
[Noita](https://noitagame.com/)<br>
[Exploring the Tech and Design of Noita](https://www.youtube.com/watch?v=prXuyMCgbTc)<br>
[How to Add Infinite Features into Minecraft (with one update)](https://www.youtube.com/watch?v=CS5DQVSp058)<br>

### Tech<br>
Written in c++<br>
Uses [SDL](https://www.libsdl.org/) for rendering<br>
Uses [Box2D](https://box2d.org/) for rigidbody physics<br>
Rigidbody mesh generation uses a modified [Marching Squares implementation](https://github.com/reunanen/cpp-marching-squares) along with a modified Douglas Peucker implementation and feeds it into [polypartition](https://github.com/ivanfratric/polypartition) for triangulation.<br>
Uses [FastNoise](https://github.com/Auburns/FastNoise) for Perlin/Simplex/Cellular noise<br>
Uses [FMOD](https://fmod.com/) for audio.

See the [Trello](https://trello.com/b/JCKJ65yP/falling-everything-survival) to get an idea of my progress.
