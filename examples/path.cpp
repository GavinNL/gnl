#include <iostream>
#include <gnl/gnl_path.h>
#include <assert.h>


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


        /*
        P.Report();
        std::cout << "FileName : " << P.FileName()          << std::endl;
        std::cout << "BaseName : " << P.FileBaseName()      << std::endl;
        std::cout << "Extension: " << P.FileExtension()      << std::endl;
        std::cout << "Extension: " << P.FileExtension()     << std::endl;
        std::cout << "Parent   : " << P.Parent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;

    {
        gnl::Path P("/home/commander/test");
        std::cout << "FileName : " << P.FileName()          << std::endl;
        //std::cout << "BaseName : " << P.FileBasename()      << std::endl;
        //std::cout << "Extension: " << P.FileExtension()     << std::endl;
        std::cout << "Parent   : " << P.Parent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
    if(0){
        gnl::Path P("home/commander/test.png");
        std::cout << "FileName : " << P.FileName()          << std::endl;
        std::cout << "BaseName : " << P.FileBasename()      << std::endl;
        std::cout << "Extension: " << P.FileExtension()     << std::endl;
        std::cout << "Parent   : " << P.Parent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
*/
    return 0;


}

