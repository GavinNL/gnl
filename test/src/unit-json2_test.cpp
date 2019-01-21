

#include <gnl/gnl_json2.h>

#include<catch2/catch.hpp>


TEST_CASE( "Copy assignment" )
{

    gnl::json J;
    gnl::json J2;

    J2 = gnl::json::array();

    J = 3;
    REQUIRE( J.type() == gnl::json::NUMBER);
    J = 4.0;
    REQUIRE( J.type() == gnl::json::NUMBER);
    J = 4.0f;
    REQUIRE( J.type() == gnl::json::NUMBER);
    J = std::string("hello");
    REQUIRE( J.type() == gnl::json::STRING);
    J = true;
    REQUIRE( J.type() == gnl::json::BOOLEAN);

    J = gnl::json::array();
    REQUIRE( J.type() == gnl::json::ARRAY);
    J = gnl::json::object();
    REQUIRE( J.type() == gnl::json::OBJECT);

    REQUIRE( J2.type() == gnl::json::ARRAY);
    J = J2;
    REQUIRE( J.type() == gnl::json::ARRAY);

}

TEST_CASE( "Copy constructor" )
{

    gnl::json Ji(3);
    gnl::json Jd(3.0);
    gnl::json Jf(3.0f);
    gnl::json Js(std::string("hello"));
    gnl::json Jb(true);


    REQUIRE( Ji.type() == gnl::json::NUMBER);
    REQUIRE( Jd.type() == gnl::json::NUMBER);
    REQUIRE( Jf.type() == gnl::json::NUMBER);
    REQUIRE( Js.type() == gnl::json::STRING);
    REQUIRE( Jb.type() == gnl::json::BOOLEAN);

}



TEST_CASE("Move operator")
{
    gnl::json J;

    std::string name("Gavin");

    J = std::move(name);
    REQUIRE( (J == "Gavin"));
    REQUIRE(( name == "") );
}

SCENARIO("Number Types")
{
    GIVEN("A JSON constructed with an int")
    {
        gnl::json J(3);

        THEN("size == 0")
        {
            REQUIRE( J.size() == 0);
        }

        THEN("The type is number (double)")
        {
            REQUIRE( J.type() == gnl::json::NUMBER );
        }

        THEN("Can be accessed using as()")
        {
           // REQUIRE( J.as<gnl::json::number>() == 3.0 );
        }

        THEN("Can be converted into an int using get")
        {
            REQUIRE( J.get<int>() == 3 );
        }

        THEN("Can be converted into an unsigned int using get")
        {
            REQUIRE( J.get<unsigned int>() == 3u );
        }

        THEN("Can be compared to an integer")
        {
            //REQUIRE( (J == 3) );
            REQUIRE( (J >= 3) );
            REQUIRE( (J <= 3) );
            //REQUIRE( (J != 4) );
            REQUIRE( (J <  4) );
            REQUIRE( (J >  2) );
        }

        THEN("Can be compared to a double")
        {
            //REQUIRE( (J == 3.0)  );
            REQUIRE( (J >= 3.0)  );
            REQUIRE( (J <= 3.0)  );
            //REQUIRE( (J != 4.0)  );
            REQUIRE( (J <  4.0)  );
            REQUIRE( (J >  2.0)  );
        }

        THEN("Can be cast to an int")
        {
            int j = J;
            REQUIRE( j == 3  );
        }
        THEN("Can be casted to an unsigned int")
        {
            unsigned int j = J;
            REQUIRE( j == 3u  );
        }

    }
}

SCENARIO("String Types")
{
    GIVEN("A JSON constructed with a raw string")
    {
        gnl::json J("hello");

        THEN("size == 0")
        {
            REQUIRE( J.size() == 5);
        }

        THEN("The type is string")
        {
            REQUIRE( J.type() == gnl::json::STRING );
        }

        THEN("Can be accessed using as()")
        {
            REQUIRE( J.as<gnl::json::string>() == std::string("hello") );
            REQUIRE( J.as<gnl::json::string>() == gnl::json::string("hello") );
            REQUIRE( J.as<gnl::json::string>() == "hello" );
        }

        THEN("accessing using a non-string type will throw expcetions")
        {
            CHECK_THROWS( J.as<gnl::json::number>()  );//== gnl::json::number() );
            CHECK_THROWS( J.as<gnl::json::boolean>() );//== gnl::json::boolean() );
            CHECK_THROWS( J.as<gnl::json::array>()   );//== gnl::json::array() );
            CHECK_THROWS( J.as<gnl::json::object>()   );//== gnl::json::array() );
        }

        THEN("Can be converted into an int using get, but returns a default constructed int")
        {
            REQUIRE( J.get<int>() == int() );
        }

        THEN("Can be converted into an unsigned int using get")
        {
           // REQUIRE( J.get<unsigned int>() == unsigned int() );
        }

        THEN("Can be compared to string types")
        {
            REQUIRE( ( J == std::string("hello") ));
            REQUIRE( ( J == gnl::json::string("hello") ));
            REQUIRE( ( J == "hello" ));
            REQUIRE( ( J <= std::string("hello") ));
            REQUIRE( ( J <= gnl::json::string("hello") ));
            REQUIRE( ( J <= "hello" ));
            REQUIRE( ( J >= std::string("hello") ));
            REQUIRE( ( J >= gnl::json::string("hello") ));
            REQUIRE( ( J >= "hello" ));
            REQUIRE( ( J <= std::string("zello") ));
            REQUIRE( ( J <= gnl::json::string("zello") ));
            REQUIRE( ( J <= "zello" ));
            REQUIRE( ( J >= std::string("aello") ));
            REQUIRE( ( J >= gnl::json::string("aello") ));
            REQUIRE( ( J >= "aello" ));
        }
        THEN("Comparing to non-string types will throw an exception")
        {
            //CHECK_THROWS(J == 3  );
            //CHECK_THROWS(J == 3.0);
            CHECK_THROWS(J == true);
            CHECK_THROWS(J == false);
        }
    }
}
SCENARIO("ARRAYS")
{
    GIVEN("A uninitialized json type")
    {
        gnl::json J;

        THEN("Accessing an elemnt converts it into an array")
        {
            J[0];
            REQUIRE( J.type() == gnl::json::ARRAY);
        }
    }

    GIVEN("A json Array type")
    {
        gnl::json J;
        J[0] = 3;
        J[1] = "gavin";

        THEN( "size == 2")
        {
            REQUIRE( J.size() == 2 );
        }

        THEN("using get with incorrect types")
        {
            REQUIRE( J.get(1, std::string("wolf") ) == "gavin");
            REQUIRE( J.get(2, std::string("wolf") ) == "wolf");
        }
    }
}

SCENARIO("OBJECT")
{
    GIVEN("A Given a json object type")
    {
        gnl::json J;
        J[ "firstname" ] = "Gavin";
        J[ "age" ]       = 33;

        THEN("Accessing an element converts it into an array")
        {
            REQUIRE( J.type() == gnl::json::OBJECT);
        }
        THEN( "size == 2")
        {
            REQUIRE( J.size() == 2 );
        }
        THEN("Using get with default values")
        {
            REQUIRE( J.get("firstname", std::string("nigel") ) == "Gavin");
            REQUIRE( J.get("age"      , 100 )                  == 33);
            REQUIRE( J.get("lastname" , std::string("Wolf")  ) == "Wolf");

        }
    }


}
