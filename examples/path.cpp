#include <iostream>
#include <gnl/gnl_path.h>
#include <assert.h>


using namespace std;


int main()
{
    gnl::Path P1("C:\\Users\\c11237\\HOME\\CodeTests\\");

    auto D = P1.GetFileList();

    for(auto & d : D)
    {
        std::cout << d << std::endl;
    }
    return 0;


    gnl::Path P("c:\\home\\comm  ander\\test.png");

    assert(P.FileName()      == "test.png");
    assert(P.FileBaseName()  == "test");
    assert(P.FileExtension() == "png");
    assert(P.Device() == "c");

    gnl::Path A("/home/gavin/Code/C++/");

    assert(  A.IsFolder() );
    assert(  A.IsFolder() );
    assert(  A.IsAbsolute() );

    A +=  gnl::Path("../../hello.txt");

    assert( A.ToString(gnl::Path::UNIX_STYLE) == "/home/gavin/hello.txt" );
    std::cout << P.ToString(gnl::Path::WINDOWS_STYLE) << std::endl;
    std::cout << P.ToString(gnl::Path::UNIX_STYLE) << std::endl;



    return 0;


}

