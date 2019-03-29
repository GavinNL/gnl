#include<catch2/catch.hpp>

#include <gnl/gnl_json.h>
#include <string>

TEST_CASE( "Testing Parsing of Unquoted Keys" )
{

    std::string raw_noquote =
                        R"del(
                            {
                                array2 : [1,2,3,4,5,6,7,8,9],
                                number : 3.114,
                                string : "hello",
                                array  : [3,1,3,5],
                                object : {
                                            num : 32
                                         }
                            }
                        )del";
    gnl::json json;
    json.parse(raw_noquote);

    REQUIRE( json.type() == gnl::json::OBJECT);

    REQUIRE( json["number"].type() == gnl::json::NUMBER);
    REQUIRE( json["string"].type() == gnl::json::STRING);
    REQUIRE( json["array" ].type() == gnl::json::ARRAY);
    REQUIRE( json["object"].type() == gnl::json::OBJECT);

    REQUIRE( json.size() == 5);



    REQUIRE( (json["number"] == 3.114f));
    REQUIRE( (json["string"] == "hello"));

    REQUIRE( (json["array"].size() == 4 ) );
    REQUIRE( (json["array" ][0]    == 3 ) );
    REQUIRE( (json["array" ][1]    == 1 ) );
    REQUIRE( (json["array" ][2]    == 3 ) );
    REQUIRE( (json["array" ][3]    == 5 ) );

    REQUIRE( json["object"]["num"].type() == gnl::json::NUMBER );
    REQUIRE( (json["object"]["num"] == 32) );

}

TEST_CASE( "Testing Parsing of Quoted Keys" )
{

    std::string raw_quote =
                        R"del(
                            {
                                "array2" : [1,2,3,4,5,6,7,8,9],
                                "number" : 3.114,
                                "string" : "hello",
                                "array"  : [3,1,3,5],
                                "object" : {
                                            "num" : 32
                                         }
                            }
                        )del";
    gnl::json json;
    json.parse(raw_quote);

    REQUIRE( json.type() == gnl::json::OBJECT);

    REQUIRE( json["number"].type() == gnl::json::NUMBER);
    REQUIRE( json["string"].type() == gnl::json::STRING);
    REQUIRE( json["array" ].type() == gnl::json::ARRAY);
    REQUIRE( json["object"].type() == gnl::json::OBJECT);

    REQUIRE( json.size() == 5);



    REQUIRE( (json["number"] == 3.114f));
    REQUIRE( (json["string"] == "hello"));

    REQUIRE( (json["array"].size() == 4 ) );
    REQUIRE( (json["array" ][0]    == 3 ) );
    REQUIRE( (json["array" ][1]    == 1 ) );
    REQUIRE( (json["array" ][2]    == 3 ) );
    REQUIRE( (json["array" ][3]    == 5 ) );

    REQUIRE( json["object"]["num"].type() == gnl::json::NUMBER );
    REQUIRE( (json["object"]["num"] == 32) );

}


TEST_CASE( "Testing Conversion to Other Types" )
{

    gnl::json json;

    json["number"] = 3.114f;
    json["string"] = "hello";
    json["array" ] = {3,1,3.0f,5},
    json["object"] = {  {3.0f, 1, 3.2}, "hello" };


    REQUIRE( json.type()           == gnl::json::OBJECT);
    REQUIRE( json["number"].type() == gnl::json::NUMBER);
    REQUIRE( json["string"].type() == gnl::json::STRING);
    REQUIRE( json["array" ].type() == gnl::json::ARRAY);
    REQUIRE( json["object"].type() == gnl::json::ARRAY);

    REQUIRE( json.size() == 4);
    REQUIRE( (json["number"] == 3.114f));
    REQUIRE( (json["string"] == "hello"));

    REQUIRE( (json["array"].size() == 4 ) );
    REQUIRE( (json["array" ][0]    == 3 ) );
    REQUIRE( (json["array" ][1]    == 1 ) );
    REQUIRE( (json["array" ][2]    == 3 ) );
    REQUIRE( (json["array" ][3]    == 5 ) );


    REQUIRE( (json["object"][0].type() == gnl::json::ARRAY ) );

    REQUIRE( (json["object"][0][0] == 3 ) );

    REQUIRE( (json["object"][1].type() == gnl::json::STRING) );


}

TEST_CASE( "Implicit casting to base types" )
{

    gnl::json json;

    json["number"] = 3;
    json["string"] = "hello";
    json["array" ] = {3,1,3.0f,5};

    float       x = json["number"];
    int         y = json["number"];
    std::string s = json["string"];

    REQUIRE( s == std::string("hello") );
    REQUIRE( fabs(x-3.0f) <= 1e-6 );
    REQUIRE( y == 3 );

}


TEST_CASE( "Operators" )
{
    gnl::json json;
    gnl::json json2;

    json["number"] = 3;
    json["string"] = "hello";
    json["bool"]   = false;


    REQUIRE( (json["number"] == 3 ));
    REQUIRE( (json["string"] == std::string("hello") ));
    REQUIRE( (json["bool"]   == false ));

    json2 = json;
    REQUIRE( (json == json2) );

    REQUIRE( (json["number"] != 4.f ));
    REQUIRE( (json["string"] != std::string("bye") ));
    REQUIRE( (json["bool"]   != true ));


    REQUIRE( (json["number"] < 4.f ));
    REQUIRE( (json["number"] < 4   ));
    REQUIRE( (json["string"] < std::string("iello") ));
    REQUIRE( (json["bool"]   < true ));

    REQUIRE( (json["number"] <= 4.f ));
    REQUIRE( (json["number"] <= 4   ));
    REQUIRE( (json["string"] <= std::string("iello") ));
    REQUIRE( (json["bool"]   <= true ));

    REQUIRE( (json["number"] <= 3 ));
    REQUIRE( (json["string"] <= std::string("hello") ));
    REQUIRE( (json["bool"]   <= false ));


    json["bool"]   = true;
    REQUIRE( (json["number"] > 2.f ));
    REQUIRE( (json["number"] > 2   ));
    REQUIRE( (json["string"] > std::string("gello") ));
    REQUIRE( (json["bool"]   > false ));

    REQUIRE( (json["number"] >= 3 ));
    REQUIRE( (json["string"] >= std::string("hello") ));
    REQUIRE( (json["bool"]   >= true));

    REQUIRE( (json["number"] >= 2.f ));
    REQUIRE( (json["number"] >= 2   ));
    REQUIRE( (json["string"] >= std::string("gello") ));
    REQUIRE( (json["bool"]   >= true ));

}


TEST_CASE( "Move Operator" )
{
    gnl::json json;
    gnl::json json2;

    json["number"] = 3;
    json["string"] = "hello";
    json["bool"]   = false;

    json2 = std::move(json);

    REQUIRE( (json2["number"] == 3 ));
    REQUIRE( (json2["string"] == std::string("hello") ));
    REQUIRE( (json2["bool"]   == false ));
    REQUIRE( json.type() == gnl::json::BOOL );
}
