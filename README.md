# FallingSandSurvival
2D survival game inspired by Noita and slightly Terraria<br>

![](preview_1.gif)<br>
*Preview (2/4/20)*

## Table of contents<br>
- [Overview](#overview)
- [Main ideas](#main-ideas)
- [Resources / References / Inspiration](#resources--references--inspiration)
- [Progress & Feature Requests](#progress--feature-requests)
- [Tech](#tech)
- [Other notes](#other-notes)
- [More Gifs](#more-gifs)

## Overview:<br>
Inspired by Noita and the Falling Everything Engine, I am replicating the engine in order to create a survival game.<br><br>
It’s vaguely like Terraria in that it’s a 2d side view survival.<br>
It’s more like Noita with its simulated physics, but less “chaotic”, since dying is somewhat more impactful.<br><br>
One of the main things is that I don’t want it to get stale like other survival games where there’s a fixed progression and repetitive world generation and items you have to craft to progress. I want it to be very procedurally generated.<br>

## Main ideas:
[Ideas Document](https://docs.google.com/document/d/1SOCFCpsvNiFs13mo8QgG-blD-eoXye1Jaay1aRuqXpI/edit?usp=sharing)
- Physics engine similar to Nolla Games' Falling Everything Engine
- No traditional inventory: hold/wear/move things in the world
- Procedural world generation: including procedural materials, biomes, etc.
- Design/construct objects/tools/mechanisms

## Resources / References / Inspiration:<br>
[Noita](https://noitagame.com/) / [Falling Everything Engine](https://nollagames.com/fallingeverything/)<br>
[Exploring the Tech and Design of Noita](https://www.youtube.com/watch?v=prXuyMCgbTc)<br>
[How to Add Infinite Features into Minecraft (with one update)](https://www.youtube.com/watch?v=CS5DQVSp058)<br>

## Progress & Feature Requests<br>
See the [Trello](https://trello.com/b/JCKJ65yP/falling-sand-survival) to get an idea of my progress.<br>
Please use the [GitHub issues](https://github.com/PieKing1215/FallingSandSurvival/issues) page for feature requests.

## Tech<br>
Written in C++<br>
Uses [SDL](https://www.libsdl.org/) for rendering<br>
Uses [Box2D](https://box2d.org/) for rigidbody physics<br>
Mesh generation uses a combination of Marching Squares, Douglas Peucker and [polypartition](https://github.com/ivanfratric/polypartition).<br>
Uses [FastNoise](https://github.com/Auburns/FastNoise) for Perlin/Simplex/Cellular noise<br>
Uses [FMOD](https://fmod.com/) for audio.

## Other notes<br>
I am not personally affiliated with Nolla Games, nor has Nolla Games endorsed this project.<br>
Noita and Falling Everything Engine are © Nolla Games.<br>
Any textures that very closely resemble Noita's are temporary and will change in the future.<br>

## More Gifs<br>
![](preview_2.gif)<br>
*Containers and temperature (2/10/20)*
![](preview_3.gif)<br>
*Chisel tool (WIP) (2/27/20)*
