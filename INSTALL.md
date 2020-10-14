INSTALLATION INSTRUCTIONS FOR BOREAS
=====================================

Please note: The reference system used by most of the developers is Debian
GNU/Linux 'Buster' 10. The build might fail on any other system. Also, it is
necessary to install dependent development packages.

Prerequisites for boreas
-------------------------

Prerequisites:
* a C compiler (e.g. gcc)
* cmake >= 3.0
* libgvm_base, libgvm_boreas, libgvm_util >= 20.8
* glib-2.0 >= 2.42

Prerequisites for building documentation:
* Doxygen
* xmltoman (optional, for building man page)

Prerequisites for building tests:
* Cgreen (optional, for building tests)

Install prerequisites on Debian GNU/Linux 'Buster' 10:

    apt-get install \
    gcc \
    cmake \
    pkg-config \
    libglib2.0-dev \
    libpcap-dev


Compiling boreas
-----------------

If you have installed required libraries to a non-standard location, remember to
set the `PKG_CONFIG_PATH` environment variable to the location of you pkg-config
files before configuring:

    export PKG_CONFIG_PATH=/your/location/lib/pkgconfig:$PKG_CONFIG_PATH

Create a build directory and change into it with:

    mkdir build
    cd build

Then configure the build with:

    cmake -DCMAKE_INSTALL_PREFIX=/path/to/your/installation ..

Or (if you want to use the default installation path /usr/local):

    cmake ..

This only needs to be done once.

Thereafter, the following commands are useful:

    make                # build the scanner
    make doc            # build the documentation
    make doc-full       # build more developer-oriented documentation
    make tests          # build tests
    make install        # install the build
    make rebuild_cache  # rebuild the cmake cache

Please note that you may have to execute `make install` as root, especially if
you have specified a prefix for which your user does not have full permissions.

To clean up the build environment, simply remove the contents of the `build`
directory you created above.


Static code analysis with the Clang Static Analyzer
---------------------------------------------------

If you want to use the Clang Static Analyzer (https://clang-analyzer.llvm.org/)
to do a static code analysis, you can do so by prefixing the configuration and
build commands with `scan-build`:

    scan-build cmake ..
    scan-build make

The tool will provide a hint on how to launch a web browser with the results.

It is recommended to do this analysis in a separate, empty build directory and
to empty the build directory before `scan-build` call.
