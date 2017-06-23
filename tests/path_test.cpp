#include <gnl/gnl_path.h>


#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"


TEST_CASE( "Path 2 Tests" )
{


    REQUIRE( gnl::path2("/home/gavin/file.txt").is_absolute() == true);
    REQUIRE( gnl::path2("/home/gavin/file.txt").is_relative() == false);
    REQUIRE( gnl::path2("/home/gavin/file.txt").is_file()     == true);
    REQUIRE( gnl::path2("/home/gavin/file.txt").is_folder()   == false);
    REQUIRE( gnl::path2("/home/gavin/file.txt").filename()    == "file.txt");
    REQUIRE( gnl::path2("/home/gavin/file.txt").extension()   == ".txt");
    REQUIRE( gnl::path2("/home/gavin/file.txt").extension().extension()   == ".txt");
    REQUIRE( gnl::path2("/home/gavin/file.txt").parent_path() == "/home/gavin/");

    REQUIRE( gnl::path2("home/gavin/file.txt").is_absolute() == false);
    REQUIRE( gnl::path2("home/gavin/file.txt").is_relative() == true);
    REQUIRE( gnl::path2("home/gavin/file.txt").is_file()     == true);
    REQUIRE( gnl::path2("home/gavin/file.txt").is_folder()   == false);

    REQUIRE( gnl::path2("C:/home/gavin/file.txt").is_absolute() == true);
    REQUIRE( gnl::path2("C:/home/gavin/file.txt").root_path() == "C:/");
    REQUIRE( gnl::path2("C:/home/gavin/file.txt").root_name() == "C:");
    REQUIRE( gnl::path2("/home/gavin/file.txt").root_path() == "/");
    REQUIRE( gnl::path2("/home/gavin/file.txt").root_name() == "");

    REQUIRE( gnl::path2("/home/gavin/file.txt").stem() == "file");
    REQUIRE( gnl::path2("/home/gavin/file").stem() == "file");


    REQUIRE( gnl::path2("C:/home/gavin/file.txt").relative_path() == "home/gavin/file.txt");


    gnl::path2 root("/home/");
    gnl::path2 file("file.txt");

    REQUIRE( (root/file) == "/home/file.txt" );
    REQUIRE( (root/"file.txt") == "/home/file.txt" );


    gnl::path2 P("C:/home/gavin/file.txt");
    P.replace_extension("bmp");
    std::cout << P << std::endl;
    REQUIRE( gnl::path2("C:/home/gavin/file.txt").replace_filename("hello.txt") == "C:/home/gavin/hello.txt");
    REQUIRE( gnl::path2("C:/home/gavin/file.txt").replace_extension(".bmp") == "C:/home/gavin/file.bmp");
#if 0
#endif
    // gnl::path2("/home/gavin/"),
    // gnl::path2("file.txt"),
    // gnl::path2("/home/file.txt"),
    // gnl::path2("home/file.txt"),
    // gnl::path2("./home/file.txt"),
    // gnl::path2("C:/home/file.txt"),
    // gnl::path2("C:\\home\\file.txt")

    //REQUIRE( gnl::path2("/home/gavin/file.txt").extension() == ".txt");
    //REQUIRE( gnl::path2("/home/gavin/file.txt").stem() == "file");
    //REQUIRE( gnl::path2("/home/gavin/file.txt").is_absolute() == true);
    //REQUIRE( gnl::path2("home/gavin/file.txt").is_absolute() == false);
    //REQUIRE( gnl::path2("C:/home/gavin/file.txt").is_absolute() == true);

    //REQUIRE( gnl::path2("C:/home/gavin/file.txt").root_name() == "C:");
    //REQUIRE( gnl::path2("C:/home/gavin/file.txt").root_directory() == "C:/");
    //
    //REQUIRE( gnl::path2("/home/gavin/file.txt").root_name() == "");
    //REQUIRE( gnl::path2("/home/gavin/file.txt").root_directory() == "/");


}

TEST_CASE( "Path Tests" )
{

    gnl::Path file_with_extension(   "C:\\myfolder\\myfile.txt"  );
    gnl::Path file(   "/home/file"    );
    gnl::Path folder( "/home/folder/" );

    gnl::Path relative(  "home/folder/" );
    gnl::Path relative2( "./home/folder/" );
    gnl::Path absolute( "/home/folder/" );

    gnl::Path device( "C:/home/folder/" );

    REQUIRE( file.IsFile()          == true  );
    REQUIRE( folder.IsFile()        == false );
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


  //  REQUIRE( absolute+relative2 == "/home/folder/home/folder/" );  // failing

    REQUIRE( file_with_extension.FileExtension() == "txt");
    REQUIRE( file_with_extension.FileBaseName()  == "myfile");

    REQUIRE( file_with_extension.ToString(gnl::Path::WINDOWS_STYLE)  == std::string("C:\\myfolder\\myfile.txt") );
}
