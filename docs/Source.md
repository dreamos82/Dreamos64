# Source code

## Organization

* All the operating system code is in the `src/` folder
* The `tests/` folder contain some tests that can be run without running the os.
* The `build/` folder contains the build related files.
* Documentation is available in the `docs/` folder.
* `examples` Contains the example applications.


### Source files

The C source files and headers for dreamos are all located inside the `src`folder.

* The `kernel` folder contains all kernel realated sources
    - Inside this folder, the  architecture dependant code is stored inside `arch/xyz`, where `xyz` is the name of the supported architecture (currently only `x86_64`)
* All the assembly code is in the `asm` folder
* `libc` contains the basic files related to the standard C functions (although only few functions are implemented).
* `include` contains the header files.

### Other files

In the project root folder the following files are present:

* The main `Makefile`, with all the main  build goals present.
* Since the kernel use grub to start, the `grub.cfg` file contains grub menu entry to load the kernel
* `README.md` file, the main readme (as seen on github)
