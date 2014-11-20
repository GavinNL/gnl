#include <iostream>
#include "gnl_filesystem.h"

#define GNL_JSON_IMPLEMENT
#include "gnl_json.h"
using namespace std;

int main()
{


    std::string raw =

            R"delimiter(
{
     students : {
                    "gavin" : {
                        age : 30
                        family: ["glenn", 4534, "calvin"],
                        single: true
                    },
                 }
}

)delimiter";


    std::istringstream S(raw);
    gnl::json::Object O;
    O.parse(S);


    std::cout << "first value: " << O["students"]["gavin"]["family"][0].as<std::string>() << std::endl;
    std::cout << "first value: " << O["students"]["gavin"]["family"][1].as<float>() << std::endl;
    std::cout << "first value: " << O["students"]["gavin"]["family"][2].as<std::string>() << std::endl;
    std::cout << "first value: " << O["students"]["gavin"]["single"].as<bool>() << std::endl;


    //std::cout << "(" << k << ")  next= (" << (char)S.get() << ")" << std::endl;
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
