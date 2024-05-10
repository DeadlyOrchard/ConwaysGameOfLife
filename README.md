## Conway's Game Of Life
- I first came across this game when I had discovered Python and Pygame. I had just started programming, and wanted to see if I could implement it.
- Of course, I failed hilariously, but it was quite the thought experiement!
- A few days ago, a peer of mine, [Luke](https://github.com/Dysax) implemented their own in Golang using the terminal, I thought I'd have a go at it.
### Tools
- I just happened to be working with [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2) for 2D game development so that was no question.
- I had also recently discovered [Flecs](https://github.com/SanderMertens/flecs) and was considering as an option for a game I've thought of.
### Build Process
- This was my first time using a framework for C++ ( as opposed to a library ), so linking everything together was quite a pain.
  - Luckily, I have the internet on my side. I saw suggestions to use CMake or Make or Bake and all of these toolsets. Personally, this is a learning experience, so I sought to learn more about the compiler instead of letting a program do it for me.
  - This is a 3-step build process. Hopefully I've made it simple.
    - **TODO**: write python script to generate .bat for this.
- Build Object file for main.cpp
  - I'm not going to put SDL2 in this repo since it's been distributed to so many systems.
    - There is quite a thorough [wiki](https://wiki.libsdl.org/SDL2/Installation) on installing SDL2 as well as several [videos](https://www.youtube.com/watch?v=H08t6gD1Y1E) covering the installation process.
  - `gcc -c .\src\main.cpp -o .\bin\main.o -I${SDL2 include directory} -I\.include`
    - Since the -c flag removes the linker step, it doesn't matter if you use g++ or gcc compiler.
    - The SDL2 include directory should hold a folder named `SDL2` which contains all of the header files. 
- Build Object file for flecs.c
  - `gcc -c .\include\flecs.c -o .\bin\flecs.o`
    - Using the [Flecs Quickstart](https://www.flecs.dev/flecs/md_docs_2Quickstart.html) page as a reference...
    - Make sure to use either "gcc / clang instead of g++ / clang++" since this a .c file.
    - If you're on Windows and not using Visual Studio (I'm in VSCode):
      - "make sure to add -lWs2_32 to **the end(!)** of the linker command".
      - In this case: `gcc -c .\include\flecs.c -o .\bin\flecs.o -lWs2_32`
- Link main.o against flecs.o and dynamically link SDL2
  - `g++ .\bin\main.o .\bin\flecs.o -o \bin\main -L${SDL2 core library} -lSDL2main -lSDL2 -lWs2_32`
    - The SDL2 core library should hold the binaries necessary for linking such as, `libSDL2.a`, `libSDL2.dll.a`, `libSDL2.lla`, `libSDL2main.a`, and `libSDL2main.la`.
- If you check the contents of `.\bin\`, you should see `flecs.o`, `main.o`, `SDL2.dll`, and `main.exe`.
### Demo
![demo](https://github.com/DeadlyOrchard/ConwaysGameOfLife/blob/main/demo.gif)
- Initial board state: [Gosper glider gun](https://playgameoflife.com/lexicon/Gosper_glider_gun)
