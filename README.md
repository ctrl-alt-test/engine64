# Ctrl-Alt-Test demo engine

## Background

This codebase includes the engine used by Ctrl-Alt-Test for making 64kB intros.
This engine was used for prods like
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

The `demos` directory has examples of how to use the engine. `demos/example01/`
is a minimal executable to get a simple OpenGL setup. `demos/example02/` has
more features, e.g. mesh, shaders, animations.

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

## Build in Edition mode

* Use [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/).
* Select the `DebugEdit` configuration.
* Select the project you want to build:
  * Each demo is a separate project.
  * `Example01` and `Example02` are minimalist examples.
  * `MeshPlayground` can be used for playing with meshes and textures.
* Open the project `Properties`. Under `Debugging`, set the value of `Working Directory` to `../..`.
  * If you forget this step, you'll get runtime errors (files not found).
* Now, you can run the demo.

## Build for Release

* Run the demo in `DebugEdit` mode and close it after the loading screen.
  * When the editor mode is running, the demo generates a bunch of `exported_`
    files under `demos/<name>/src/`. These files are used only for the release build.
* Select the `Release` configuration and build.
* The executable is automatically compressed with kkrunchy, and the Output pane
  will you tell the file size.


## Editor

### Player

Controls are defined in `DebugControls.cpp`.

Time:

* `Space`: pause
* Right/left arrows: Move forward/backward in time. By default, the increment is
  1s; use `ctrl` to move by 10s; use `shift` to move by 10ms.

Camera:

* `Q`: toggle between the automatic camera and the manual camera.
* `F`: copy the automatic camera into the manual camera.
  * You'll often want to press `F` after doing `Q` (or when you're lost).
* `Shift-F`: go to the beginning of the automatic shot.
* `Ctrl-F`: go to the end of the automatic shot.
* `Z`/`X`: change the FOV.
* To move the manual camera:
  * Do a drag & drop with the left, the middle, or the right button. They all do
  something different.
  * `WASD` keys can also be used.

Many of the controls support the modifiers `shift` and `ctrl` for a more precise
or a faster change.

### Camera

The camera is controlled by a file `asset/<demo>.shots.json`. This file is
converted by the editor. A more compact version is used for the release.

When the json file is modified, it is instantly reloaded.
