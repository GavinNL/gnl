#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <gnl/gnl_animate.h>


using Animate = gnl::Animate<float>;

int main(int argc, char ** argv)
{

    Animate B;

    B.set(0.0f)
     .to(  10.0f, 10.0)
     .to( -10.0f, 10.0);

    std::cout << "Time left: " << B.time_left() << std::endl;

    while( !B.stable() )
    {
        float cast_to_float = B; // same as calling B.get()
        std::cout << cast_to_float << std::endl;
        std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
    }

    return 0;
}

