

#include <gnl/gnl_json2.h>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"


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

TEST_CASE( "json strings" )
{

//    auto A = "hellp";
//    static_assert(std::is_convertible< decltype(A), std::string>::value, "");
//    static_assert(NUMERIC(decltype(A)), "");

    gnl::json Js(std::string("hello"));
    gnl::json Jc("hello");

    REQUIRE( Js.type() == gnl::json::STRING);
    REQUIRE( Jc.type() == gnl::json::STRING);

    REQUIRE( Js.as<gnl::json::string>()  == "hello");
    REQUIRE( Jc.as<gnl::json::string>()  == "hello");

    REQUIRE( Js.as<gnl::json::string>()  == std::string("hello") );
    REQUIRE( Jc.as<gnl::json::string>()  == std::string("hello") );

    static_assert( std::is_convertible<char*, std::string>::value, "");

    Js = std::string("hello");
    Jc = "hello";

    REQUIRE( Js.as<gnl::json::string>()  == std::string("hello") );
    REQUIRE( Jc.as<gnl::json::string>()  == std::string("hello") );

    REQUIRE( Js == std::string("hello") );
    REQUIRE( Jc == std::string("hello") );

    REQUIRE( Js == "hello" );
    REQUIRE( Jc == "hello" );
}

TEST_CASE("json numbers")
{
    gnl::json Ji(3);
    gnl::json Jd(3.0);
    gnl::json Jf(3.0f);

    REQUIRE( Ji.as<gnl::json::number>()  == 3);
    REQUIRE( Jd.as<gnl::json::number>()  == 3.0);
    REQUIRE( Jf.as<gnl::json::number>()  == 3.0f);

    Ji = 4;
    Jd = 4.0;
    Jf = 4.0f;

    REQUIRE( Ji.as<gnl::json::number>()  == 4);
    REQUIRE( Jd.as<gnl::json::number>()  == 4.0);
    REQUIRE( Jf.as<gnl::json::number>()  == 4.0f);

    REQUIRE( Ji == 4);
    REQUIRE( Jd == 4.0);
    REQUIRE( Jf == 4.0f);

    float  f = Jf;
    double d = Jf;
    int    i = Ji;

    REQUIRE( i == 4);
    REQUIRE( d == 4.0);
    REQUIRE( f == 4.0f);

    REQUIRE( Jf.as<gnl::json::number>() == 4.0f);
}

TEST_CASE("json arrays")
{
    gnl::json J;
    J[0] = 3;

    REQUIRE( J[0] == 3);

    J[4] = "hello";
    REQUIRE( J[4] == "hello");
    REQUIRE( J.size() == 5);

    REQUIRE( J.get(10, 88) == 88 );
}

TEST_CASE("json objects")
{
    gnl::json J;

    J["first_name"] = "Gavin";
    J["last_name"]  = "Wolf";
    J["height"] = 182;

    REQUIRE( J["height"] == 182);
    REQUIRE( J["first_name"] == "Gavin");
    REQUIRE( J["last_name"] == "Wolf");
    REQUIRE( J.get("middle_name", std::string("bob")) == "bob");
    REQUIRE( J.get("age", 33) == 33);
    REQUIRE( J.get("height", 5000) == 182);

    gnl::json::print(J["first_name"]);
    gnl::json::print(J["height"]);
    std::cout << std::endl;
}

TEST_CASE("Move operator")
{
    gnl::json J;

    std::string name("Gavin");

    J = std::move(name);
    REQUIRE( J == "Gavin");
    REQUIRE( name == "");

}
