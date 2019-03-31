#include <gnl/data_store.h>
#include <iostream>
#include <gnl/singleton.h>
#include <catch2/catch.hpp>


using namespace gnl;

struct MyData_t
{
    int x;
};

SCENARIO("Adding data to the data_store")
{
    GIVEN("An empty data store")
    {
        data_store D;

        WHEN("We add data to it")
        {
            MyData_t d;
            d.x = 1;

            D["name"] = d;

            THEN("The data is stored with it")
            {
                REQUIRE( D.has<MyData_t>("name") );
                REQUIRE( D.get<MyData_t>("name").x == 1);
            }
        }
    }
}


