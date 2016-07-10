#include <iostream>
#include <gnl/gnl_path.h>
#include <assert.h>


using namespace std;


int main()
{


        gnl::Path P("c:\\home\\commander\\test.png");
        assert(P.FileName()      == "test.png");
        assert(P.FileBaseName()  == "test");
        assert(P.FileExtension() == "png");


        gnl::Path A("/home/gavin/Code/C++/");
        std::cout << (A+gnl::Path("../../hello.txt"))  << std::endl;


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

