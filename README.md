# Generic-C-Collections

CPP like Generic C collections library for educational purposes.

Please, check examples (especially examples/funwithvectors.c) folder to see how to use the libraries.
Note the wrapping of vector_create and usage of atexit to delegate the responsibility of freeing memory to clean function.
This way user need not worry to call vector_destroy for each vector created.

[cmocka]: https://api.cmocka.org/
[CMakeLists]: tests/CMakeLists.txt
[AddressSanitizer]: https://clang.llvm.org/docs/AddressSanitizer.html
[calculator_test]: https://git.cryptomilk.org/projects/cmocka.git/tree/example/calculator_test.c

For compiling our code we will be using CMake and for testing we will be using **[cmocka]** which is an elegant unit testing framework for `C` with support for mock objects. It only requires the standard `C` library, unit testing `C` library with mock support.

## How to install CMake
Remove old version of cmake:

`sudo apt purge --auto-remove cmake`

Obtain a copy of the signing key:

`wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null`

Add the repository to your sources list:

a. For Ubuntu Focal Fossa (20.04)

`sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'`     

b. For Ubuntu Bionic Beaver (18.04)

`sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'`

c. For Ubuntu Xenial Xerus (16.04)

`sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ xenial main'`

Update and install

`sudo apt update`
`sudo apt install cmake`


## How to install CMocka
1. wget http://git.cryptomilk.org/projects/cmocka.git/snapshot/master.tar.gz
2. tar -xvf master
3. cd master
4. mkdir build
5. cd build/
6. cmake ..
7. sudo make all install

**Note** If you get the following error when you build your project with cmocka:

`error while loading shared libraries: libcmocka.so.0: cannot open shared object file: No such file or directory`

It means that program loader cannot find the cmocka shared library file. You need to add the directory in which the shared library  `libmocka.so.0` is present to the file `/etc/ld.so.conf`:

1. First check if `/usr/local/lib` is in LD_LIBRARY_PATH : `echo $LD_LIBRARY_PATH`
2. If step 1 fails: Try temporarily adding it using `export LD_LIBRARY_PATH=/usr/local/lib64:${LD_LIBRARY_PATH}`. When you run your program this time it should work.
3. If step 2 works: Check whether `libcmocka.so.0` resides in `/usr/local/lib` or `/usr/local/lib64`. For example in my Fedora Workstation it resides in `/usr/local/lib64`.
4. Now check whether `/usr/local/lib64` is in `etc/ld.so.conf`. Note that some distros, like my Fedora Workstation, have an `/etc/ld.so.conf.d` directory with and `.conf` file is in that directory. In my case it is `/etc/ld.so.conf.d/libiscsi-x86_64.conf`.
5. Add `/usr/local/lib` (or `/usr/local/lib64`) into the `.conf` file. So in my case I add `/usr/local/lib64` into `/etc/ld.so.conf.d/libiscsi-x86_64.conf` by opening my `emacs` editor in `sudo` mode.
6. Run `sudo ldconfig`.

## How to install adress sanitizer library:
`sudo apt install libasan5`

`sudo dnf install libasan`

## How to add code-coverage

[gcovr]: https://gcovr.com/en/stable/
[code coverage script]: https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake
[Lars Bilke]: https://github.com/bilke

## About **[gcovr]**
[gcovr] provides a utility for managing the use of the GNU `gcov` utility and generating summarized code coverage results. 
To obtain `gcovr`, follow the steps in the web page of **[gcovr]** for installation. It has a simple the tutorial to learn about code coverage
(You may also need to install `lcov` : `sudo dnf install lcov` which will also install `genhtml`)


## A useful script by **[Lars Bilke]**

Say for example we want to investigate the code coverage of the lib files in `libs` directory which were
used by the test `test_vector`. 

A simple beautiful script **[code coverage script]** by **[Lars Bilke]** is very useful. All we need to include the following
code into our `CMakeLists`. 

Thank you **[Lars Bilke]** for making this script available for us!

```
setup_target_for_coverage_gcovr_html(
            NAME test_calc_uptime_coverage  // name of the coverage executable
            EXECUTABLE test_calc_uptime     // test executable
            BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/Uptime" // base dir of the files whose code coverage will be generated
        )
```

**Note** Sometimes when I build the project from VSCode I started to get ninja error. So, I use terminal:

  Build a Debug build:
  1.  mkdir build && cd build
  2.  cmake -DCMAKE_BUILD_TYPE=Debug ..
  3.  make

Now at this point we can see the utility `vector_coverage`. When you run it, `html` files will be created inside
`build\vector_coverage`. Inside this folder, right click on `index.html` to go to the folder that contains it.
You can open `index.html` with your favourite browser.

