#include <iostream>
#include "gnl_filesystem.h"

#define GNL_JSON_IMPLEMENT
#include "gnl_json.h"

using namespace std;

int main()
{
    std::string raw =
R"del(
    {
        number : -2.66e12,
        string : "hello\nnewline",
        array : [3,1,3,5],
        object : {
                    num : 32
                 }
    }
)del";



    gnl::json::Value O(raw);

    std::cout << " NUmber: " << O["string"].as<string>() << std::endl;
    std::cout << " NUmber: " << O["number"].as<float>() << std::endl;


    return 0;
    {
        gnl::Path P("c:\\home\\commander\\test.png");
        std::cout << "FileName : " << P.getFileName()          << std::endl;
        std::cout << "BaseName : " << P.getFileBasename()      << std::endl;
        std::cout << "Extension: " << P.getFileExtension()     << std::endl;
        std::cout << "Parent   : " << P.getParent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
    {
        gnl::Path P("/home/commander/test.png");
        std::cout << "FileName : " << P.getFileName()          << std::endl;
        std::cout << "BaseName : " << P.getFileBasename()      << std::endl;
        std::cout << "Extension: " << P.getFileExtension()     << std::endl;
        std::cout << "Parent   : " << P.getParent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
    {
        gnl::Path P("home/commander/test.png");
        std::cout << "FileName : " << P.getFileName()          << std::endl;
        std::cout << "BaseName : " << P.getFileBasename()      << std::endl;
        std::cout << "Extension: " << P.getFileExtension()     << std::endl;
        std::cout << "Parent   : " << P.getParent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
    return 0;
}

#define GNL_IMPLEMENTATION
#include "gnl_filesystem.h"
#undef GNL_IMPLEMENT
