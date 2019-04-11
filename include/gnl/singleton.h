#ifndef GNL_SINGLETON_H
#define GNL_SINGLETON_H

#include <memory>

namespace gnl
{

template<typename T, size_t ID=0>
class singleton
{
public:
    static T & get()
    {
        static auto t = std::make_shared<T>();
        return *t;
    }

    static size_t id()
    {
        return ID;
    }
};

}


#endif

