<h1 align="center">
    <br>
    <img src="repoIcon.png" alt="App Logo" height="100"/>
    <br>
    <img src="https://img.shields.io/github/license/BenchatonDev/Celeste-Numworks"/>
    <img src="https://img.shields.io/github/downloads/BenchatonDev/Celeste-Numworks/latest/total"/>
    <br>
    Celeste Classic on the Numworks Calculator !
</h1>

A working, albeit janky and unoptimized port of ccleste by [Lemon Sherbet](https://github.com/lemon-sherbet/) which it self is a port of Celeste classic for the PICO-8 fantasy console by [EXOK](https://github.com/EXOK) to C/C++. It runs at full speed (mostly, you wont notice it though) what can I say ! Oh and saves are persistant through app exit.

# Todo :
- Maybe add cool bezels on the side of the Pico-8 screen
- Maybe make the timer real time accurate (not accurate to the real game but...)

# Controls :
- Pico-8
  - `OK`: Dash
  - `BACK`: Jump
  - `Dpad`: Moves around
- General
  - `Backspace` : Pause
  - `Shift` : Create SaveState
  - `Alpha` : Load SaveState
  - `XNT` : Reset
  - `ScreenShake On/Off` : Toolbox
  - `Home`: Exit


# Building
You'll need the embeded arm compiler and it's optimized std C lib (newlib), nodeJS 16, if you are on MacOS, Windows or a Debian based distro, Numworks provides instructions [here](https://www.numworks.com/engineering/software/build/). If like me you use Arch Linux, I'd recomend you use an AUR helper like [yay](https://github.com/Jguer/yay) and run the following command :
```
yay -S base-devel arm-none-eabi-gcc arm-none-eabi-newlib numworks-udev nvm nodejs npm (numworks-epsilon)
```
The first two packages allow you to compile the project, numworks-udev allows WebDFU and by extention NWLink to see the calculator as the calculator, and the last three are for the packaging of the app, nvm allows you to switch nodeJS version which is usefull because to my knowledge NWLink doesn't work on versions higher than 16 and npm is just node's package manager. Optionally you can install numworks-epsilon which is the calculator os compiled to run as a native app on your system, nice if you want to debug with gdb, it will run faster than on real hardware though (A precompiled version can be found [here](https://github.com/emilie-feral/rpn-app/raw/refs/heads/main/epsilon_simulators.zip) for all platforms but on linux the make file expects you to use numworks-epsilon).
To compile the project just run
```
make
```
you can also directly send it to your calculator by running (or simulator if you set it as the build platform)
```
make run
```

# Acknowledgements
- [EXOK](https://github.com/EXOK) to be more exact Noel Berry and Matt Thorson. For creating Celeste Classic
- [Lemon Sherbet](https://github.com/lemon-sherbet/) Who actually ported the game's code to C (I do not claim his code under this repo's license)
- [Riley0122](https://github.com/riley0122/) For making the template I used (~~MakeFile~~ + Necessary SDK components)
- [Yaya.Cout](https://github.com/Yaya-Cout) For the frameLimiter's code and making storage.h
- [Numworks](https://github.com/numworks/) For being as open as allowed by education legislations and providing an SDK
- [Emilie Feral](https://github.com/emilie-feral) Who made the new MakeFile I use with support for the simulator and web version of Epsilon and for providing a build of the Simulator
