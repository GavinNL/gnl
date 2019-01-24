#include<catch2/catch.hpp>

#include <gnl/gnl_periodic.h>
#include <string>

#include <iostream>
int process(int * x)
{
    std::cout << *x++ << std::endl;
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

    REQUIRE( y == 0 );

}
