Note to authors of CMake files
===============================

Indentation
------------

We use spaces for indentation, tabstop/tabwith is 2. For VIM users:

et ts=2 sw=2 comments=\:\#


Variables
----------

We would like to to produce consistent variable names. So please use only upper
case for VARIABLE names.

XXX_INCLUDE_DIRS        The final set of include directories listed in one
                        variable for use. This should not be a cache entry.
XXX_LIBRARIES           The libraries to link against to use XXX. These should
                        include full paths. This should not be a cache entry.
XXX_DEFINITIONS         Definitions to use when compiling code that uses XXX.
                        This should not be a cache entry.
XXX_LIBRARY_DIRS        Optionally, the final set of library directories listed
                        in one variable for use by client code.
XXX_EXECUTABLE          Where to find the XXX tool.


Modules
--------

If you write CMake Modules follow the Module specifications from the readme.txt
in the CMake/Modules directory on there CVS at http://www.cmake.org/. You could
use the script 'generate_findpackage_file' in the Scripts directory here, to
generate a initial file.
