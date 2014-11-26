gnl
===

A collection of header only/single file libraries. Still work in progress.

Each header file is a single module and contains the header and source code for that particular module. It is modeled after the STB library.

To use it in your projects, include the header file whenever you need it use it. But in ONE of your source files (.cpp). Do the following:

#defile GNL_JSON_IMPLEMENT
#include "gnl_json.h"

The define variable is GNL_MODULENAME_IMPLEMENT.  eg: GNL_FILESYSTEM_IMPLEMENT, GNL_ZIP_IMPLEMENT.

LICENSE:
** To be completed in more detail **
Do whatever you want with the code, but don't blame me for anything.


