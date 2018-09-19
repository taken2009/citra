# Merge log

Scroll down for the original README.md!

Base revision: 241f10a6697fe70aaa7a7be5a83d8c9532e2d48d

|Pull Request|Commit|Title|Author|Merged?|
|----|----|----|----|----|
|[6](https://github.com/citra-emu/citra-canary/pull/6)|[a53a8d3](https://github.com/citra-emu/citra-canary/pull/6/files/)|Canary Base (MinGW Test)|[liushuyu](https://github.com/liushuyu)|Yes|
|[4232](https://github.com/citra-emu/citra/pull/4232)|[697bc1a](https://github.com/citra-emu/citra/pull/4232/files/)|loader, cfg: choose region based on language if multiple regions available|[wwylele](https://github.com/wwylele)|Yes|
|[4229](https://github.com/citra-emu/citra/pull/4229)|[d360775](https://github.com/citra-emu/citra/pull/4229/files/)|citra_qt, core: game list "Open XXX Location" improvements|[zhaowenlan1779](https://github.com/zhaowenlan1779)|Yes|
|[4211](https://github.com/citra-emu/citra/pull/4211)|[f3d5955](https://github.com/citra-emu/citra/pull/4211/files/)|web_service: stop using std::future + callback style async|[wwylele](https://github.com/wwylele)|Yes|
|[4209](https://github.com/citra-emu/citra/pull/4209)|[df04b28](https://github.com/citra-emu/citra/pull/4209/files/)|service/ptm, citra_qt: Add Play Coins setting|[zhaowenlan1779](https://github.com/zhaowenlan1779)|Yes|
|[4194](https://github.com/citra-emu/citra/pull/4194)|[c9c7097](https://github.com/citra-emu/citra/pull/4194/files/)|audio_core: Simplify sink interface|[MerryMage](https://github.com/MerryMage)|Yes|
|[4183](https://github.com/citra-emu/citra/pull/4183)|[cdd3bd7](https://github.com/citra-emu/citra/pull/4183/files/)|citra_qt/multiplayer: three minor fixes|[zhaowenlan1779](https://github.com/zhaowenlan1779)|Yes|
|[4181](https://github.com/citra-emu/citra/pull/4181)|[2a68dd3](https://github.com/citra-emu/citra/pull/4181/files/)|Add encrypted CIA support|[wwylele](https://github.com/wwylele)|Yes|
|[4140](https://github.com/citra-emu/citra/pull/4140)|[a3625b5](https://github.com/citra-emu/citra/pull/4140/files/)|citra_qt/configuration: misc input tab improvements|[zhaowenlan1779](https://github.com/zhaowenlan1779)|Yes|
|[4114](https://github.com/citra-emu/citra/pull/4114)|[a8af14a](https://github.com/citra-emu/citra/pull/4114/files/)|Implement soc - RecvFromOther|[namkazt](https://github.com/namkazt)|Yes|
|[4113](https://github.com/citra-emu/citra/pull/4113)|[0e89555](https://github.com/citra-emu/citra/pull/4113/files/)|fix soc service - RecvFrom - SendTo|[namkazt](https://github.com/namkazt)|Yes|
|[4089](https://github.com/citra-emu/citra/pull/4089)|[80bdc4a](https://github.com/citra-emu/citra/pull/4089/files/)|[NOT FOR MERGE YET] Ignore format reinterpretation hack|[jroweboy](https://github.com/jroweboy)|Yes|
|[4005](https://github.com/citra-emu/citra/pull/4005)|[0d15b99](https://github.com/citra-emu/citra/pull/4005/files/)|service/cecd: Implement some functions|[NarcolepticK](https://github.com/NarcolepticK)|Yes|
|[3959](https://github.com/citra-emu/citra/pull/3959)|[ad6b140](https://github.com/citra-emu/citra/pull/3959/files/)|service/apt: Implement soft reset & CloseApplication|[zhaowenlan1779](https://github.com/zhaowenlan1779)|Yes|
|[3786](https://github.com/citra-emu/citra/pull/3786)|[74c287d](https://github.com/citra-emu/citra/pull/3786/files/)|citra-qt: Make hotkeys configurable via the GUI.|[adityaruplaha](https://github.com/adityaruplaha)|Yes|


End of merge log. You can find the original README.md below the break.

------

**BEFORE FILING AN ISSUE, READ THE RELEVANT SECTION IN THE [CONTRIBUTING](https://github.com/citra-emu/citra/blob/master/CONTRIBUTING.md#reporting-issues) FILE!!!**

Citra
==============
[![Travis CI Build Status](https://travis-ci.org/citra-emu/citra.svg?branch=master)](https://travis-ci.org/citra-emu/citra)
[![AppVeyor CI Build Status](https://ci.appveyor.com/api/projects/status/sdf1o4kh3g1e68m9?svg=true)](https://ci.appveyor.com/project/bunnei/citra)

Citra is an experimental open-source Nintendo 3DS emulator/debugger written in C++. It is written with portability in mind, with builds actively maintained for Windows, Linux and macOS.

Citra emulates a subset of 3DS hardware and therefore is useful for running/debugging homebrew applications, and it is also able to run many commercial games! Some of these do not run at a playable state, but we are working every day to advance the project forward. (Playable here means compatibility of at least "Okay" on our [game compatibility list](https://citra-emu.org/game).)

Citra is licensed under the GPLv2 (or any later version). Refer to the license.txt file included. Please read the [FAQ](https://citra-emu.org/wiki/faq/) before getting started with the project.

Check out our [website](https://citra-emu.org/)!

For development discussion, please join us at #citra-dev on freenode.

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
