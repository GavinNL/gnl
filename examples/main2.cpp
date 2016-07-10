/*
 * This file is to show how you can combile the gnl libraries into two namespaces.
 *
 * This is useful, for example, you are building software and you want to use the gnl::json code. But you are
 * using an external library (eg: ExLib), which uses an older version of gnl::json. Trying to compile your software
 * will give problems because you can't have two version of the same source code because you will get a multiple
 * definitions error.
 *
 * Since the gnl sources are small, you can compile them separately into a another namespace by doing the following;
 *
 */

#define GNL_JSON_IMPLEMENT
#define GNL_NAMESPACE MyNewNamespace
#include "gnl_json.h"


int main2()
{
    std::string raw =
R"del(
    {
        string : "hello"
    }
)del";

    MyNewNamespace::json::Value O(raw);

    std::cout << "Output from MyNewNamespace::json: " << O["string"].as<std::string>() << std::endl;
    return 0;
}

