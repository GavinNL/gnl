#include <catch2/catch.hpp>

#include <gnl/gnl_resource_path.h>
#include <stdio.h>
#include <iostream>

using namespace gnl;

SCENARIO( "Absolute and Relative Path tests" )
{
    gnl::gnl_resource_path R;


    GIVEN("A set of absolute/relative paths")
    {
        THEN("is_absolute( ) should correctly identify them.")
        {
            REQUIRE( R.is_absolute("/home/gavin") == true);
            REQUIRE( R.is_absolute("/")           == true);
            REQUIRE( R.is_absolute("c:")          == true);
            REQUIRE( R.is_absolute("c:/")         == true);
            REQUIRE( R.is_absolute("")            == false);
        }
    }
}

SCENARIO("Format path tests")
{
    gnl::gnl_resource_path R;

    GIVEN("A set of paths with varous number of repeated separators")
    {
        THEN("format_path() should correctly format the path into a standard form")
        {
            REQUIRE( R.format_path("//home/gavin/")             == "/home/gavin/");
            REQUIRE( R.format_path("\\home/gavin\\")            == "/home/gavin/");
            REQUIRE( R.format_path("C:\\home/gavin\\")          == "C:/home/gavin/");
            REQUIRE( R.format_path("C:\\home/gavin")            == "C:/home/gavin");
            REQUIRE( R.format_path("C:\\///home///\\\\gavin")   == "C:/home/gavin");
        }

    }

}

SCENARIO( "join() tests" ) {

    gnl::gnl_resource_path R;

    GIVEN("A Root path folder and a stem ")
    {
        THEN("join() should be able to combine the two paths into a single path")
        {
            REQUIRE( "/home/gavin/test" == R.format_path("//home/gavin\\test") );

            REQUIRE( R.join("/home/gavin" , "file.txt") == "/home/gavin/file.txt");
            REQUIRE( R.join("/home/gavin/", "file.txt") == "/home/gavin/file.txt");

            REQUIRE( R.join("home/gavin/", "file.txt") == "home/gavin/file.txt");

        }
    }
    GIVEN("Two absolute paths")
    {
        THEN("join() should throw an error")
        {
            REQUIRE_THROWS( R.join("/home/gavin", "/home/gavin") );
        }
    }
}


SCENARIO( "OS File exists" )
{
    gnl::gnl_resource_path R;

#if defined __linux__
    GIVEN("A a set of known files on Linux")
    {
        THEN("fexists() should be able to correctly identify whether they exist")
        {
            REQUIRE(R.fexists("/dev/null") == true);
            REQUIRE(R.fexists("/tmp") == true);
            REQUIRE(R.fexists("/bin/ls") == true);

            REQUIRE(R.is_dir("/tmp") == true);
            REQUIRE(R.is_dir("/dev/null") == false);

        }
    }
    GIVEN("A a set of known folders on Linux")
    {
        THEN("is_dir() should be able to correctly identify them as folders")
        {
            REQUIRE(R.is_dir("/tmp")      == true);
            REQUIRE(R.is_dir("/dev/null") == false);
        }
    }
#endif

#if defined _WIN32

    GIVEN("A a set of known files on Windows")
    {
        THEN("fexists() should be able to correctly identify whether they exist")
        {
            REQUIRE(R.fexists("C:\\Windows") == true);
            REQUIRE(R.fexists("C:\\Windows\\system32") == true);

            REQUIRE(R.is_dir("C:\\Windows") == true);
            REQUIRE(R.is_dir("C:\\Windows\\system32") == true);

            REQUIRE(R.fexists("C:\\Windows\\system32\\cmd.exe") == true);
            REQUIRE(R.is_dir("C:\\Windows\\system32\\cmd.exe") == false);

        }
    }
#endif
}


SCENARIO( "Resource exists" )
{
    gnl::gnl_resource_path R;

#if defined __linux__

    GIVEN("A gnl_resource_path with folders added to the resource list")
    {
        R.add_path("/dev");
        R.add_path("/bin");
        R.add_path("/usr/bin");

        THEN("get() should correctly find the folders in which the files exist")
        {
            REQUIRE( R.get("null")                  == "/dev/null");
            REQUIRE( R.get("ls")                    == "/bin/ls");
            REQUIRE( R.get("jgvuyasekasdf.asdasdf") == "");

            std::cout << R.get_exe_path() << std::endl;
            std::cout << R.get_cwd() << std::endl;
        }
    }

#endif


}
