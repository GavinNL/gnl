#ifndef GNL_DATA_STORE_H
#define GNL_DATA_STORE_H

#if __cplusplus < 201703L
error "Requires a C++17 compliant compiler"
# else


#include <vector>
#include <array>
#include <any>
#include <limits>
#include <map>
#include <functional>
#include <tuple>
#include <type_traits>
#include <iostream>
#include <memory>

#ifndef GNL_NAMESPACE
#define GNL_NAMESPACE gnl
#endif

namespace GNL_NAMESPACE
{


/**
 * @brief The data_store class
 *
 * A simple class to store arbitary data. This is basically a
 * smalle helper around a map of std::any.
 *
 * Nothing special here.
 */
class data_store
{
public:
    std::any & operator [] (std::string const & name)
    {
        return m_data[name];
    }

    template<typename T>
    T & get(std::string const & name)
    {
        return std::any_cast<T&>( m_data.at(name) );
    }

    template<typename T>
    T const & get(std::string const & name) const
    {
        return std::any_cast<T const&>( m_data.at(name) );
    }

    template<typename T>
    bool has(std::string const & name) const
    {
        auto f = m_data.find(name);
        if( f == std::end(m_data))
        {
            return false;
        }
        else
        {
             return f->second.type() == typeid (T);
        }
    }
public:
    std::map< std::string, std::any> m_data;
};

}

#endif

#endif

