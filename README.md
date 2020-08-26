# GNL


| Branch    | Master                                                                                                                           | Dev                                                                                                                        |
| --------- | -------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------- |
| Gitlab-CI | [![pipeline status](https://gitlab.com/GavinNL/gnl/badges/master/pipeline.svg)](https://gitlab.com/GavinNL/gnl/-/commits/master) | [![pipeline status](https://gitlab.com/GavinNL/gnl/badges/dev/pipeline.svg)](https://gitlab.com/GavinNL/gnl/-/commits/dev) |



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
