# AF\_Lib

Atom Forge Library (AF\_Lib). A simple C library to help with game development.

[License: GPL-3.0 license](https://opensource.org/license/gpl-3-0)

## 📖 About The Project

This project is a small, foundational C library created for hobby game development. It provides a collection of essential systems needed to build simple games. This library is the core engine component utilised by the [AF\_Editor](https://github.com/jhallDevelop/AF_Editor).

Key features include:

*   Simple Rendering Engine (OpenGL 3.x)
*   Entity Component System (ECS) architecture
*   Scene and Project serialization using [cJSON](https://github.com/DaveGamble/cJSON)
*   Basic custom Input, Physics, and Audio systems
*   Build using Make

---

## 🚀 Getting Started

Follow these instructions to get a copy of the library up and running on your local machine for development and testing.

### Installation

**Clone the repository:**

(Alternatively, for private collaboration, or contact `jhall.develop@gmail.com` to be added as a contributor.)

```
git clone --recurse-submodules https://github.com/jhallDevelop/AF_Lib.git
```

Ensure recursive clone to ensure submodules are downloaded.

**Build the library (OSX/Linux):**

*   Navigate to the cloned directory and run `make`.

**Build the library (Windows):**

*   Open the project in Visual Studio and build the solution.

### Prerequisites

*   **Git** (for cloning the repository).
*   **Make** (for build project).

# OSX/Linux

*   C & C++ compiler supporting C99 & C++ 17 (like GCC or Clang)
*   Make
*   Check that the following dependencies have also been cloned; if not, please download them.
    *   [assimp](https://github.com/assimp/assimp)
    *   [cJSON](https://github.com/DaveGamble/cJSON)
    *   [stb](https://github.com/nothings/stb)
    *   [glew](https://glew.sourceforge.net/)
    *   [glfw](https://www.glfw.org/)

# Windows

*   Visual Studio 2022 with C++ dependencies installed (Desktop Development with C++).

---

## ✨ Features

This library includes the following main features:

### Renderer

*   A simple rendering engine supporting multiple graphics APIs.
    *   **OpenGL**: Model loading, texturing, basic lighting, and shadows using OpenGL 3.x.
    *   **DirectX11**: (WIP)
    *   **Vulkan**: (Planned for the future)

### Core Systems

*   **ECS**: A basic Entity Component System for managing game objects and their behaviors.
*   **Scene Management**: Functions for loading, saving, and managing game scenes.
*   **Input Handling**: A system for processing keyboard, mouse, and controller inputs.
*   **Physics & Collision**: Simple physics and collision detection utilities.
*   **Audio**: Basic audio playback functionality.

### Multi-Platform Support

*   Designed to be compiled and run on various platforms.
    *   OSX
    *   Linux
    *   Windows (WIP)

---

## 🛠️ Usage

To use `AF_Lib`, include the main header `AF_Lib_API.h` in your project and link against the compiled library file. The library provides a set of functions to initialize the engine, manage the game loop, and render scenes.

Example usage can be found in the `AF_Editor` project which uses this library as its core.

---

## ❓ FAQ

**"I compiled the project but I'm getting linker errors."**

*   **Check Dependencies:** Ensure that all required libraries like `glew`, `glfw`, etc., are correctly linked in your build process. The `CMakeLists.txt` file should handle this, but manual linking might be necessary in some environments.

---

## 🌿 Branches

This repository uses the following branches:

*   `**main**`: Main working branch that should contain a tested version of the AF\_Lib project. Report bugs in issues.
*   `**dev/windows**`: WIP branch for Windows, DirectX and other Windows-related development.

---

## 🤝 Contributing

This is a private personal project for now; however, I am open to contributions and feedback.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".  
Don't forget to give the project a star! Thanks again!

1.  Fork the Project
2.  Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3.  Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4.  Push to the Branch (`git push origin feature/AmazingFeature`)
5.  Open a Pull Request

---

## 📜 License

Distributed under the GPL-3.0 license License. See `LICENSE` file for more information.

---

## 📞 Contact

Josh - \[jhall.develop@gmail.com\]

Project Link: [https://github.com/jhallDevelop/AF_Lib](https://github.com/jhallDevelop/AF_Lib)

---

## 🙏 Acknowledgements

Special thanks to all the great online resources and books that have inspired and help me develop this framework.

*   Rendering: 
    *   [LearnOpenGL - Joey de Vries](https://learnopengl.com/)
    *   [OGLdev - Etay Meiri](https://www.ogldev.org/index.html)
*   Physics:
    *   [Game Engineering (Masters) Course - Newcastle University](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/)
    *   [Realtime Collision Detection - Christer Ericson](https://realtimecollisiondetection.net/)
*   General Engine Architecture
    *   [Game Engine Architecture - Jason Gregory](https://www.gameenginebook.com/)
    *   [Game Coding Complete - Mike McShaffry](https://www.google.com.au/books/edition/Game_Coding_Complete/ja6JPwAACAAJ?hl=en)
    *   [COMP 4300 - Intro to Game Programming - David Churchill (Memorial University of Newfoundland)](https://www.cs.mun.ca/~dchurchill/teaching/)  
        \`\`\`\`

---

## ⚠️ Project Status

This project is currently **unreleased** and in active development. It has been made public to share my progress, gather feedback, and serve as a reference for others.

Please be aware that:

*   You may encounter bugs, errors, or problems with installation.
*   The project is primarily used for my personal game projects and game jams.

I am open to receiving feedback, bug reports, and pull requests for fixes. Your contributions are welcome!