# GNL

| Branch | Gitlab-CI | Travis-CI | Appveyor  | CodeCov   | Codacy    |
|--------|-----------|-----------|-----------|-----------|-----------|
| master    | [![Build Status](https://gitlab.com/GavinNL/gnl/badges/master/build.svg)](https://gitlab.com/GavinNL/gnl/pipelines/) | [![Build Status](https://travis-ci.org/GavinNL/gnl.svg?branch=master)](https://travis-ci.org/GavinNL/gnl) | [![Build status](https://ci.appveyor.com/api/projects/status/0tsak73ak5c2mhbu/branch/master?svg=true)](https://ci.appveyor.com/project/GavinNL/gnl/branch/master) | [![codecov](https://codecov.io/gh/GavinNL/gnl/branch/master/graph/badge.svg)](https://codecov.io/gh/GavinNL/gnl) | [![Codacy Badge](https://api.codacy.com/project/badge/Grade/4fe50de6102e4d678bacdd292c949310)](https://www.codacy.com/app/GavinNL/gnl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=GavinNL/gnl&amp;utm_campaign=Badge_Grade) |
| dev       | [![Build Status](https://gitlab.com/GavinNL/gnl/badges/dev/build.svg)](https://gitlab.com/GavinNL/gnl/pipelines/) | [![Build Status](https://travis-ci.org/GavinNL/gnl.svg?branch=dev)](https://travis-ci.org/GavinNL/gnl) | [![Build status](https://ci.appveyor.com/api/projects/status/0tsak73ak5c2mhbu/branch/dev?svg=true)](https://ci.appveyor.com/project/GavinNL/gnl/branch/dev) | [![codecov](https://codecov.io/gh/GavinNL/gnl/branch/dev/graph/badge.svg)](https://codecov.io/gh/GavinNL/gnl) | ![Codacy branch grade](https://img.shields.io/codacy/grade/4fe50de6102e4d678bacdd292c949310/dev.svg) |

A set of header-only utility libraries I have built to assist in various projects.

# animate

Animate numerical variables based off wall-clock time.

# aspan (aliased span)

Aspans are similar to the gsl::span which allows you to create a view into a
random access container. But additionaly, aspans, allow you to alias the
raw data as another type. For example, you can alias 4 floats as a 2x2 matrix.


```C++
struct vec3 {
    float x, float y, float z;
};

std::vector<float> raw_floats(3*10);

gnl::aspan<vec3> V(raw_floats);

assert( V.size() == 10);

V[0].x = 3.15;
V[0].y = 1.5;
V[0].z = 66.54;

V[9].x = 3.15;
V[9].y = 1.5;
V[9].z = 66.54;


// create an alias for every 3rd float
gnl::array_alias<float> X(raw_floats, 0, sizeof(float)*3);
X[0] == V[0]
X[1] == V[3]
X[2] == V[6]
///
```

# array_view (deprecated in favour of aspan)

Create views into arrays.

# base64

Convert to and from base64

# Entity Component System (entity_component_system.h)

Create entity component systems for games.

# interp

Interpolate variables using linear or cubic splines

# Message Bus (message_bus2.h)

Provides: gnl::event_bus, gnl::event_queue, gnl::event_queue_bus

Create a message bus and send messages to any functions which are listening.

```C++
struct MyMessage
{
    int x;
};

void receiver(const MyMessage & M)
{

}

gnl::event_bus E;

E.connect( &receiver );

E.send( MyMessage() );
```

# Resource_Path

Used to locate resources that you need (eternal files). Add a set of folders
to the Resource_path and then search for the first occurance of a file.

```C++
 gnl::resource_path R;

 R.add_path("/usr/share");
 R.add_path("/usr/local/share");

// find the first occurance of my_file.txt in one of the two folders provided
 auto abs_path = R.get("my_file.txt");  // either returns /usr/share/my_file.txt
                                        // /usr/local/share/my_file.txt
                                        // or "".
```

# meta

Some template meta-helpers

# path

Create and manipulate filesystem paths.

# rk4

4th order Runge-Kutta solver.

```C++
```

# Signal

Signal-slot interface similar to Qt

# socket

Cross-platform C++ socket class.

```C++
```
# Socket_shell

Create a simple multi-user unix-like shell for your application. Register
shell commands and link them to functions in your application. Connect to the
shell using `netcat` or `socat`

See [examples/socket_shell.cpp](examples/socket_shell.cpp)

# threadpool

Thread Pool class, push tasks into the pool and have them run on separate threads.

# unicode

Convert to and from unicode

# variant (deprecated)

Simple variant implementation. C++17's variant class deprecates this.
