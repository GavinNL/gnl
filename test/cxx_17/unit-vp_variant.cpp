#include <catch2/catch.hpp>

#include <gnl/vp_variant.h>
#include <stdio.h>
#include <iostream>

using namespace gnl;

TEST_CASE("Assigning a fundamental type to the vp_variant")
{
    auto X = std::make_shared<int>(10);
    std::shared_ptr<const int> Y = X;

    using var = vp_variant<int>;

    var x;

    x = 3;

    REQUIRE( std::holds_alternative<var::value_type>(x.get_variant()) );
    REQUIRE( std::get<var::value_type>(x.get_variant()) == 3);
    REQUIRE( x == 3);

    WHEN("the vp_variant can be converted into its value_type")
    {
        int y = x;

        REQUIRE(y==3);
    }

    THEN("The vp_variant can be compared")
    {
        REQUIRE( x <  5);
        REQUIRE( x <= 5);
        REQUIRE( x >  0);
        REQUIRE( x >= 0);

        REQUIRE( x >= 3);
        REQUIRE( x <= 3);
    }

}

TEST_CASE("Linking a variant")
{
     using var = vp_variant<int>;

     var x;
     var y;

     x = 3;

     REQUIRE( std::get<int>(x.get_variant()) == 3);
     REQUIRE( x == 3);

     WHEN("We link y = link(x) ")
     {
         y = link(x);

         REQUIRE( x.is_linked() == true );
         REQUIRE( y.is_linked() == true );

         REQUIRE( x.is_stack() == false );
         REQUIRE( y.is_stack() == false );

         THEN("The variants of both are now shared pointers")
         {
            REQUIRE( std::holds_alternative<var::pointer_type>(x.get_variant()) );
            REQUIRE( std::holds_alternative<var::const_pointer_type>(y.get_variant()) );
         }

         THEN("x and y will be == to each other")
         {
            REQUIRE( x == y );
         }

         WHEN("We change the value x")
         {
             x = 5;

             THEN("y will change as well")
             {
                REQUIRE( x == 5 );
                REQUIRE( y == 5 );
                REQUIRE( x < 10 );
             }
         }

         WHEN("We change the value y")
         {
             y = 10;

             THEN("y has been converted into a value_type")
             {
                 REQUIRE( std::holds_alternative<var::value_type>(y.get_variant()) );
             }
             THEN("x will NOT change but y will.")
             {
                REQUIRE( x == 3 );
                REQUIRE( y == 10 );
                REQUIRE( x != y );
             }
         }
     }
}



