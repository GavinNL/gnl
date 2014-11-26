#gnl

A collection of header only/single file libraries. Still work in progress.

Each header file is a single module and contains the header and source code for that particular module. It is modeled after the STB library.

To use it in your projects, include the header file whenever you need it use it. But in ONE you must define a preprocessor variable before
including the header. Defining the GNL_XXX_IMPLMEMENT variable will include the source implementation code to compile as well.

The variable is GNL_MODULENAME_IMPLEMENT.  eg: GNL_FILESYSTEM_IMPLEMENT, GNL_ZIP_IMPLEMENT, GNL_JSON_IMPLEMENT. See Examples if you are unclear on
how to include the source code.

#LICENSE:
** To be completed in more detail **
Do whatever you want with the code, but don't blame me for anything.


