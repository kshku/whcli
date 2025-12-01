# whcli
Search and install wallpapers from wallhaven.

## Building
Clone the repo and initialize the submodules
```sh
git clone --recurse-submodules https://github.com/kshku/whcli.git
cd whcli
```
or
```sh
git clone https://github.com/kshku/whcli.git
cd whcli
git submodule update --init --recursive
```
Build using CMake
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To install
```sh
sudo cmake --install build
```

The wallhavenapi library will be installed to `/usr/local/lib`.
If you are getting a error like
```sh
whcli: error while loading shared libraries: libwhapi.so.1: cannot open shared object file: No such file or directory
```
Then the linker is not looking at `/usr/local/lib` for libraries.
There are different ways tell linker to look there, and one way is by writing .conf file.
Run
```sh
echo "/usr/local/lib/" | sudo tee /etc/ld.so.conf.d/wallhaven.conf
sudo ldconfig
```

## Usage
Run
```sh
whcli -h
```
for available options and also refere to the [wallhaven api documentation](https://wallhaven.cc/help/api)
