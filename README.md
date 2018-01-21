# Merge log

Scroll down for the original README.md!

Base revision: 189bd79dffaf450964f5c09547fe7760c999446d

|PR Number|Commit|Title|Author|Merge Success|
|----|----|----|----|----|
|[3402](undefined)|980923d3511e549699ec0bdc9022107f0b3a0f78|Allow Installed Apps to Display in the Game List|BreadFish64|true|
|[3367](undefined)|f42f7dfcab724d8ef7c35875a9221ea65d472b9d|Service/Y2R: convert to ServiceFramework|wwylele|true|
|[3361](undefined)|9be3ce83db0fac5d2b32fd1e4fc83206d27955ad|Service/PTM: convert to ServiceFramework|wwylele|true|
|[3353](undefined)|22be430238ae98ac671eda51a8a1bf4fac903c83|citra-qt: Add customizable speed limit target|jroweboy|true|
|[3325](undefined)|fc1bdc3c626aa088ffa84a9903e134eeddb5dad8|Add ability to send test cases through telemetry|BreadFish64|true|
|[3281](undefined)|88f652151173c0642ec0ee6d01bf0802ba1abdd4|Texture Cache Rework|jroweboy|true|
|[3272](undefined)|fb2d34997e13327ad68b2aa7aa8dd64c20397bb1|core/arm: Backend-specific context implementations|MerryMage|true|
|[3101](undefined)|2052a201c0f60cfff1b043cf1ff631e57f2ad688|Kernel/Threads: Add a new thread status that will allow using a Kernel::Event to put a guest thread to sleep inside an HLE handler until said event is signaled|Subv|true|
|[3073](undefined)|9c482ee1fa9e373d166b00fede5ad13bf5402827|Citra-qt: Add multiplayer ui|jroweboy|true|
|[3071](undefined)|3f96a08984b481fbbbf40ec33903f47efeb3cdb1|Network: Added an executable to host an dedicated room for local wifi|B3n30|true|
|[3070](undefined)|aaec55a8e04826165f30f0f0722ada582498731e|NWM_UDS: change to Service Framework|B3n30|true|
|[3069](undefined)|a5e63a8c35f02022f2c393c5941f53b2518641fd|Announce room webservice|B3n30|true|
|[2882](undefined)|0238e0c5e7071a7c450d258a47106610b3b3fbf3|Movie (Game Inputs) recording and playback|danzel|true|


End of merge log. You can find the original README.md below the break.

------

**BEFORE FILING AN ISSUE, READ THE RELEVANT SECTION IN THE [CONTRIBUTING](https://github.com/citra-emu/citra/blob/master/CONTRIBUTING.md#reporting-issues) FILE!!!**

Citra Emulator
==============
[![Travis CI Build Status](https://travis-ci.org/citra-emu/citra.svg?branch=master)](https://travis-ci.org/citra-emu/citra)
[![AppVeyor CI Build Status](https://ci.appveyor.com/api/projects/status/sdf1o4kh3g1e68m9?svg=true)](https://ci.appveyor.com/project/bunnei/citra)

Citra is an experimental open-source Nintendo 3DS emulator/debugger written in C++. It is written with portability in mind, with builds actively maintained for Windows, Linux and macOS. Citra only emulates a subset of 3DS hardware and therefore is generally only useful for running/debugging homebrew applications.

Citra is even able to boot several commercial games! Some of these do not run at a playable state, but we are working every day to advance the project forward. (Playable here means the same as "OK" on our [game compatibility list](https://citra-emu.org/game).)

Citra is licensed under the GPLv2 (or any later version). Refer to the license.txt file included. Please read the [FAQ](https://citra-emu.org/wiki/faq/) before getting started with the project.

Check out our [website](https://citra-emu.org/)!

For development discussion, please join us at #citra on freenode.

### Development

Most of the development happens on GitHub. It's also where [our central repository](https://github.com/citra-emu/citra) is hosted.

If you want to contribute please take a look at the [Contributor's Guide](CONTRIBUTING.md) and [Developer Information](https://github.com/citra-emu/citra/wiki/Developer-Information). You should as well contact any of the developers in the forum in order to know about the current state of the emulator because the [TODO list](https://docs.google.com/document/d/1SWIop0uBI9IW8VGg97TAtoT_CHNoP42FzYmvG1F4QDA) isn't maintained anymore.

If you want to contribute to the user interface translation, please checkout [citra project on transifex](https://www.transifex.com/citra/citra). We centralize the translation work there, and periodically upstream translation.

### Building

* __Windows__: [Windows Build](https://github.com/citra-emu/citra/wiki/Building-For-Windows)
* __Linux__: [Linux Build](https://github.com/citra-emu/citra/wiki/Building-For-Linux)
* __macOS__: [macOS Build](https://github.com/citra-emu/citra/wiki/Building-for-macOS)


### Support
We happily accept monetary donations or donated games and hardware. Please see our [donations page](https://citra-emu.org/donate/) for more information on how you can contribute to Citra. Any donations received will go towards things like:
* 3DS consoles for developers to explore the hardware
* 3DS games for testing
* Any equipment required for homebrew
* Infrastructure setup
* Eventually 3D displays to get proper 3D output working

We also more than gladly accept used 3DS consoles, preferably ones with firmware 4.5 or lower! If you would like to give yours away, don't hesitate to join our IRC channel #citra on [Freenode](http://webchat.freenode.net/?channels=citra) and talk to neobrain or bunnei. Mind you, IRC is slow-paced, so it might be a while until people reply. If you're in a hurry you can just leave contact details in the channel or via private message and we'll get back to you.
