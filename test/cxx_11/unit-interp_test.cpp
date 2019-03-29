#include <gnl/interp.h>
#include <vector>

#include<catch2/catch.hpp>
#include <string>

TEST_CASE( "Testing Parsing of Unquoted Keys" )
{

    std::vector<float> t = {0,1,2,3};
    std::vector<float> y = {0,1,2,3};

    gnl::linear_spline<float> L(t,y);
    gnl::linear_spline2<float> L2(t,y);

    REQUIRE( L(0.0) == Approx(0.0L));

    REQUIRE( L(0.0)  == Approx( 0.0 )  );
    REQUIRE( L(1.0)  == Approx( 1.0 )  );
    REQUIRE( L(2.0)  == Approx( 2.0 )  );
    REQUIRE( L(3.0)  == Approx( 3.0 )  );

    REQUIRE( L(1.5)  == Approx( 1.5 )  );
    REQUIRE( L(2.5)  == Approx( 2.5 )  );
                                        ;
    REQUIRE( L2(0.0) == Approx( 0.0 )  );
    REQUIRE( L2(1.0) == Approx( 1.0 )  );
    REQUIRE( L2(2.0) == Approx( 2.0 )  );
    REQUIRE( L2(3.0) == Approx( 3.0 )  );

    REQUIRE( L2(1.5) == Approx( 1.5 )  );
    REQUIRE( L2(2.5) == Approx( 2.5 )  );

    REQUIRE_THROWS( L2.at(8.0) );


}
