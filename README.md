# wvm - Wine version manager
Download and build different versions of wine.

Still a WIP so still a little wonky, but still an easier experience then building wine yourself.

If you use any of the following distros consider downloading the the prebuilt binaries from [here](https://dl.winehq.org/wine-builds/):
- debian
- fedora
- macOS
- mageia
- ubuntu

## Installation
### Requirements
- curl
- g++
- gnu make

### build
Simply clone the repo:
`$ git clone https://github.com/mativ123/wvm.git`
and build:
`$ make`

## Usage
Simply use the `-i` option followed by the version you want to install. wvm will automatically build.
  
Use the `--help,-h` option for more information.
