#include <gnl/gnl_binpacking.h>
#include <iostream>
#include <catch2/catch.hpp>

TEST_CASE( "Testing Period class" )
{
    int y = 0;

    gnl::Bin<int> B(100,100);

    {
        auto r = B.insert( 10, 10, 2);
        if(r)
        {
            std::cout << r.x << " "
                      << r.y << " "
                      << r.w << " "
                      << r.h << std::endl;
        }
    }

    {
        auto r = B.insert( 10, 10, 2);
        if(r)
        {
            std::cout << r.x << " "
                      << r.y << " "
                      << r.w << " "
                      << r.h << std::endl;
        }
    }

    REQUIRE( y == 0 );

}
