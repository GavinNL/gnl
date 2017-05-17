/*
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


// In progress, no testing done yet
// Do not use yet.

#ifndef GNL_VARIANT_H
#define GNL_VARIANT_H


#include <type_traits>
#include <iostream>

namespace gnl
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
class Variant;



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
class Variant
{
    public:
        static const std::size_t size       = sizeof...(_T);
        static const std::size_t bytes_size = type_size<_T...>::max;

        using Variant_Type = Variant<_T...>;

        Variant() {}

        Variant_Type& operator=(const  Variant_Type & V)
        {
            if( this != &V)
            {
                current_type = V.current_type;
                cpydata(V.data, data, bytes_size);
            }
            return *this;
        }

        Variant(const Variant_Type & V)
        {
            current_type = V.current_type;
            cpydata(V.data, data, bytes_size);
        }

        Variant(Variant_Type && V)
        {
            current_type = V.current_type;
            cpydata(V.data, data, bytes_size);
            V.current_type = 255;
        }

        template<typename T>
        Variant(const T & V)
        {
            std::cout << "VARIANT copy constructor" << std::endl;
            static_assert( type_in< typename std::remove_const<T>::type, _T...>::value, "That type is not represented in the variant!" );
            new (data)T(V);
            current_type = index_in_list<T,_T...>::value;
        }

        //template<typename T>
        //Variant( T & V) : Variant( static_cast<const T>(V) )
        //{
        //}

        template<typename T>
        Variant( T && V)
        {
            std::cout << "VARIANT move constructor" << std::endl;
            //static_assert( type_in<T,_T...>::value, "That type is not represented in the variant!" );

            static_assert( type_in< typename std::remove_const<T>::type, _T...>::value, "That type is not represented in the variant!" );

            new (data)T( std::move(V) );
            current_type = index_in_list<T,_T...>::value;
        }



        Variant_Type & operator=( Variant_Type && V)
        {
            std::cout << "VARIANT Move operator" << std::endl;
            current_type = V.current_type;
            cpydata(V.data, data, bytes_size);
            V.current_type = 255;

            return *this;
        }


       template<typename T>
       typename std::enable_if <  type_in<T, _T...>::value  , Variant_Type  >::type & operator=( T &&V)
       {
          // std::cout << "Move operator" << std::endl;
           std::cout << "VARIANT  Moving type" << std::endl;

           static_assert( detail::type_in<T,_T...>::value, "That type is not represented in the variant!" );

           if( is<T>() )
           {
               as<T>() = std::move( V );
           }
           else
           {
               Destroy();
               new (data)T( std::move(V) );
               current_type = index_in_list<T,_T...>::value;
           }

           return *this;
       }



        template<typename T>
        Variant& operator=(const T & V)
        {
         //   std::cout << "Assignment operator" << std::endl;
            ConstructType<T>(V);
            return *this;
        }



        ~Variant()
        {
          //  std::cout << "destroying variant" << std::endl;
            Destroy();
        }

        void Destroy()
        {
            if(current_type == 0xFF ) return;
            static data_destroyer< sizeof...(_T)-1, _T...> destroy;
            destroy(current_type, data);
        }


        template<typename T>
        void MoveType( T && V)
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
                Destroy();
                new (data)T( std::move(V) );
                current_type = index_in_list<T,_T...>::value;
            }

        }

        template<typename T>
        void ConstructType(const T & V)
        {
            static_assert( type_in< typename std::remove_const<T>::type, _T...>::value, "That type is not represented in the variant!" );

           // std::cout << "ConstructType: "<< V << std::endl;

            if( is<T>() )
            {
                as<T>() = V;
            }
            else
            {
                Destroy();
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

        friend std::ostream & operator<< (std::ostream& stream, const Variant_Type & V)
        {
            static print_as<sizeof...(_T)-1, _T...> P;

            stream << "(" << (int)V.current_type << ")" ;

            P(V.current_type, stream, (void*)V.data);
            return stream;
        }

        void cpydata(void const * source,  void * dest, int length )
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
using Variant = gnl::detail::Variant<T...>;

}


#endif // GNL_VARIANT_H


