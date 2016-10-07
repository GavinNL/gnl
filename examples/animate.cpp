#include <gnl/gnl_animate.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <string>


void Run_Const( const gnl::Animate<float> & constanimate);


int main(int argc, char ** argv)
{
    using namespace gnl;

    // A will hold an animated floating value.
    Animate<float> A;

    A.To(50, 2,  Animate<float>::easeInBounce() );
    A.To(0,  2,  Animate<float>::linear() );
    A.To(50, 2,  Animate<float>::easeOutBounce() );
    A.To(0, 2, Animate<float>::easeInCubic   ());
    A.To(50, 2, Animate<float>::easeOutQuad   ());
    A.To(0, 2, Animate<float>::easeInOutQuad ());
    A.To(50, 2, Animate<float>::easeOutCubic  ());
    A.To(0, 2, Animate<float>::easeInOutCubic());
    A.To(50, 2, Animate<float>::easeInQuart   ());
    A.To(0, 2, Animate<float>::easeOutQuart  ());
    A.To(50, 2, Animate<float>::easeInOutQuart());
    A.To(0, 2, Animate<float>::easeInQuint   ());
    A.To(50, 2, Animate<float>::easeOutQuint  ());
    A.To(0, 2, Animate<float>::easeBackEase());
    A.To(50, 2, Animate<float>::easeInOutQuint());
    A.To(0, 2, Animate<float>::easeInQuad    ());

    // Using A.get() changes the internal structure of A
    // Animated values cannot be reset so they get animated the same way.
    // Once A has been set to a new value using .To( ) it will
    // change ti's internal value automatically according to the real-time clock.
    while( !A.Stable() )
    {
        float value = A.get();
        std::string bar( 30 + (int)value,' '); // Everytime A.get() is called, the internal structure of A is changed.
        bar += '#';

        std::cout << bar << std::endl;

        std::this_thread::sleep_for( std::chrono::milliseconds(10) );

    }



    // move the float value from 0 to 50 and back again a number of times
    // using a different easing functions
    A.To(50, 2,  Animate<float>::easeInBounce() );
    A.To(0,  2,  Animate<float>::linear() );
    A.To(50, 2,  Animate<float>::easeOutBounce() );
    A.To(0, 2, Animate<float>::easeInCubic   ());
    A.To(50, 2, Animate<float>::easeOutQuad   ());
    A.To(0, 2, Animate<float>::easeInOutQuad ());
    A.To(50, 2, Animate<float>::easeOutCubic  ());
    A.To(0, 2, Animate<float>::easeInOutCubic());
    A.To(50, 2, Animate<float>::easeInQuart   ());
    A.To(0, 2, Animate<float>::easeOutQuart  ());
    A.To(50, 2, Animate<float>::easeInOutQuart());
    A.To(0, 2, Animate<float>::easeInQuint   ());
    A.To(50, 2, Animate<float>::easeOutQuint  ());
    A.To(0, 2, Animate<float>::easeBackEase());
    A.To(50, 2, Animate<float>::easeInOutQuint());
    A.To(0, 2, Animate<float>::easeInQuad    ());
    Run_Const(A);

    return 0;
}


// Since Animate.Get() changes the internal structure, if you are passing a const Animate
// to a function, you can use the Animate::ConstSampler class to obtain the values
// It works just like Animate.get()
void Run_Const( const gnl::Animate<float> & constanimate)
{
    gnl::Animate<float>::ConstSampler A(constanimate);

    while( !A.Stable() )
    {
        float value = A.get();

        std::string bar( 30 + (int)value,' '); // A.get() does not change the internal structure here.
        bar += '#';

        std::cout << bar << std::endl;

        std::this_thread::sleep_for( std::chrono::milliseconds(10) );

    }
}
