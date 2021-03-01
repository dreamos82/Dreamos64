# DreamOS64

## Prerequisites: 

These are the packages you need to build and run it: 

* nasm
* qemu and qemu-kvm
* grub-mkrescue
* xorriso
* Gcc cross compiler - Instructions coming soon

## Build
### Building the cross compiler

To build the cross compiler you need the following packages:

* build-essential (on debian derivatives)
* bison
* flex
* libgmp3-dev
* libmpc-dev
* libmpfr-dev
* texinfo
* libcloog 

Then you need to download sources of: 
* Binutils - available here: https://www.gnu.org/software/binutils/
* Gcc - Available here: https://www.gnu.org/software/gcc/

Decide where to install the tools and declare the environment variables: 

```bash
export PREFIX="$HOME/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
```

Now create a new folder and enter into it: 

```bash
mkdir build-binutils
cd build-binutils
```

Let's assume that this folder is in the same level of the binutils sources.

Then you can compile the tools using the custom PREFIX and TARGET: 

```bash
../binutils-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install
```

* --disable-nls disable national language support, it helps to reduce compile time.
* --with-sysroot Tells binutils to enable sysroot support in the cross-compiler by pointing it to a default empty directory.

If everything goes ok, you can see in your $PREFIX folder the output of the build.
Now is time for gcc, we will use a similar approach. First create a build dir: 
```bash
mkdir build-gcc
cd build-gcc
```

Again i assume that it is on the same level of the gcc sources folder. 
To compile: 

```bash
../gcc-x.y.z/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```

* --enable-languages=c,c++ we tell gcc to enable only those languages
* --without-headers We tell to ignore all the C libraries present for the target.

Now if you want you can make permanent the $PATH update:


### Build the OS 
To build just run: 

```bash
    make
```

It will create an output file called DreamOs64.iso

## Run

To run just execute: 
```bash
    make run
```

If you use instead:
```bash
make debug
```

It will output logging information on a logfile.

More info to be added.



