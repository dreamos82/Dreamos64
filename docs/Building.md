# Build configuration

## Compilation

The following flags have to be properly set before building the os:

* `BUILD`: this control the folder where all the output files from the build are placed. Default value is: `dist`
* `ARCH_PREFIX`: for now it only supports `x86_64-elf`, this will be used as prefix for the compiler and the linker.
* `FONTS`: specify the folder where the font is stored.


## Kernel related flags

They can change at any moment, or can be removed in the future

* `USE_FRAMEBUFFER`   if set to 1 it use the framebuffer video mode, if set to 0 it use the legacy VGA driver. 
* `SMALL_PAGES` if set to 1 the virtual memory will use 4k pages if set to 0 it will use 2mb pages 
