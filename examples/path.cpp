#include <iostream>
#include <gnl/gnl_path.h>
#include <assert.h>
#include <thread>

#include <gnl/gnl_interp.h>
#include <gnl/gnl_animate.h>

using namespace std;


int main()
{

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

