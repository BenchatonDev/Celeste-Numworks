<h1 align="center">
    <br>
    <img src="repoIcon.png" alt="App Logo" height="100"/>
    <br>
    <img src="https://img.shields.io/github/license/BenchatonDev/Celeste-Numworks"/>
    <img src="https://img.shields.io/github/downloads/BenchatonDev/Celeste-Numworks/latest/total"/>
    <br>
    Celeste Classic on the Numworks Calculator !
</h1>

A working, albeit janky and unoptimized port of ccleste by [Lemon Sherbet](https://github.com/lemon32767) which it self is a port of Celeste classic for the PICO-8 fantasy console by [EXOK](https://github.com/EXOK) to C/C++. It runs at full speed (mostly, you wont notice it though) what can I say ! Oh and saves are persistant through app exit.

# Controls :
- Pico-8
  - `OK`: Dash
  - `BACK`: Jump
  - `Dpad`: Moves around
- General
  - `Backspace` : Pauses the game
  - `Toolbox` : Opens the settings
  - `Shift` : Creates a new save
  - `Alpha` : Loads a save
  - `Ans` : Loads a backup save
  - `XNT` : Reset
  - `Home`: Exit


# Building

You'll need a few dependencies, if you are on MacOS, Windows or a Debian based distro, Numworks provides instructions [here](https://www.numworks.com/engineering/software/build/). If like me you're running Arch Linux, I'd recomend you use an AUR helper like [yay](https://github.com/Jguer/yay) and run the following command :
```
yay -S arm-none-eabi-gcc arm-none-eabi-newlib numworks-udev nodejs npm [numworks-epsilon]
```

(The simulator binary can be found [here](https://github.com/emilie-feral/rpn-app/raw/refs/heads/main/epsilon_simulators.zip) for all platforms, note that on Linux it's expected that numworks-epsilon is in your path).

To compile the project run :
```
make
```

You can also directly test it by running this:
```
make run [PLATFORM=simulator]
```

# Acknowledgements
- [EXOK](https://github.com/EXOK) to be more exact Noel Berry and Maddy Thorson. For creating Celeste Classic
- [Lemon Sherbet](https://github.com/lemon32767) Who actually ported the game's code to C (I do not claim his code under this repo's license)
- [Riley0122](https://github.com/riley0122/) For making the template I used (~~MakeFile~~ + Necessary SDK components)
- [Yaya.Cout](https://github.com/Yaya-Cout) For the frameLimiter's code and making storage.h
- [Oignontom8283](https://github.com/Oignontom8283) For fixing a crash that could occur on N0115 and N0120 upon app exit
- [Numworks](https://github.com/numworks/) For being as open as allowed by education legislations and providing an SDK
- [Emilie Feral](https://github.com/emilie-feral) Who made the new MakeFile I use with support for the simulator and web version of Epsilon and for providing a build of the Simulator
