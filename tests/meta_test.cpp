#include <gnl/gnl_meta.h>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
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


    using first_type = typename first_type_of<char,  int, float, double>::type;
    using zero       = typename nth_type_of<0, char,  int, float, double>::type;
    using first      = typename nth_type_of<1, char,  int, float, double>::type;
    using second     = typename nth_type_of<2, char,  int, float, double>::type;
    using third      = typename nth_type_of<3, char,  int, float, double>::type;
    using last_type  = typename last_type_of<char,  int, float, double>::type;

    static_assert(  std::is_same<zero,   char>::value , "failed");
    static_assert(  std::is_same<first,  int>::value , "failed");
    static_assert(  std::is_same<second, float>::value , "failed");
    static_assert(  std::is_same<third,  double>::value , "failed");

    static_assert(  std::is_same<first_type,  char>::value , "failed");
    static_assert(  std::is_same<last_type,   double>::value , "failed");

}
