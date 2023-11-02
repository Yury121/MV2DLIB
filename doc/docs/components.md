components.md
================

#Project content

This implementation contains a Visual Studio solution for creating dynamic libraries (debug and release) for Windows x86/x64 and Linux, and for creating test units for them. It also contains documentation is written as regular Markdown files and compiled using the MkDocs static site generator.

##Source code of dynamic libraries
###Folder ./src
Contains cpp source code files for dynamic library implementation
###Folder ./include
Contains header files for dynamic library implementation
###Folder ./mv2dll
Contains a Visual Studio project for a Windows dynamic library.
###Folder ./mv2lib
Contains a Visual Studio project for a Linux dynamic library.

##Source code of test modules
###Folder ./TestDll
Contains a Visual Studio project and source code for testing a Windows dynamic library.
###Folder ./TestMv2Lin
Contains a Visual Studio project and source code for a Linux dynamic library.


##Documentation
Contains documentation.