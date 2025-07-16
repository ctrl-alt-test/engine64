# Ctrl-Alt-Test demo engine

## Background

This codebase includes a subset of the engine used by Ctrl-Alt-Test for making
64kB intros. This engine was used for prods like
[Immersion](https://www.ctrl-alt-test.fr/productions/h-immersion/), but it has
evolved since the last demo releases.

We are publishing this code with no guarantees, with the goal of helping other
developers. Any file or function may change in the future, without any backward
compatibility. Many parts of the codebase are not polished or documented; this
should improve in the future.

Feedback is welcome, discussions are encouraged.

## Overview

This project is a lightweight 3D engine designed for real-time graphics demos
that fit in 64kB (64k intros).

This engine is optimized for creating 64k intros. It's a C++ library that can
render realtime graphics using OpenGL, with a focus on binary size, performance,
and developer productivity. It is designed for minimal overhead while providing
flexibility during development.

### Code Structure

The `src` directory is the generic engine source code.

The engine is designed to be used as a library: you write your own `main`
function and you decide which parts of the engine you use. C++ compilers are
good at removing deadcode, so unused features shouldn't add any overhead.

The `demos` directory has an example of how to use the engine. `demos/example01/`
is a minimal executable to get a simple OpenGL setup. You can make a copy of the `example01/` directory and put the code of your demo there.

### Compilation modes

The code can be built in two different modes:
* Edition. This is an interactive mode, where you can use an ImGUI interface,
  control the camera, and control the time (e.g. to replay some parts, see the
  demo frame by frame, etc.). Live-coding is supported for textures and meshes.
  Other data is stored in JSON files (e.g. camera data) which is live-reloaded
  during the demo.
* Release. This creates a very compact and efficient binary, that includes only
  the features needed to play the demo.

Our approach is to generate C++ files during the Edition mode. These files can
later be included in the release build. JSON data is converted into C++
structures, shaders are minified, and the final executable is compressed (using
kkrunchy).

# Platform

The engine is developed primarily for Windows and the Microsoft Visual C++
compiler.

With a bit of work, most of the code should also build on Linux with Clang.
We have tried it in the past, but we haven't maintained the compatibility
and the cmake files are often outdated.
