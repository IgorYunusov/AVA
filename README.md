<p align="center">
<br/>
  <img src="etc/logo/free_logo_2.png" width="80px"/><br/>
  ΛVΛ<br/>
  Small game engine.<br/>
  Written in 32 random ̷d̷a̷y̷s̷ m̷o̷n̷t̷h̷s̷ years.<br/>
<br/>|
  <a href="#features">Features</a> |
  <a href="#rationale">Rationale</a> |
  <a href="#getting-started">Getting started</a> |
<br/>|
  <a href="#unlicense">Unlicense</a> |
  <a href="#documentation">Documentation</a> |
  <a href="#todo-ᕕᐛᕗ">Todo ᕕ(ᐛ)ᕗ</a> |
<br/>
<br/>
</p>

## Features

- [x] ~Fast~. Small.
- [x] ~Modern~. Readable.
- [x] ~Smart~. Simple.
- [x] ~C++~. Bindable C.
- [x] ~Full featured~. Extendable.
- [x] ~Object-oriented~. Data-oriented.
- [x] ~Browsable documentation~. Commented.
- [x] ~Hundred of file formats supported~. Unique binary format.
- [x] ~Royaltie fee~. Free and unlicensed.
- [x] ~Other buzzwords~. Fun. Direct. Embeddable.

## Rationale

- Minimalistic coding style.
- Less is more, DOD, ECS, KISS.
- Heap allocations under control.
- No installation, no makefiles, fast compilation times.
- Agnostic collaborative 3D editor that you can reuse for your own game engine.
- Agnostic data pipeline that you can reuse for your own game engine.
- Portable tools that speak JSON everywhere: still human-readable, still mergeable workflow.
- Runtime uses MSGPACK and 64-bit hashes only (or JSON and utf8 strings if debugging).
- Inspiration from: bitsquid, stb, rlk, krig, mio, tigr, jo.

## Todo ᕕ(ᐛ)ᕗ
- [x] 01 [Project structure, build system and makefiles.](#01)
-
-

## Screenshots

### 01
Project structure, build system and makefiles.
<img src="etc/doc/001.gif" height="96px" align="right">
<br><br>

## Releases

v001_Monkey3_Icarus, v000_YOB_Marrow

## Getting started

Execute the `AVA.sh.bat` script file to build & run the editor.

Additionally:
- `AVA`                        (launch editor)
- `AVA game [platform]`        (launch game)
- `AVA make [game] [platform]` (make game.exe executable)
- `AVA cook [game] [platform]` (cook all raw game assets)
- `AVA pack [game] [platform]` (pack all cooked game assets into game.pak file)
- `AVA fuse [game] [platform]` (fuse game.pak and game.exe both together)

## Disk structure

- `#bin/`, the generic and collaborative 3d editor.
- `#sdk/`, the engine runtime plus all your custom extensions.
- `#etc/`, the scripts that the editor uses to import foreign data.
- `#www/`, custom data to share in your network. (#lan, #wan, #www ?).
- `game/`, every other folder is actually a game to be processed and packaged (*).
- `.git/`, dot folders are always ignored.

(*) Tip: So, be tidy at structuring your datas from the very beginning!

## Download

## Build

### AVA as engine:
- [x] Compile the amalgamated `AVA.c` source file to get a functional game engine.
- [x] Compile your game as a dynamic library and place it near the `AVA` executable.
- [x] Tip: Rename both executable and dynamic library as desired. Ie, `game.exe` with `game.dll` will work.
- [x] Tip: Place an `.ico` icon close to the binary if you want to as well.
```lisp
cl      AVA.c                && echo AVA as engine (Windows)
g++     AVA.c                && echo AVA as engine (Linux)
clang++ AVA.c                && echo AVA as engine (OSX)
```

### AVA as library:
- [x] Compile your game and link amalgamated `AVA.c` source file with it.
```lisp
cl      game.c AVA.c         && echo AVA as library (Windows)
g++     game.c AVA.c         && echo AVA as library (Linux)
clang++ game.c AVA.c         && echo AVA as library (OSX)
```

### Redistribution
```
- game.exe + .ico           (player)
- game.dll/.lua/.socket      (logic) \
- data.rom/.pak         (ro-storage) | game :)
- data.ram/.sql         (rw-storage) /
```

## Alternatives

[amulet](https://github.com/search?utf8=%E2%9C%93&q=game+engine+amulet&type=),
[anvil](https://github.com/search?utf8=%E2%9C%93&q=game+engine+anvil&type=),
[aroma](https://github.com/search?utf8=%E2%9C%93&q=game+engine+aroma&type=),
[blendelf](https://github.com/search?utf8=%E2%9C%93&q=game+engine+blendelf&type=),
[chickpea](https://github.com/search?utf8=%E2%9C%93&q=game+engine+chickpea&type=),
[corange](https://github.com/search?utf8=%E2%9C%93&q=game+engine+corange&type=),
[ejoy2d](https://github.com/search?utf8=%E2%9C%93&q=game+engine+ejoy2d&type=),
[hate](https://github.com/search?utf8=%E2%9C%93&q=game+engine+hate&type=),
[island](https://github.com/search?utf8=%E2%9C%93&q=game+engine+island&type=),
[juno](https://github.com/search?utf8=%E2%9C%93&q=game+engine+juno&type=),
[love](https://github.com/search?utf8=%E2%9C%93&q=game+engine+love&type=),
[mini3d](https://github.com/search?utf8=%E2%9C%93&q=game+engine+mini3d&type=),
[mio](https://github.com/search?utf8=%E2%9C%93&q=game+engine+mio&type=),
[openframeworks](https://github.com/search?utf8=%E2%9C%93&q=game+engine+openframeworks&type=),
[openkore](https://github.com/search?utf8=%E2%9C%93&q=game+engine+openkore&type=),
[pez](https://github.com/search?utf8=%E2%9C%93&q=game+engine+pez&type=),
[moon](https://github.com/search?utf8=%E2%9C%93&q=game+engine+moon&type=),
[punity](https://github.com/search?utf8=%E2%9C%93&q=game+engine+punity&type=),
[roam](https://github.com/search?utf8=%E2%9C%93&q=game+engine+roam&type=),
[sushi](https://github.com/search?utf8=%E2%9C%93&q=game+engine+sushi&type=),
[tigr](https://github.com/search?utf8=%E2%9C%93&q=game+engine+tigr&type=),

## Status

[![Discord Channel](https://img.shields.io/badge/discord-AVA%20lounge-738bd7.svg)](https://discord.gg/vu6Vt9d)
[![Travis](https://api.travis-ci.org/r-lyeh/AVA.svg?branch=master)](https://travis-ci.org/r-lyeh/AVA)
[![Unlicensed](http://img.shields.io/badge/license-Unlicense-blue.svg?style=flat)](http://unlicense.org/)
