[![Build Status](https://travis-ci.org/kost/keepassz.png)](https://travis-ci.org/kost/keepassz)

# keepassz
Keepassz is improved Keepassx 0.x/1.x

It is based on latest 0.4.4 source of keepassx.

## Features

- Modified to build with newer compilers 

- added option to hide comments

- Renamed to keepassz in order to avoid conflicts

## Installing

Installing depends on platform and distribution.

### Ubuntu (LTS) from PPA

It is standard PPA way:

```
sudo add-apt-repository ppa:kost/ppa
sudo apt-get update
sudo apt-get install keepassz
```

### From source

You have basically two ways. Download release or git clone current master. 
Check out building section for building and installing instructions.
You just need to say ```make install``` on the end in order to install binaries on the system.

## Building

In order to build you need to satisfy requirements.

### Requirements

You need QT4 for building. On Debian/Ubuntu, install following packages:

```apt-get install libqt4-dev cmake```

### Building

It is standard cmake build:

```
git clone https://github.com/kost/keepassz.git
cd keepassz
mkdir build
cd build
cmake ..
make
```



