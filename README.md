# LICENSE: UNLICENSE #

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>



# gnl #

A collection of cross-platform header only/single file libraries designed to
be easily dropped into any project.

The following are a list of classes provided by gnl.

## gnl_path ##
Provides a way to represent paths to files/folders. Also gives some
limited access to known paths such as the user's home directory
and temp folders.

## gnl_animate ##
A library to represent time-varying variables. Variables are animated
based on the real time clock and can be animated using different
easing functions.

## gnl_base64 ##
Encode and decode base64 strings

## gnl_binpacking ##
Tool for packing smaller rectangles into a bigger rectangle. Useful for
texture atlas' and fonts

## gnl_interp ##
A library for various interpolation schemes such as bezier curves and splines

## gnl_json ##
A library for reading and manipulating json files

## gnl_periodic ##
A libray used for calling functions at a particular intervals.

## gnl_socket ##
A wrapper around unix sockets and winsock based on what OS you are compiling on.

## gnl_threadpool ##
A thread pool implementation. Push tasks onto the queue and the threadpool will
automatically run the tasks in order.

## gnl_unicode ##
A library for working with unicode conversions. Still in testing.

## gnl_variant ##
An implementation of a variant. Probably not the best way to do it.  It can be
used if you don't want to include the massive Boost library and can't wait
till C++17 becomes more standard.


