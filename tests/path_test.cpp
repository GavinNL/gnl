#include <gnl/gnl_path.h>


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"


TEST_CASE( "is_absolute" )
{
    REQUIRE( gnl::path("/home/gavin/file.txt").is_absolute() == true);
    REQUIRE( gnl::path("/home/gavin/").is_absolute() == true);
    REQUIRE( gnl::path("C:/home/gavin/file.txt").is_absolute() == true);
    REQUIRE( gnl::path("C:/home/gavin/").is_absolute() == true);
    REQUIRE( gnl::path("C:\\home\\gavin\\file.txt").is_absolute() == true);

    REQUIRE( gnl::path("home\\gavin\\file.txt").is_absolute() == false);
    REQUIRE( gnl::path("home\\gavin\\").is_absolute() == false);
}

TEST_CASE( "is_file" )
{
    REQUIRE( gnl::path("/home/gavin/").is_file() == false);
    REQUIRE( gnl::path("C:/home/gavin/").is_file() == false);
    REQUIRE( gnl::path("home\\gavin\\").is_file() == false);

    REQUIRE( gnl::path("/home/gavin/file.txt").is_file() == true);
    REQUIRE( gnl::path("C:/home/gavin/file.txt").is_file() == true);
    REQUIRE( gnl::path("C:\\home\\gavin\\file.txt").is_file() == true);
    REQUIRE( gnl::path("home\\gavin\\file.txt").is_file() == true);
}


TEST_CASE( "extension" )
{
    REQUIRE( gnl::path("/home/gavin/file.txt").extension()               == ".txt");
    REQUIRE( gnl::path("/home/gavin/file.txt").extension().extension()   == ".txt");
    REQUIRE( gnl::path("/home/gavin/file.txt/").extension()   == "");
}

TEST_CASE( "stem" )
{
    REQUIRE( gnl::path("/home/gavin/file.txt").stem() == "file");
    REQUIRE( gnl::path("/home/gavin/file").stem() == "file");
    REQUIRE( gnl::path("/home/gavin/file.tar.gz").stem() == "file.tar");
}

TEST_CASE( "parent_path" )
{
    REQUIRE( gnl::path("/home/gavin/file.txt").parent_path() == "/home/gavin/");
    REQUIRE( gnl::path("/home/gavin/").parent_path() == "/home/");
    REQUIRE( gnl::path("").parent_path() == "");
}

TEST_CASE( "root_path" )
{
    REQUIRE( gnl::path("C:/home/gavin/file.txt").root_path() == "C:/");
    REQUIRE( gnl::path("C:/home/gavin/file.txt").root_name() == "C:");
    REQUIRE( gnl::path("/home/gavin/file.txt").root_path() == "/");
    REQUIRE( gnl::path("/home/gavin/file.txt").root_name() == "");

    REQUIRE( gnl::path("gavin/file.txt").root_name() == "");
    REQUIRE( gnl::path("gavin/file.txt").root_path() == "");
}


TEST_CASE( "relative_path" )
{
    REQUIRE( gnl::path("C:/home/gavin/file.txt").relative_path() == "home/gavin/file.txt");
    REQUIRE( gnl::path("/home/gavin/file.txt").relative_path() == "home/gavin/file.txt");
}

TEST_CASE( "append" )
{
    gnl::path root("/home/");
    gnl::path file("file.txt");

    REQUIRE( (root/file) == "/home/file.txt" );
    REQUIRE( (root/"file.txt") == "/home/file.txt" );


    auto r = root;
    auto f = file;
    r /= file;

    REQUIRE( r == "/home/file.txt" );

    r = root;
    r /= file.to_string();
    REQUIRE( r == "/home/file.txt" );

}

TEST_CASE( "concat" )
{
    gnl::path root("/home/file");
    gnl::path file(".txt");

    REQUIRE( (root+file) == "/home/file.txt" );
    REQUIRE( (root+file) == "/home/file.txt" );

    auto r = root;
    auto f = file;
    r += file;

    REQUIRE( r == "/home/file.txt" );

    r = root;
    r += file.to_string();
    REQUIRE( r == "/home/file.txt" );

}

TEST_CASE("replace extension")
{
    gnl::path P("C:/home/gavin/file.txt");
    P.replace_extension("bmp");

    REQUIRE( gnl::path("C:/home/gavin/file.txt").replace_filename("hello.txt") == "C:/home/gavin/hello.txt");
    REQUIRE( gnl::path("C:/home/gavin/file.txt").replace_extension(".bmp") == "C:/home/gavin/file.bmp");

}
