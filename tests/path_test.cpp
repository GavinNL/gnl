#include <gnl/gnl_path.h>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"


TEST_CASE( "Path Tests" )
{

    gnl::Path file(   "/home/file"    );
    gnl::Path folder( "/home/folder/" );

    gnl::Path relative(  "home/folder/" );
    gnl::Path relative2( "./home/folder/" );
    gnl::Path absolute( "/home/folder/" );

    gnl::Path device( "C:/home/folder/" );

    REQUIRE( file.IsFile()         == true  );
    REQUIRE( folder.IsFile()       == false );
    REQUIRE( relative2.IsAbsolute() == false );
    REQUIRE( relative.IsAbsolute()  == false );
    REQUIRE( absolute.IsAbsolute()  == true  );


    REQUIRE( device.Device() == "C" );

    std::string S;
    S = file;
    REQUIRE( S == std::string("/home/file") );

    REQUIRE( file == std::string("/home/file") );
    REQUIRE( file == "/home/file" );


    REQUIRE( file.BasePath()       == "/home/" );
    REQUIRE( absolute.ParentPath() == "/home/" );

    REQUIRE( absolute+relative  == "/home/folder/home/folder/" );

    //relative2.Report();
    //auto p = absolute+relative2;
    //p.Report();
    //std::cout << (absolute+relative2) << std::endl;
    REQUIRE( absolute+relative2 == "/home/folder/home/folder/" );
}
