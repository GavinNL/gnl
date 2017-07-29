#include <iostream>
#include <gnl/gnl_path.h>
#include <assert.h>
#include <thread>


#include <cstdlib>

using namespace std;


int x[] ={1,2,3,4,5};

int main()
{

    std::cout << "Home Directory: " << gnl::path::home_dir() << std::endl;
    std::cout << "Temp Directory: " << gnl::path::temp_dir()  << std::endl;

    auto temp_dir = gnl::path::temp_dir() / "test_dir_123456789/";

    std::cout << "Creating new Directory: " << temp_dir << std::endl;
    std::cout << "               Success: " << gnl::path::mkdir(  temp_dir )  << std::endl;


    gnl::path paths[] =
    {
        gnl::path(),
        gnl::path("/home/gavin/"),
        gnl::path("file.txt"),
        gnl::path("/home/file.txt"),
        gnl::path("home/file.txt"),
        gnl::path("./home/file.txt"),
        gnl::path("C:/home/file.txt"),
        gnl::path("C:\\home\\file.txt")
    };

    for(auto & p : paths)
    {
        std::cout << "( " << p << " )" << "::filename    :" << p.filename() << std::endl;
        std::cout << "( " << p << " )" << "::is_absolute :" << p.is_absolute() << std::endl;
        std::cout << "( " << p << " )" << "::is_relative :" << p.is_relative() << std::endl;
        std::cout << "( " << p << " )" << "::is_file     :" << p.is_file() << std::endl;
        std::cout << "( " << p << " )" << "::is_folder   :" << p.is_folder() << std::endl;
        std::cout << "( " << p << " )" << "::extension   :" << p.extension() << std::endl;
        std::cout << "( " << p << " )" << "::parent_path :" << p.parent_path() << std::endl;
    }
    gnl::path p("/home/gavin/file.txt");

    return 0;


}



