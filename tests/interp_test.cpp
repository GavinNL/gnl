#include <gnl/gnl_interp.h>
#include <vector>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <string>

TEST_CASE( "Testing Parsing of Unquoted Keys" )
{

    std::vector<float> t = {0,1,2,3};
    std::vector<float> y = {0,1,2,3};

    gnl::linear_spline<float> L(t,y);

    REQUIRE( fabs(L(0.0) -  0.0) < 0.0001 );
    REQUIRE( fabs(L(1.0) -  1.0) < 0.0001 );
    REQUIRE( fabs(L(2.0) -  2.0) < 0.0001 );
    REQUIRE( fabs(L(3.0) -  3.0) < 0.0001 );

    REQUIRE( fabs(L(1.5) -  1.5) < 0.0001 );
    REQUIRE( fabs(L(2.5) -  2.5) < 0.0001 );
    REQUIRE( fabs(L(4)   -  2.5) < 0.0001 );

}
