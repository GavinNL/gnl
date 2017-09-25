#ifndef GNL_JSON_H
#define GNL_JSON_H

#include <vector>
#include <map>
#include <cassert>
#include <type_traits>
#include <iostream>
#include <cstring>

namespace gnl
{

class json
{
public:
    using number  = double;
    using boolean = bool;
    using string  = std::string;
    using array   = std::vector<json>;
    using object  = std::map<string, json>;

    enum type_t
    {
        null,
        BOOLEAN,
        NUMBER,
        STRING,
        OBJECT,
        ARRAY
    };

    #define MAX_(a,b) ( (a)>(b)?(a):(b) )
    #define SIZE_ MAX_( sizeof(string), MAX_( sizeof(array), sizeof(object) ))

    type_t        m_type;
    unsigned char m_data[SIZE_];
#undef SIZE_
#undef MAX_



#define CONDITIONAL_TYPE(c, T1, T2) typename std::conditional< c, T1, T2 >::type

#define STRICT_TYPES(T) \
    (std::is_same<T,object>::value  | std::is_same<T,array>::value   | \
     std::is_same<T,string>::value  | std::is_same<T,number>::value  | \
     std::is_same<T,boolean>::value )

#define NUMERIC(T)     ( !std::is_same<bool,T>::value && (std::is_integral<T>::value | std::is_floating_point<T>::value) )
#define STRING_CONV(T) ( std::is_convertible< T, std::string>::value )

#define EXTENDED_TYPES(T) \
    (std::is_same<T,object>::value  | std::is_same<T,array>::value   | \
     std::is_same<T,string>::value  | std::is_same<T,number>::value  | \
     std::is_integral<T>::value  | std::is_floating_point<T>::value  | \
     std::is_same<T,boolean>::value  )

#define CHECK_TYPES(T) static_assert( STRICT_TYPES(T), "incorrect type");

    json() : m_type(null)
    {
    }

    explicit json(json const & other) : m_type(null)
    {
        construct(other.type());
        switch(type())
        {
            case BOOLEAN: __as<boolean>() = other.__as<boolean> (); return;
            case NUMBER:  __as<number >() = other.__as<number > (); return;
            case ARRAY:   __as<array  >() = other.__as<array  > (); return;
            case OBJECT:  __as<object >() = other.__as<object > (); return;
            case STRING:  __as<string >() = other.__as<string > (); return;
            case null:
            break;
            default:
                throw std::runtime_error("Bad cast");
        }
    }

    template<typename T>
    explicit json(T const & other) : m_type(null)
    {
        static_assert( STRICT_TYPES(T) | NUMERIC(T) | STRING_CONV(T), "incorrect types");

        if( std::is_same<T,object>::value  ) { construct<object>(); *reinterpret_cast<object *>(m_data) = *reinterpret_cast<object const*>(&other);  return;}
        if( std::is_same<T,array>::value   ) { construct<array>();  *reinterpret_cast<array  *>(m_data) = *reinterpret_cast<array const*>(&other);   return;}
        if( STRING_CONV(T)  ) { construct<string>(); __set<string,T>(other);  return;}
        if( std::is_same<T,boolean>::value ) { construct<boolean>();*reinterpret_cast<boolean*>(m_data) = *reinterpret_cast<boolean const*>(&other); return;}
        if( NUMERIC(T)  )                    { construct<number>(); __set<number,T>(other); return ;}

    }


    json(json && other) : m_type(null)
    {
        static_assert(sizeof(m_data) >= sizeof(string), "testing" );
        memcpy(m_data, other.m_data, sizeof(m_data) );
        memset(other.m_data, 0, sizeof(m_data));
        m_type = other.m_type;
        other.m_type = null;
    }

    ~json()
    {
        destroy();
    }

    json & operator=(json const & other)
    {
        if( this != &other)
        {
            if( type() != other.type() )
                construct(other.type());

            switch(type())
            {
                case BOOLEAN: __as<boolean>() = other.__as<boolean> (); return *this;
                case NUMBER:  __as<number >() = other.__as<number > (); return *this;
                case ARRAY:   __as<array  >() = other.__as<array  > (); return *this;
                case OBJECT:  __as<object >() = other.__as<object > (); return *this;
                case STRING:  __as<string >() = other.__as<string > (); return *this;
                case null:
                    break;
                default:
                    throw std::runtime_error("Bad cast on operator=");
            }
        }
        return *this;
    }

    json & operator=(json && other)
    {
        if( this != &other)
        {
            static_assert(sizeof(m_data) >= sizeof(string), "testing" );
            memcpy(m_data, other.m_data, sizeof(m_data) );
            memset(other.m_data, 0, sizeof(m_data));
            m_type = other.m_type;
            other.m_type = null;
        }
        return *this;
    }


    template<typename T>
    typename std::enable_if< STRICT_TYPES(T) , json>::type
    & operator=(T const & other)
    {
        static_assert( STRICT_TYPES(T) | NUMERIC(T) , "incorrect types");

        construct<T>() = other;
        return *this;
    }

    template<typename T>
    typename std::enable_if< !STRICT_TYPES(T) && NUMERIC(T) , json>::type
    & operator=(T const & other)
    {
        static_assert( NUMERIC(T) , "incorrect types");

        construct<number>() = static_cast<number>(other);
        return *this;
    }

    template<typename T>
    typename std::enable_if< !STRICT_TYPES(T) && STRING_CONV(T) , json>::type
    & operator=(T const & other)
    {
        static_assert( STRING_CONV(T) , "incorrect types");

        construct<string>() = other;
        return *this;
    }

    //=======================================================================
    template<typename T>
    typename std::enable_if< STRICT_TYPES(T) , json>::type
    & operator=(T && other)
    {
        static_assert( STRICT_TYPES(T) | NUMERIC(T) , "incorrect types");

        construct<T>() = std::move(other);
        return *this;
    }

    //=======================================================================

    /**
     * @brief type
     * @return
     *
     * Returns the type stored in the json class
     */
    type_t type() const
    {
        return m_type;
    }

    /**
     * @brief as
     * @return
     *
     * Gets a reference to the object. the template paramter must be
     * the one of the string json types (json::number, json::boolean, json::string, json::array, json::object)
     *
     * If the underlying object is not the type specified in the templater parameter. It will throw an exception.
     */
    template<typename T>
    T & as()
    {
        static_assert( STRICT_TYPES(T) , "incorrect types");

        if( type() != type_from_template<T>() )
            throw std::runtime_error("Json is not the correct type");

        switch( type() )
        {
            case BOOLEAN:
            case NUMBER:
            case ARRAY:
            case OBJECT:
            case STRING:  return *reinterpret_cast<T*>(m_data);
            default:
                throw std::runtime_error("Bad cast");
        }
    }

    template<typename T>
    T const & as() const
    {
        static_assert( STRICT_TYPES(T) , "incorrect types");

        if( type() != type_from_template<T>() )
            throw std::runtime_error("Json is not the correct type");

        switch( type() )
        {
            case BOOLEAN:
            case NUMBER:
            case ARRAY:
            case OBJECT:
            case STRING:  return *reinterpret_cast<T const*>(m_data);
            default:
                throw std::runtime_error("Bad cast");
        }
    }

    template<typename T>
    T get() const
    {
        if( convertable_to<T>() )
        {
            using proper_json_type = CONDITIONAL_TYPE(  NUMERIC(T), number, CONDITIONAL_TYPE(  STRING_CONV(T), string, T) );

            return static_cast<T>( __as<proper_json_type>() );
        }

        return T();
    }


    /**
     * @brief at
     * @param i
     * @return
     *
     * Accesses the i'th element in the json array. Throws a runtime error
     * if the json is not an array or if the array is out of bounds
     */
    json & at(size_t i)
    {
        switch(m_type)
        {
            case ARRAY:
                return __as<array>().at(i);
            default:
                throw std::runtime_error("Not an array");
        }
    }
    json const & at(size_t i) const
    {
        switch(m_type)
        {
            case ARRAY:
                return __as<array>().at(i);
            default:
                throw std::runtime_error("Not an array");
        }
    }
    /**
     * @brief operator []
     * @param i
     * @return
     *
     * Access the i'th element in the json array. If the json is
     * not an array, it will convert it into an array.
     * If the index is out of bounds, it will resize appropriately.
     *
     */
    json & operator[] (size_t i)
    {
        if(m_type!=ARRAY)
            construct<array>();

        if( i >= __as<array>().size() )
            __as<array>().resize(i+1);

        return __as<array>()[i];
    }
    json const & operator[] (size_t i) const
    {
        if(m_type!=ARRAY)
            throw std::runtime_error("Not an array");

        return __as<array>()[i];
    }

    /**
     * @brief get
     * @param i
     * @param default_value
     * @return
     *
     * Gets the i'th element in the array, if the item does not exist, or is not of type T, it will
     * return default_value
     */

    template<typename T>
    T get( size_t i, T const & default_value) const
    {
        if(m_type != ARRAY)
        {
            return default_value;
        }

        if( i < __as<array>().size() )
        {
            if( type_from_template<T>() == __as<array>()[i].type() )
            {
                using proper_json_type = CONDITIONAL_TYPE(  NUMERIC(T), number, CONDITIONAL_TYPE(  STRING_CONV(T), string, T) );

                return static_cast<T>( __as<array>()[i].__as<proper_json_type>() );//  *reinterpret_cast<proper_json_type const*>( __as<array>()[i].m_data) );
            }
        }


        return default_value;
    }



    /**
     * @brief at
     * @param i
     * @return
     *
     * Returns the json element identified by the key, i. If the
     * json is not an OBJECT or the key does not exist. It will
     * throw an exception.
     */
    json & at(string const & i)
    {
        switch(m_type)
        {
            case OBJECT:
                return __as<object>().at(i);
            default:
                throw std::runtime_error("Not an object");
        }
    }
    json const & at(string const & i) const
    {
        switch(m_type)
        {
            case OBJECT:
                return __as<object>().at(i);
            default:
                throw std::runtime_error("Not an OBJECT");
        }
    }


    /**
     * @brief operator []
     * @param i
     * @return
     *
     * Access the json object's key identified by, i. If it is not
     * a json object, it will be converted into one.
     */
    json & operator[](string const & i)
    {
        if(m_type!=OBJECT)
            construct<object>();
        return __as<object>()[i];
    }
    json const & operator[] (string const & i) const
    {
        if(m_type!=OBJECT)
            throw std::runtime_error("Not an OBJECT");

        return __as<object>().at(i);
    }

    template<typename T>
    T get( string const & key, T const & default_value) const
    {
        if(m_type != OBJECT)
        {
            return default_value;
        }
        auto f = __as<object>().find(key);

        if( f == __as<object>().end() )
            return default_value;

        if( type_from_template<T>() == f->second.type())
        {
            using proper_json_type = CONDITIONAL_TYPE(  NUMERIC(T), number, CONDITIONAL_TYPE(  STRING_CONV(T), string, T) );

            return static_cast<T>( f->second.__as<proper_json_type>() );//*reinterpret_cast<proper_json_type const*>(f->second.m_data));
        }

        return default_value;
    }

    size_t size() const
    {
        switch( m_type)
        {
            case OBJECT:
                return __as<object>().size();
            case ARRAY:
                return __as<array>().size();
            case STRING:
                return __as<string>().size();
            default:
                return 0;
        }
    }
    bool is_object() const { return type() == OBJECT; }
    bool is_string() const { return type() == STRING; }
    bool is_number() const { return type() == NUMBER; }
    bool is_array()  const { return type() == ARRAY;  }
    bool is_null()   const { return type() == null;   }
    bool is_boolean()  const { return type() == BOOLEAN;  }

    template<typename T>
    bool operator==(T const & other) const
    {
        using proper_json_type = CONDITIONAL_TYPE(  NUMERIC(T), number, CONDITIONAL_TYPE(  STRING_CONV(T), string, T) );

        if( type() == type_from_template<proper_json_type>() )
        {
            return __as<proper_json_type>() == other;
        }

        throw std::runtime_error("JSON item is not the proper type.");
        return false;
    }

    template<typename T>
    bool operator!=(T const & other) const
    {
        using proper_json_type = CONDITIONAL_TYPE(  NUMERIC(T), number, CONDITIONAL_TYPE(  STRING_CONV(T), string, T) );

        if( type() == type_from_template<proper_json_type>() )
        {
            return __as<proper_json_type>() != other;
        }
        throw std::runtime_error("JSON item is not the proper type.");
        return false;
    }

#define CMP_OPERATOR(op) \
    template<typename T> \
    bool operator op (T const & other) const \
    { \
        using proper_json_type = CONDITIONAL_TYPE(  NUMERIC(T), number, CONDITIONAL_TYPE(  STRING_CONV(T), string, T) ); \
        if( type() == type_from_template<proper_json_type>() ) \
        { \
            return __as<proper_json_type>() op other; \
        } \
        throw std::runtime_error("JSON item is not the proper type.");\
    }

    CMP_OPERATOR(<)
    CMP_OPERATOR(>)
    CMP_OPERATOR(<=)
    CMP_OPERATOR(>=)


    template<typename T>
    operator T() const
    {
        static_assert( EXTENDED_TYPES(T), "Cannot convert");

        using proper_json_type = CONDITIONAL_TYPE(  NUMERIC(T), number, CONDITIONAL_TYPE(  STRING_CONV(T), string, T) );

        return static_cast<T>( __as<proper_json_type>() );
    }

private:
    template<typename T>
    type_t type_from_template() const
    {
            if( std::is_same<bool,T>::value   ) return BOOLEAN;
            if( std::is_same<array,T>::value  )  return ARRAY;
            if( std::is_same<object,T>::value ) return OBJECT;
            if( std::is_same<string,T>::value ) return STRING;
            if( NUMERIC(T) ) return NUMBER;
            return null;
    }

    template<typename T>
    bool convertable_to() const
    {
        switch(m_type)
        {
            case NUMBER:
                return std::is_convertible<T, number>::value;
            default:
                return type() == type_from_template<T>();
        }
    }

    template<typename T, typename T2>
    typename std::enable_if< std::is_same<T,number>::value && NUMERIC(T2) , void>::type
    __set(T2 const & other)
    {
        *reinterpret_cast<number*>(m_data) = other;
    }
    template<typename T, typename T2>
    typename std::enable_if< std::is_same<T,number>::value && !NUMERIC(T2) , void>::type
    __set(T2 const & other)
    {
      //  *reinterpret_cast<number*>(m_data) = other;
    }


    template<typename T, typename T2>
    typename std::enable_if< std::is_same<T,string>::value && STRING_CONV(T2), void>::type
    __set(T2 const & other)
    {
        *reinterpret_cast<string*>(m_data) = other;
    }
    template<typename T, typename T2>
    typename std::enable_if< std::is_same<T,string>::value && !STRING_CONV(T2), void>::type
    __set(T2 const & other)
    {
       // *reinterpret_cast<string*>(m_data) = other;
    }

    template<typename T>
    T & __as()
    {
        return *reinterpret_cast<T*>(m_data);
    }

    template<typename T>
    T const & __as() const
    {
        return *reinterpret_cast<T const*>(m_data);
    }
    void construct(type_t t)
    {
        // Make sure to destroy if the current type is different
        if( t != m_type)
        {
            destroy();
        }

        switch(t)
        {
            case ARRAY:   construct<array> (); break;
            case OBJECT:  construct<object> (); break;
            case STRING:  construct<string> (); break;
            default:
                break;
        }
    }

    template<typename T>
    T & construct()
    {
        //CHECK_TYPES(T);
        static_assert( STRICT_TYPES(T), "incorrect types");
        //assert( m_type == null);
        destroy();

        auto * p = new(m_data) T();

        if( std::is_same<T,array>::value  ) { m_type = ARRAY;  return *p;}
        if( std::is_same<T,object>::value ) { m_type = OBJECT; return *p;}
        if( std::is_same<T,string>::value ) { m_type = STRING; return *p;}
        if( std::is_same<T,boolean>::value ) { m_type = BOOLEAN; return *p;}
        if( NUMERIC(T) ) { m_type = NUMBER; return *p;}

        return *p;
    }

    void destroy()
    {
        switch(m_type)
        {
            case BOOLEAN: destroy<boolean> (); break;
            case NUMBER:  destroy<number>(); break;
            case ARRAY:   destroy<array> (); break;
            case OBJECT:  destroy<object>(); break;
            case STRING:  destroy<string>(); break;
            default:
                break;
        }
        m_type = null;
    }

    template<typename T>
    void destroy()
    {
        CHECK_TYPES(T);

        assert( m_type != null);

        reinterpret_cast<T*>(m_data)->~T();

        m_type = null;
    }

public:
    static void print( json const & J)
    {
        switch( J.type() )
        {
            case BOOLEAN: std::cout << J.__as<boolean>(); break;
            case NUMBER:  std::cout << J.__as<number>();  break;
            case STRING:  std::cout << J.__as<string>().c_str();  break;
            case ARRAY:   break;//std::cout << destroy<array> (); break;
            case OBJECT:  break;//std::cout << destroy<object>(); break;
            default: break;
        }
    }
#if 0
    static json parse_json(std::istringstream & S)
    {

    }

    static json parse_array(std::istringstream & S)
    {

    }

    static json parse_object(std::istringstream & S)
    {

    }

    static json parse_number(std::istringstream & S)
    {

    }

    static json parse_bool(std::istringstream & S)
    {

    }

    static json parse_string(std::istringstream & S)
    {
        std::string str;
        str.reserve(20);


        char c = ' ';

        while( std::isspace(c) ) { c = S.get() };

        if( c == '"') // first quote
        {
            c = S.get();
            while( c != '"') // loop until we get the last quote
            {
                if( c == '\\')
                {
                    c = S.get();
                    switch(c)
                    {
                        case 'n':
                            c = '\n'; break;
                        case 't':
                            c = '\t'; break;
                        case 'f':
                            c = '\f'; break;
                        case '\\':
                            c = '\\'; break;
                        case '/':
                            c = '/'; break;
                        case 'r':
                            c = '\r'; break;
                        default:
                            break;
                    }
                }
                str += c;
                c = S.get();
            }

        }
    }
#endif
};
}

std::ostream & operator<<(std::ostream & out, const gnl::json & J)
{
    return out;
}

#endif

