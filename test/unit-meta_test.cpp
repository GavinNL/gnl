#include <gnl/gnl_meta.h>

#include<catch2/catch.hpp>
#include <string>

TEST_CASE( "Testing Compile Time functions" )
{

    static_assert(  type_size<float,char,double,long double>::max == sizeof(long double) , "failed" );
    static_assert(  type_size<float,char,double,long double>::min == sizeof(char) , "failed" );
    static_assert(  type_size<float,char,double,long double>::sum ==  sizeof(float)
                                                                    + sizeof(char)
                                                                    + sizeof(long double)
                                                                    + sizeof(double), "failed" );



    static_assert( index_in_list<int,     int, float, double>::value  == 0 , "failed");
    static_assert( index_in_list<float,   int, float, double>::value  == 1 , "failed");
    static_assert( index_in_list<double,  int, float, double>::value  == 2 , "failed");


    static_assert( type_in<double,  int, float, double>::value , "failed");
    static_assert( !type_in<char,  int, float, double>::value , "failed");


    #ifdef _WIN32
        #define TYPENAME
    #else
        #define TYPENAME typename
    #endif

    using first_type = TYPENAME first_type_of<char,  int, float, double>::type;
    using zero       = TYPENAME nth_type_of<0, char,  int, float, double>::type;
    using first      = TYPENAME nth_type_of<1, char,  int, float, double>::type;
    using second     = TYPENAME nth_type_of<2, char,  int, float, double>::type;
    using third      = TYPENAME nth_type_of<3, char,  int, float, double>::type;
    using last_type  = TYPENAME last_type_of<char,  int, float, double>::type;

    static_assert(  std::is_same<zero,   char>::value , "failed");
    static_assert(  std::is_same<first,  int>::value , "failed");
    static_assert(  std::is_same<second, float>::value , "failed");
    static_assert(  std::is_same<third,  double>::value , "failed");

    static_assert(  std::is_same<first_type,  char>::value , "failed");
    static_assert(  std::is_same<last_type,   double>::value , "failed");

}
