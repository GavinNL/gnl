/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */



#ifndef GNL_VARIANT_H
#define GNL_VARIANT_H


#include <type_traits>
#include <iostream>

#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif
namespace GNL_NAMESPACE
{

namespace detail
{

//===============================================================================
// META-FUNCTIONS FOR EXTRACTING THE n-th TYPE OF A PARAMETER PACK

// Declare primary template
template<int I, typename... Ts>
struct nth_type_of
{};
// Base step
template<typename T, typename... Ts>
struct nth_type_of<0, T, Ts...>
{using type = T;};

// Induction step
template<int I, typename T, typename... Ts>
struct nth_type_of<I, T, Ts...>
{ using type = typename nth_type_of<I - 1, Ts...>::type;};

// Helper meta-function for retrieving the first type in a parameter pack
template<typename... Ts>
struct first_type_of
{using type = typename nth_type_of<0, Ts...>::type;};

// Helper meta-function for retrieving the last type in a parameter pack
template<typename... Ts>
struct last_type_of
{using type = typename nth_type_of<sizeof...(Ts) - 1, Ts...>::type;};


//========================================

// Meta functions for determining whether a type exists in a paramter pack
//
//  eg: type_in<float,    int, doulbe, char,float>::value  == true (since float exists in the pack)

template<typename T, typename ...Tail>
struct type_in
{};

template<typename T, typename Head>
struct type_in<T,Head>
{static const bool value = std::is_same<T,Head>::value;};

template<typename T, typename Head, typename ...Tail>
struct type_in<T,Head,Tail...>
{static const bool value = std::is_same<T,Head>::value || type_in<T, Tail...>::value;};

//===========================================
template<int I, typename... Ts>
struct pth_type_of
{};
// Base step
template<typename T, typename... Ts>
struct pth_type_of<0, T, Ts...>
{
    static const int value = std::is_same < T, typename first_type_of<Ts...>::type >::value ? 0 : -1;
};

// Induction step
template<int I, typename T, typename... Ts>
struct pth_type_of<I, T, Ts...>
{static const int value = std::is_same < T, typename nth_type_of<I, Ts...>::type >::value ? I : pth_type_of<I-1,T, Ts...>::value;};

// Meta function for determining the location (index) of a type within a paramter pack.
//                                 0     1       2
//  eg:  index_in_list<float,     int, float, double>::value == 1 since (float is type 1)
template<typename T, typename... Ts>
struct index_in_list
{static const int value = pth_type_of< sizeof...(Ts)-1, T, Ts...>::value;};

//================================

// meta function for determining the max and min size type in
// a parameter pack.
template<typename T, typename ...Tail>
struct type_size{};

template<typename T, typename Head>
struct type_size<T,Head>
{
    static const int max   = sizeof(T)>sizeof(Head) ? sizeof(T) : sizeof(Head);
    static const int min   = sizeof(T)<sizeof(Head) ? sizeof(T) : sizeof(Head);
};

template<typename T, typename Head, typename ...Tail>
struct type_size<T,Head,Tail...>
{
    static const int max = sizeof(T) > type_size<Head,Tail...>::max ? sizeof(T) : type_size<Head,Tail...>::max;
    static const int min = sizeof(T) < type_size<Head,Tail...>::min ? sizeof(T) : type_size<Head,Tail...>::min;

};


template<int I, typename... Ts>
struct data_destroyer;

// Induction step
template<int I, typename... Ts>
struct data_destroyer
{
    // static const int value = std::is_same < T, typename nth_type_of<I, Ts...>::type >::value ? I : pth_type_of<I-1,T, Ts...>::value;

    void operator()(int index, void * data)
    {
       // std::cout << "Current Index: " << index << " ?= " << I <<std::endl;
        if( index == I)
        {
            using T = typename nth_type_of< I, Ts... >::type;

            static_cast< T* > (data)->~T();
         //   std::cout << "Index " << I << " destroyed" << std::endl;
        }
        else
        {
            data_destroyer<I-1,Ts...> D;
            D( index ,data );
        }
    }
};

// Base step
template< typename... Ts >
struct data_destroyer<0, Ts...>
{
    void operator()(int index, void * data)
    {
        if( index == 0 )
        {
            using T = typename nth_type_of<0, Ts...>::type;

            static_cast< T*>(data)->~T( );
        }

    }
};



template<typename... Ts>
class variant;



// Base step
template<int I, typename... Ts>
struct print_as;

// Induction step
template<int I, typename... Ts>
struct print_as
{
    // static const int value = std::is_same < T, typename nth_type_of<I, Ts...>::type >::value ? I : pth_type_of<I-1,T, Ts...>::value;

    void operator()(int index , std::ostream& stream , void * data) const
    {
       // std::cout << "Current Index: " << index << " ?= " << I <<std::endl;
        if( index == I )
        {
            using T = typename nth_type_of< I, Ts... >::type;

            stream << *static_cast<T*>(data);
        }
        else
        {
            print_as<I-1,Ts...> D;
            D( index , stream , data );
        }
    }
};

template< typename... Ts >
struct print_as<0, Ts...>
{
    void operator()(int index , std::ostream& stream , void * data) const
    {
        if( index == 0 )
        {
            using T = typename nth_type_of< 0, Ts... >::type;

            stream << *static_cast<T*>(data);
        }

    }
};




template<typename ..._T>
class variant
{
    public:
        static const std::size_t size       = sizeof...(_T);
        static const std::size_t bytes_size = type_size<_T...>::max;

        using variant_type = variant<_T...>;

        variant() {}

        variant_type& operator=(const  variant_type & V)
        {
            if( this != &V)
            {
                current_type = V.current_type;
                cpydata(V.data, data, bytes_size);
            }
            return *this;
        }

        variant(const variant_type & V)
        {
            current_type = V.current_type;
            cpydata(V.data, data, bytes_size);
        }

        variant(variant_type && V)
        {
            current_type = V.current_type;
            cpydata(V.data, data, bytes_size);
            V.current_type = 255;
        }

        template<typename T>
        variant(const T & V)
        {
           // std::cout << "VARIANT copy constructor" << std::endl;
            static_assert( type_in< typename std::remove_const<T>::type, _T...>::value, "That type is not represented in the variant!" );
            new (data)T(V);
            current_type = index_in_list<T,_T...>::value;
        }

        //template<typename T>
        //Variant( T & V) : Variant( static_cast<const T>(V) )
        //{
        //}

        template<typename T>
        variant( T && V)
        {
           // std::cout << "VARIANT move constructor" << std::endl;
            //static_assert( type_in<T,_T...>::value, "That type is not represented in the variant!" );

            static_assert( type_in< typename std::remove_const<T>::type, _T...>::value, "That type is not represented in the variant!" );

            new (data)T( std::move(V) );
            current_type = index_in_list<T,_T...>::value;
        }



        variant_type & operator=( variant_type && V)
        {
          //  std::cout << "VARIANT Move operator" << std::endl;
            current_type = V.current_type;
            cpydata(V.data, data, bytes_size);
            V.current_type = 255;

            return *this;
        }


       template<typename T>
       typename std::enable_if <  type_in<T, _T...>::value  , variant_type  >::type & operator=( T &&V)
       {
          // std::cout << "Move operator" << std::endl;
          // std::cout << "VARIANT  Moving type" << std::endl;

           static_assert( detail::type_in<T,_T...>::value, "That type is not represented in the variant!" );

           if( is<T>() )
           {
               as<T>() = std::move( V );
           }
           else
           {
               destroy();
               new (data)T( std::move(V) );
               current_type = index_in_list<T,_T...>::value;
           }

           return *this;
       }



        template<typename T>
        variant& operator=(const T & V)
        {
         //   std::cout << "Assignment operator" << std::endl;
            construct_type<T>(V);
            return *this;
        }



        ~variant()
        {
          //  std::cout << "destroying variant" << std::endl;
            destroy();
        }

        void destroy()
        {
            if(current_type == 0xFF ) return;
            static data_destroyer< sizeof...(_T)-1, _T...> destroy;
            destroy(current_type, data);
        }


        template<typename T>
        void move_type( T && V)
        {
            //std::cout << "Moving type" << std::endl;
            static_assert(
                             type_in<T,_T...>::value, "That type is not represented in the variant!" );

            if( is<T>() )
            {
                as<T>() = std::move(V);
            }
            else
            {
                destroy();
                new (data)T( std::move(V) );
                current_type = index_in_list<T,_T...>::value;
            }

        }

        template<typename T>
        void construct_type(const T & V)
        {
            static_assert( type_in< typename std::remove_const<T>::type, _T...>::value, "That type is not represented in the variant!" );

           // std::cout << "ConstructType: "<< V << std::endl;

            if( is<T>() )
            {
                as<T>() = V;
            }
            else
            {
                destroy();
                new (data)T(V);
                current_type = index_in_list<T,_T...>::value;
            }

        }

        int type() const
        {
            return current_type;
        }

        template<typename T>
        bool is() const
        {
            return current_type == index_in_list<T,_T...>::value;
        }

        template<typename T>
        const T & as() const
        {
            return *static_cast<T*>( (void*)&data[0]);
        }

        template<typename T>
        T & as()
        {
            return *static_cast<T*>( (void*)&data[0]);
        }

        friend std::ostream & operator<< (std::ostream& stream, const variant_type & V)
        {
            static print_as<sizeof...(_T)-1, _T...> P;

            stream << "(" << (int)V.current_type << ")" ;

            P(V.current_type, stream, (void*)V.data);
            return stream;
        }

        void cpydata(void const * source,  void * dest, size_t length )
        {
            unsigned char const * src = (unsigned char const *)source;
            unsigned char * dst = (unsigned char*)dest;
            while(length--) *dst++ = *src++;
        }



        unsigned char current_type = 0xFF;
        unsigned char data[bytes_size];
};




}

template<typename ...T>
using Variant = gnl::detail::variant<T...>;

}


#endif // GNL_VARIANT_H


