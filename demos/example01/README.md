# Example01

A minimal example of the Ctrl-Alt-Test demo engine.

## Overview

This minimal example shows how to setup an OpenGL window that compiles
to a small binary size. While the size is not as small as with 1kB or
4kB intros, it is small enough for 64kB intros.

The project shows:
- Build options used (in Visual Studio) to generate a small binary for
  size-coding.
- How to setup an OpenGL window and rendering loop.
- A suggestion of how to plug a demo entry point to the low level event
  loop.

When it is run, the example opens an empty window with an animated
background.
