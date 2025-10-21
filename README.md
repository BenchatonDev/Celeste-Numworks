<h1 align="center">
    <br>
    <img src="repoIcon.png" alt="App Logo" height="100"/>
    <br>
    <img src="https://img.shields.io/github/license/BenchatonDev/Celeste-Numworks"/>
    <img src="https://img.shields.io/github/downloads/BenchatonDev/Celeste-Numworks/latest/total"/>
    <br>
    Celeste Classic on the Numworks Calculator !
</h1>

A working, albeit janky and unoptimized port of ccleste by [Lemon Sherbet](https://github.com/lemon-sherbet/) which it self is a port of Celeste classic for the PICO-8 fantasy console by [EXOK](https://github.com/EXOK) to C/C++. Perfromance is OK (probably around the expected 30fps of a PICO-8 game if not a bit more) but the way I chose to directectly render to the screen makes it flicker really fast, if you are light sensitive DO NOT USE IT, which is why I don't provide a build in a release.

# Todo :
- FrameBuffer rendering issues in the [framebuffer-dev branch](https://github.com/BenchatonDev/Celeste-Numworks/tree/framebuffer-dev)
- Add cool bezels on the side of the Pico-8 screen
- Add Reset functionality
- Maybe add saving / loading
- Maybe switch to cppleste (High performance cpp port by the Celeste Classic community)

# Controls :
- Pico-8
  - `OK`: Dash
  - `BACK`: Jump
  - `Dpad`: Moves around
- General (Implemented)
  - `Backspace` : Pause
  - `Home`: Exit
- General (To be implemented)
  - `Shift` : Create SaveState
  - `Alpha` : Load SaveState
  - `ToolBox` : Reset

# Building
You'll need the embeded arm compiler and it's optimized std C lib (newlib), nodeJS 16, if you are on MacOS, Windows or a Debian based distro, Numworks provides instructions [here](https://www.numworks.com/engineering/software/build/). If like me you use Arch Linux, I'd recomend you use an AUR helper like [yay](https://github.com/Jguer/yay) and run the following command :
```
yay -S base-devel arm-none-eabi-gcc arm-none-eabi-newlib numworks-udev nvm nodejs npm
```
The first two packages allow you to compile the project, numworks-udev allows WebDFU and by extention NWLink to see the calculator as the calculator, and the last three are for the packaging of the app, nvm allows you to switch nodeJS version which is usefull because to my knowledge NWLink doesn't work on versions higher than 16 and npm is just node's package manager.
To compile the project just run
```
make
```
you can also directly send it to your calculator by running
```
make run
```

# Acknowledgements
- [EXOK](https://github.com/EXOK) to be more exact Noel Berry and Matt Thorson. For creating Celeste Classic
- [Lemon Sherbet](https://github.com/lemon-sherbet/) Who actually ported the game's code to C (I do not claim his code under this repo's license)
- [Riley0122](https://github.com/riley0122/) For making the template I used (MakeFile + Necessary SDK components)
- [Numworks](https://github.com/numworks/) For being as open as allowed by education legislations and providing an SDK
