# GraphRox Graph Approximation Library

GraphRox is a C library (with a Python wrapper) for generating smaller approximations of large graphs for the purpose of performing fast computations.

## C and Python Interfaces

Currently, documentation is not available for the C and Python interfaces. The best documentation comes from the `graphrox/gphrx/include/gphrx.h` file. Each function declaration is preceded by a documentation-style comment explaining the function's purpose.

The `graphrox/gphrx.py` file has a `GphrxGraph` class containing all relevant Python functions. For the most part, they line up with the C interface (except for the C functions meant for freeing memory; the freeing of memory is handled in the `__del__()` functions in `gphrx.py`).

## Building the Library

To build the project into a Dynamic Link Library (DLL), navigate to the `graphrox` directory and run `./build.sh`. On some systems, you may need to give the build script the `+x` permissions with `chmod +x ./build.sh` before you can run it. 

The output file name is determined by the `$OUTPUT_LOC` variable near the top of the build script. Other parameters, such as compiler flags, can be modified by changing variables in the script as well. The built DLL will be copied into the `lib` directory for the Python wrapper to use. It is important to use the correct name for the DLL, or the Python wrapper won't be able to find the library. The expected names per system are as follows:

*Windows x86-64*: `graphrox.dll`
*Linux x86-64*: `graphrox.so`
*macOS x86-64*: `graphrox-x86.dylib`
*macOS Apple Silicon*: `graphrox-arm64.dylib`

## Testing the Library

Though the tests themselves are **not** platform specific, the test runner included in the repository currently only supports Bash (or any shell that has a  similar API, such as Zsh) on Unix systems. All code for the test runner can be found in the `graphrox/gphrx/test` directory and can be swapped out for code compatible with any system.

The tests can be run from the `graphrox` directory by running the `./test.sh` script. Like the build script, build parameters can be changed for the tests by modifying variables in the test script.

## Other Scripts

The `graphrox/scripts` directory houses scripts that are useful for common tasks such as converting graph files from one format into the GraphRox format (denoted by the `.gphrx` file extension). The scripts expect to be run with the `graphrox` folder as the working directory, not the `graphrox/scripts` folder.
