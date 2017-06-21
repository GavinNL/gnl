#include <iostream>
#include <gnl/gnl_path.h>
#include <assert.h>
#include <thread>


#include <cstdlib>

using namespace std;


int x[] ={1,2,3,4,5};

int main()
{

    auto * Fi = gnl::path2::fopen( gnl::path2("/tmp/test/test/test/file.txt"), "w");
    std::cout << Fi << std::endl;
    std::fclose(Fi);

    std::cout << gnl::path2::mkdir( gnl::path2("/usr/bin/testfolder/testsubfolder/") ) << std::endl;

    std::cout << gnl::path2::home_dir() << std::endl;
    std::cout << gnl::path2::temp_dir() / gnl::path2::temp_dir() << std::endl;
    return 0;
    gnl::path2 paths[] = {
        gnl::path2(),
        //gnl::path2("/home/gavin/"),
        //gnl::path2("file.txt"),
        //gnl::path2("/home/file.txt"),
        //gnl::path2("home/file.txt"),
        //gnl::path2("./home/file.txt"),
        //gnl::path2("C:/home/file.txt"),
        //gnl::path2("C:\\home\\file.txt")
    };

    for(auto & p : paths)
    {
        std::cout << p << "::filename    :" << p.filename() << std::endl;
        std::cout << p << "::is_absolute :" << p.is_absolute() << std::endl;
        std::cout << p << "::is_relative :" << p.is_relative() << std::endl;
        std::cout << p << "::is_file     :" << p.is_file() << std::endl;
        std::cout << p << "::is_folder   :" << p.is_folder() << std::endl;
        std::cout << p << "::extension   :" << p.extension() << std::endl;
        std::cout << p << "::parent_path :" << p.parent_path() << std::endl;
    }
    gnl::path2 p("/home/gavin/file.txt");
    return 0;
    std::cout << " extension  : " << p.extension() << std::endl;
    std::cout << " stem       : " << p.stem()      << std::endl;
    std::cout << " is relative: " << p.is_relative() << std::endl;
    std::cout << " is absolute: " << p.is_absolute() << std::endl;

    std::cout << p << std::endl;
   // std::cout << *p.begin() << std::endl;
    return 0;
    int * y = x;

    y = y+2;
    std::cout << *y << std::endl;

    std::cout << gnl::Path::Home() << std::endl;
    std::cout << gnl::Path::Temp() << std::endl;


    gnl::path2 P("/home/gavin/tempfile.txt/");

//    std::cout << P.filename() << std::endl;
//    std::cout << gnl::path2("/foo/bar.txt").filename() << '\n'
//              << gnl::path2("/foo/.bar").filename() << '\n'
//              << gnl::path2("/foo/bar/").filename() << '\n'
//              << gnl::path2("/foo/.").filename() << '\n'
//              << gnl::path2("/foo/..").filename() << '\n'
//              << gnl::path2(".").filename() << '\n'
//              << gnl::path2("..").filename() << '\n'
//              << gnl::path2("/").filename() << '\n';

    //auto f = gnl::path2("..");

    std::vector<gnl::path2> F = { gnl::path2("C:/"), gnl::path2("/usr/home") };
    for(auto & f : F)
    {
        std::cout << " root path " << f.root_path() << std::endl;
        std::cout << " relative path" << f.relative_path() << std::endl;
        std::cout << " parent path" << f.parent_path() << std::endl;
        std::cout << " extension " << f.extension() << std::endl;
        std::cout << "----" << std::endl;
    }

    return 0;


}



