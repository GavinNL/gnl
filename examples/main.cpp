#include <iostream>
#include "gnl_filesystem.h"



//#define GNL_JSON_IMPLEMENT
//#include "gnl_json.h"


using namespace std;


int main2();  // see main2.cpp for definition.

int main()
{
    std::string raw =
R"del(
    {
        number : -2.66e12,
        string : "hello",
        array  : [3,1,3,5],
        object : {
                    num : 32
                 }
    }
)del";


    gnl::json::Value O(raw);

    std::cout << " String: " << O["string"].as<string>() << std::endl;
    std::cout << " Number: " << O["number"].as<float>()  << std::endl;
    std::cout << " Array: [" << O["array"][0].as<float>() << ", "
                             << O["array"][1].as<float>() << ", "
                             << O["array"][2].as<float>() << ", "
                             << O["array"][3].as<float>() << "] "<< std::endl;


    //  Adding new values:

    O["NewObject"];  // accessing a value that does not exist, will automatically create it and
                     // set it to an UNKNOWN value.

    O["NewObject"]["name"]        = "Gavin";         // attemptign to access it like an OBJECT, will force it to become an object.
    O["NewObject"]["age"]         = 30;
    O["NewObject"]["parents"][0]  = "Glenn";
    O["NewObject"]["parents"][1]  = "May";


    std::cout << "Checking newly created objects\n";
    std::cout << "NewObject->Name: " << O["NewObject"]["name"].as<std::string>() << std::endl;
    std::cout << "NewObject->age : " << O["NewObject"]["age" ].as<float>()       << std::endl;
    std::cout << "NewObject->parents: [" << O["NewObject"]["parents"][0].as<std::string>() << "," << O["NewObject"]["parents"][1].as<std::string>() << "]" << std::endl;


    // Here we create a new array.
    O["NewArray"][0] = 1;
    O["NewArray"][1] = 2;

    std::cout << "NewArray: [" << O["NewArray"][0].as<float>() << "," << O["NewArray"][1].as<float>() << "]" << std::endl;


    // but we will now turn it into a number
    O["NewArray"] = 32;
    std::cout << "NewArray: " << O["NewArray"].as<float>() << std::endl;

    try {
        std::cout << "Attempting to cast a float to a string: " << O["NewArray"].as<std::string>() << std::endl;
    } catch (gnl::json::incorrect_type & e) {
        std::cout << "Exception: Cannot cast a float value to a string." << std::endl;
    }


    // create an array of objects:
    O["Students"][0]["name"] ="bart";
    O["Students"][0]["age"]  = 10;
    O["Students"][0]["grade"]  = 4;

    O["Students"][1]["name"] ="lisa";
    O["Students"][1]["age"]  = 8;
    O["Students"][1]["grade"]  = 2;

    O["Students"][2]["name"] ="milhouse";
    O["Students"][2]["age"]  = 10;
    O["Students"][2]["grade"]  = 4;

    // Loop through values in the aray.
    std::cout << "Students size:" << O["Students"].size() << std::endl;
    for(auto a : O["Students"].getValueVector() )
    {
        gnl::json::Value & S = *a;
        std::cout << "name: " << S["name"].as<std::string>()  << std::endl;
        std::cout << "grade:" << S["grade"].as<float>()  << std::endl;
        std::cout << "age  :" << S["age"  ].as<float>()  << std::endl;
    }

    std::cout << O.type() << std::endl;
    for(auto a : O.getValueMap() )
    {
         gnl::json::Value & S = *a.second;
        std::cout << "Object: "  << a.first << std::endl;
        std::cout <<    "size: " << S.size()  << std::endl;
    }

    //==========================================
    // Call main2() which uses
    //==========================================
    main2();
    //==========================================

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


    //
    //
}

#define GNL_IMPLEMENTATION
#include "gnl_filesystem.h"
#undef GNL_IMPLEMENT
