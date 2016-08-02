#include <iostream>

#include <gnl/gnl_json.h>
#include <assert.h>

using namespace std;


struct vec3
{
    float x,y,z;
};


namespace gnl
{
namespace json
{
template<>
inline Value::operator vec3()  const
{
    if( type() == Value::ARRAY)
        return { this->get(0), this->get(1), this->get(2) };

    return {0,0,0};
}
}
}


template<typename T>
bool Casting(T  J)
{
    std::cout << J << std::endl;
}

int main()
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


    std::string raw_quote =
                        R"del(
                            {
                                "array2" : [1,2,3,4,5,6,7,8,9],
                                "number" : 3.14,
                                "string" : "hello",
                                "array"  : [3,1,3,5],
                                "object" : {
                                            num : 32
                                         }
                            }
                        )del";
    gnl::json::Value O;

    //O.parse(raw);

    O = "String";      assert( O.type() == gnl::json::Value::STRING);
    O = 3.2f;          assert( O.type() == gnl::json::Value::NUMBER);
    O = true;          assert( O.type() == gnl::json::Value::BOOL);

    O = {3.4f,"5.5f"}; assert( O.type() == gnl::json::Value::ARRAY);
   // O["sub"] = 32.3f;  assert( O.type() == gnl::json::Value::OBJECT);

    O.parse(raw_noquote);

    assert( O["array2"][0] == 1 );
    assert( O["array2"][1] == 2 );
    assert( O["array2"][2] == 3 );
    assert( O["array2"][3] == 4 );
    assert( O["array2"][4] == 5 );
    assert( O["array2"][5] == 6 );
    assert( O["array2"][6] == 7 );
    assert( O["array2"][7] == 8 );
    assert( O["array2"][8] == 9 );

    int x = O["number"];
    assert(  x == 3 );
    std::string s = O["string"];
    assert( s == "hello" );


    vec3 V;
    V = O["array2"];

    assert(V.x==1);
    assert(V.y==2);
    assert(V.z==3);

    assert(V.z!=4);

    O["three"]    = 3.0f;
    assert( O["three"] < 4.0f);
    assert( O["three"] > 2.0f);
    O["foo"]    = "foo";

    assert( O["foo"] > "bar" );

    O["bool"]   = true;
    O["float"]  = 3.15f;
    O["string"] = "string";

    assert( O["bool"]   != false);
    assert( O["float"]  != 4.15f);
    assert( O["string"] != "ssdtring");

    Casting<bool>( O["bool"] ) ;
    Casting<float>( O["float"] ) ;
    Casting<int>( O["float"] ) ;
    Casting<std::string>( O["string"] ) ;

    std::cout << O << std::endl;

    return 0;

}

