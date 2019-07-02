The ART+COM AG Y60 Media Engine is a multipurpose media and graphics engine targetted at interactive screen installations, interactive projection tables, 3D games and everything else that might go under the name "media installation". It can also be used as a general purpose 3D scene graph and renderer.

Y60 itself is implemented in C++ for the Windows, Linux and Mac OS X platforms. It can be scripted in JavaScript, offering a DOM API for its scene graph that is similar to what can be found in common web browsers. For graphics rendering, it targets OpenGL. 

It is open-source licensed under GPL and available for windows, linux and mac-osx.

# Prerequisites
- CMake: download and install the latest installer from: [cmake]
- Cg Toolkit: get the latest installer from [NVIDIA Cg Toolkit] or install it with your favorite package manager on your OS
- y60 relies heavily on ASL the ART+COM Standard Library and AcMake, so you need to install or build those too. Instructions can be found in the readme at [ASL] and [AcMake]
- windows only:
    - DirectX SDK: download from [DirectX SDK]
    - get the latest dependencies: PRO60Dependencies-*.*.*-win32.exe [Y60 Prebuild Installer]
    - Nullsoft Scriptable Install System NSIS from [NSIS] (optional, only needed for building packages)


# Prebuild Windows 32 Bit binary
Currently, we distribute windows installers at [Y60 Prebuild Installer]
Install the following packages and you should be able to run y60 applications
- get the latest installer Y60-*.*.*-win32.exe 
- get the appropriate dependencies: PRO60Dependencies-*.*.*-win32.exe
- get the appropriate asl library: ASL-*.*.*-win32.exe

# Build from sources
## Checkout
Checkout y60 sources
 
    git clone git@github.com:artcom/y60.git

### Build on Windows
#### Prerequisits
- Visual Studio Express 9 2008, 32Bit
from [Y60 Prebuild Installer]
- get the latest dependencies: PRO60Dependencies-*.*.*-win32.exe
- get the appropriate asl library: ASL-*.*.*-win32.exe
- get the appropriate acmake library: AcMake-*.*.*-win32.exe
and install them

#### If you want to use the gtk binding, also known as G60 (optional), you also need:

install Gtkmm 2.14 or higher (development package) from [gtkmm]

#### Cygwin shell
It is recommended to build Y60 within cygwin, but the instructions should also work with the Windows Command Shell.
- do not use the cygwin cmake
- you must move the following cygwin executables out of the way because they are in conflict with the MS compiler:

        mv /usr/bin/link.exe /usr/bin/_link_cygw.exe
        mv /usr/bin/mt.exe /usr/bin/_mt_cygw.exe

- setup the environment for visual studio, therefore add the following line to the cygwin.bat located in your cygwin install directory

        call "C:\Programme\Microsoft Visual Studio 9.0\Common7\Tools\vsvars32.bat" 

#### Build process
Create build target directory:
  
    cd y60
    mkdir _build  
    cd _build  

Make build scripts using cmake (either for nmake or visual Studio 9 2008).  
Build with ide: 

    cmake -G "Visual Studio 9 2008" .. 
    open Y60.sln and build it using the ide

You can also build using Visual Studio from the command line:

    vcbuild Y60.sln "Release|Win32" 

Build with nmake via shell: 

    cmake -G "NMake Makefiles" ..
    nmake

### Linux
These instructions should work on Ubuntu >=8.04 and Debian (>=lenny) RedHat/CentOS >= 6 on either x86 or X86_64 systems. The Dependent library names may vary on those target systems

#### Install dependencies
     sudo apt-get install git cmake nvidia-cg-dev build-essential autoconf2.13 libboost-dev libboost-system-dev libboost-thread-dev libsdl1.2-dev libglib2.0-dev libavcodec-dev libavformat-dev libswscale-dev libglew1.5-dev libcurl4-openssl-dev libssl-dev libopenexr-dev libtiff5-dev libpng12-dev libgif-dev libasound2-dev libfreetype6-dev libcrypto++-dev

#### If you want to use the gtk binding, also known as G60, you also need:
     sudo apt-get install libgtkmm-2.4-dev libgtkglext1-dev libglademm-2.4-dev

#### Building the documentation requires:
     sudo apt-get install doxygen graphviz texlive

#### You also need java to build the documentation, for which we recommend OpenJDK.
     sudo apt-get install openjdk-6-jre

#### Create yourself a build directory (you will need one per build configuration)
    cd y60
    mkdir -p _builds/release

#### Configure the build tree (this is the equivalent of ./configure)

    cd _builds/release
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local ../../

#### Build the sources

    make -jX

#### Install Y60

    make install

#### Test Y60

    make test

### Mac OS X with Homebrew

We have Homebrew [Homebrew] support. This makes installing on Mac OS X easier than ever!

#### Prerequisites:

- Homebrew Installation: [Homebrew Installation]

#### Add the artcom tap for y60 and related projects (https://github.com/artcom/homebrew-y60):

    brew tap artcom/y60

#### Install watchdog:

    brew install watchdog

#### You can also install the repository head:
    brew install --HEAD y60

With --use-internal-git ART+COMs internal git server is used.

# Building Documentation
#### Y60 API Reference
    make Y60-jsdoc

The results will be in y60/doc/jsdoc.

#### Y60 Schema Reference
    make Y60-xsddoc

The results will be in y60/doc/schema.
#### Doxygen C++ References
    make Y60-doxygen

The results will be y60/doc/doxygen.

- - -
*Copyright (c) [ART+COM AG](http://www.artcom.de/), Berlin Germany 2012 - Author: Gunnar Marten (gunnar.marten@artcom.de)*

[Homebrew Installation]: https://github.com/mxcl/homebrew/wiki/installation
[Homebrew]: https://github.com/mxcl/homebrew/
[gtkmm]: https://live.gnome.org/gtkmm/MSWindows
[Y60 Prebuild Installer]: https://y60.artcom.de/redmine/projects/y60/files
[NSIS]: http://nsis.sourceforge.net/
[DirectX SDK]: http://www.microsoft.com/en-us/download/details.aspx?id=6812
[ASL]: https://github.com/artcom/asl
[AcMake]: https://github.com/artcom/acmake
[NVIDIA Cg Toolkit]: http://developer.nvidia.com/cg-toolkit
[cmake]: http://cmake.org/cmake/resources/software.html
