#include <gnl/gnl_periodic.h>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <string>

int process(int * x)
{
    *x++;
    std::cout << *x << std::endl;
    return 0;
}

TEST_CASE( "Testing Period class" )
{
    int y = 0;

    {
        ///gnl::Periodic P;
        ///int x = 0;
        ///P.Start( std::chrono::seconds(1), 4, process, &x);
        ///std::this_thread::sleep_for( std::chrono::milliseconds(5000));
        ///y = x;
    }

    REQUIRE( y==4 );

}
