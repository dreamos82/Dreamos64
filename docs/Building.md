# Build configuration

## Compilation

The following flags have to be properly set before building the os:

* `BUILD_FOLDER`: this control the folder where all the output files from the build are placed. Default value is: `dist`
* `FONT_FOLDER`: where to find the font for the kernel
* `ARCH_PREFIX`: for now it only supports `x86_64-elf`, this will be used as prefix for the compiler and the linker.
* `FONTS`: specify the folder where the font is stored.
* `IMAGE_BASE_NAME`: the base name of the image (the full name will have also the `ARCH_PREFIX` variable, and a version number.

The above flags are present in the `build/Config.mk` folder.

## Kernel related flags

They can change at any moment, or can be removed in the future

* `USE_FRAMEBUFFER`   if set to 1 it use the framebuffer video mode, if set to 0 it use the legacy VGA driver.
* `SMALL_PAGES` if set to 1 the virtual memory will use 4k pages if set to 0 it will use 2mb pages

They are experimental temporary features, there are chances that they can be removed in the future.

## Makefile Goals

Below the flags that are currently implemented to control the build/execution of the kernel:

* `build`: Just build the os, and create the iso in `$BUILD_FOLDER`
* `run`:  It run the os using qemu
* `clean`: To delete all build files
* `debug`: To start the OS with the DEBUG flag active, it will print all messages on stdout, logging in _Verbose_ mode
* `gdb`: To start the OS with remote debugging mode enabled, to start the OS you need to connect using gdb and control execution from there.
* `tests`: To run some _kind of_ unit tests.
