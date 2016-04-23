# keepassz
Keepassz is improved Keepassx 0.x/1.x

It is based on latest 0.4.4 source of keepassx.

## Features

- Modified to build with newer compilers 

- added option to hide comments

- Renamed to keepassz in order to avoid conflicts

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



