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

template<>
inline JSON::operator vec3()  const
{
    if( type() == JSON::ARRAY)
        return { this->get(0), this->get(1), this->get(2) };

    return {0,0,0};
}

}



template<typename T>
bool Casting(T  J)
{
    std::cout << J << std::endl;
    return true;
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


    gnl::JSON json;



    json.parse(raw_noquote);


    json = "String";      assert( json.type() == gnl::JSON::STRING);
    json = 3.2f;          assert( json.type() == gnl::JSON::NUMBER);
    json = true;          assert( json.type() == gnl::JSON::BOOL  );
    json = {3.4f,"5.5f"}; assert( json.type() == gnl::JSON::ARRAY );
   // O["sub"] = 32.3f;  assert( O.type() == gnl::JSON::OBJECT);


    assert( json["array2"][0] == 1 );
    assert( json["array2"][1] == 2 );
    assert( json["array2"][2] == 3 );
    assert( json["array2"][3] == 4 );
    assert( json["array2"][4] == 5 );
    assert( json["array2"][5] == 6 );
    assert( json["array2"][6] == 7 );
    assert( json["array2"][7] == 8 );
    assert( json["array2"][8] == 9 );

    int x = json["number"];
    assert(  x == 3 );
    std::string s = json["string"];
    assert( s == "hello" );


    vec3 V;
    V = json["array2"];

    assert(V.x==1);
    assert(V.y==2);
    assert(V.z==3);


    assert(V.z!=4);


    json["three"]    = 3.0f;
    assert( json["three"] < 4.0f);
    assert( json["three"] > 2.0f);
    json["foo"]    = "foo";

    assert( json["foo"] > "bar" );

    json["bool"]   =  true;
    json["float"]  =  3.15f;
    json["string"] = "string";

    assert( json["bool"]   != false);
    assert( json["float"]  != 4.15f);
    assert( json["string"] != "ssdtring");

    Casting<bool>(  json["bool"]  ) ;
    Casting<float>( json["float"] ) ;
    Casting<int>(   json["float"] ) ;

    Casting<std::string>( json["string"] ) ;

    std::cout << json << std::endl;

    return 0;

}

