/*
gnl_json.h - v1.00 - Public domain JSON reader

   A single file dropin library for handling JSON style structures inspired by the STB library
   https://github.com/nothings/stb

   This library is in the public domain and can be used as one sees fit but cannot
   hold the author responsible for any damages that may occur as a result of using
   this code.


   To use, define GNL_JSON_IMPLEMENT before including the header file to compile the
   implementation. Include the header without defining GNL_JSON_IMPLEMENT to simple
   include the function and class definitions.

      #define GNL_JSON_IMPLEMENT
      #include "gnl_json.h"


    Example:  main.cpp

    gnl::json::Object myJSON;

    myJSON.parse( S );


 */

#ifndef __GNL_JSON__H__
#define __GNL_JSON__H__

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <exception>
#include <memory>

namespace gnl {

namespace json {

class Value;


typedef std::shared_ptr<Value> pValue;


class incorrect_type : public std::exception {
};

class parse_error : public std::exception {
};

class Value
{

    public:

        typedef enum {
            UNKNOWN,
            BOOL,
            NUMBER,
            STRING,
            ARRAY,
            OBJECT
        } TYPE;

        Value() : _type(UNKNOWN) {}
        Value(const Value  & rhs);
        Value(const std::string & rhs);

        ~Value();


        void init(const Value & rhs);
        // clears the Value and sets it's type to UNKNOWN
        void clear();

        // Interprets the Value as a specific type: bool, float, string.
        template <typename T>
        const T & as();

        // returns a new object of type T
        template <typename T>
        T to() const;

        bool exists(const std::string & key);
        // gets the size of the Value. If bool, string or float, returns 1, if array or object, returns
        // the number of values inside the container.
        size_t size();

        Value & operator=(const int         & rhs);
        Value & operator=(const float       & rhs);
        Value & operator=(const bool        & rhs);
        Value & operator=(const std::string & rhs);
        Value & operator=(const Value       & rhs);
        Value & operator=(const char        * rhs);

        // Access the i'th element in the array. If the value is not an array, it will
        // discard any previous data in the value and create a blank array with at least
        // i+1 size.
        Value & operator[](int i);

        // Access the value with key 'i'. If the value is not an Object, it will discard
        // any previous data and create a blank object.
        Value & operator[](const std::string & i);

        // gets the type of the object.
        const  TYPE  & type() {return _type;};

        // gets the key/value map if the Value is a json Object. Throws exception if it is not an object.
        const std::map<std::string, pValue> & getValueMap()    { if( _type != OBJECT ) throw incorrect_type(); return _object; };

        // gets the vector of values if the Value object is a json Array. Throws exception if it is not an array.
        const std::vector<pValue>           & getValueVector() { if( _type != ARRAY  ) throw incorrect_type(); return _array;  };

        // Parses JSON text.
        void parse(std::istringstream & S);
        void parse(std::string & S);

    private:
        float                           _float;
        bool                            _bool;
        std::vector<pValue>              _array;
        std::map<std::string, pValue>    _object;
        std::string                     _string;
        TYPE _type;


        static std::string                   parseString(std::istringstream & S );
        static bool                          parseBool(  std::istringstream & S );
        static float                         parseNumber(std::istringstream & S );
        static std::vector<pValue>            parseArray (std::istringstream & S );
        static std::string                   parseKey(std::istringstream & S);
        static std::map<std::string, pValue>  parseObject(std::istringstream & S);
};

template<>
inline const std::string & Value::as<std::string>()  {
    if( _type != Value::STRING) throw gnl::json::incorrect_type();
    return _string;
}

template<>
inline const  bool & Value::as<bool>()  {
    if( _type != Value::BOOL) throw gnl::json::incorrect_type();
    return _bool;
}

template<>
inline const  float & Value::as<float>()   {
    if( _type != Value::NUMBER) throw gnl::json::incorrect_type();
    return _float;
}

template<>
inline const  Value & Value::as<gnl::json::Value>()   {
    return *this;
}

};

};



#endif


#ifdef GNL_JSON_IMPLEMENT

#define REMOVEWHITESPACE { char a = S.peek(); while( std::isspace(a) ) {S.get(); a = S.peek(); } }


bool gnl::json::Value::exists(const std::string & key)
{
    if(_type != gnl::json::Value::OBJECT) return false;

    return( (bool)( _object.count(key)>=1)  );
}

gnl::json::Value::Value(const std::string & rhs)
{
    std::istringstream S( rhs );
    parse( S );
}

gnl::json::Value::Value(const gnl::json::Value       & rhs)
{
    if( &rhs != this) init(rhs);
}

void gnl::json::Value::init(const gnl::json::Value & rhs)
{

    switch( rhs._type )
    {
        case Value::BOOL:
            _bool   = rhs._bool; break;

        case Value::STRING:
            _string = rhs._string; break;

        case Value::NUMBER:
            _float = rhs._float; break;

        case Value::ARRAY:
            _array.clear();
            for(auto a : rhs._array)
            {
                _array.push_back( std::make_shared<Value>( *a ) );
            }
            break;

        case Value::OBJECT:
            _object.clear();
            for(auto a : rhs._object)
            {
                _object[a.first] = std::make_shared<Value>(*a.second);
            }
            break;

        case Value::UNKNOWN:
            break;

    }

    _type   = rhs._type;
}

size_t gnl::json::Value::size()
{
    switch( _type )
    {
        case gnl::json::Value::ARRAY:
            return _array.size();
        case gnl::json::Value::OBJECT:
            return _object.size();
        case gnl::json::Value::UNKNOWN:
            return 0;
        default:
            return 1;
    }

}


gnl::json::Value & gnl::json::Value::operator=(const float       & rhs)
{
    clear();
    _type = Value::NUMBER;
    _float = rhs;
    return *this;
}

gnl::json::Value & gnl::json::Value::operator=(const int         & rhs)
{
    clear();
    _type = Value::NUMBER;
    _float = (float)rhs;
    return *this;
}

gnl::json::Value & gnl::json::Value::operator=(const bool        & rhs)
{
    clear();
    _type = Value::BOOL;
    _bool = rhs;
    return *this;
}

gnl::json::Value & gnl::json::Value::operator=(const std::string & rhs)
{
    clear();
    _string = rhs;
    _type   = Value::STRING;
    return *this;
}



gnl::json::Value & gnl::json::Value::operator=(const char   *    rhs)
{
    clear();
    _string = std::string(rhs);
    _type   = Value::STRING;
    return *this;
}


gnl::json::Value & gnl::json::Value::operator=(const json::Value & rhs)
{
    if( this != &rhs) init(rhs);
    return *this;
}

void gnl::json::Value::clear()
{
    switch( _type )
    {
        case Value::STRING:
            _string = "";
            break;
        case Value::OBJECT:
            _object.clear();
            break;

        case Value::ARRAY:
            _array.clear();
            break;
        default: break;
    }
    _type = Value::UNKNOWN;
}


gnl::json::Value::~Value()
{
    clear();
}


gnl::json::Value & gnl::json::Value::operator[](int i)
{

    if( _type != Value::ARRAY)
    {
        clear();
        _type   = gnl::json::Value::ARRAY;
    }


    while( (int)_array.size() < i+1) _array.push_back( std::make_shared<Value>() );

    return *_array[i];
}

gnl::json::Value & gnl::json::Value::operator[](const std::string & i)
{
    if( _type == Value::UNKNOWN)
    {
        _object.clear();
        _type   = gnl::json::Value::OBJECT;
    }

    if( _type != Value::OBJECT) throw gnl::json::incorrect_type();

    if( _object.count(i)==0 ) _object[i] = std::make_shared<Value>();

    //std::cout << "getting value: " << i << std::endl;
    return *_object[i];
}


void gnl::json::Value::parse(std::string &S)
{
    std::istringstream SS(S);
    parse(SS);
}

void gnl::json::Value::parse(std::istringstream &S )
{
    REMOVEWHITESPACE;

    char c = S.peek();

    if(c=='"')
    {
        _string =  Value::parseString(S);
        _type = Value::STRING;
    }
    else if( std::isdigit(c) || c=='-' || c=='+' || c=='.')
    {
        _float = Value::parseNumber(S);
        _type = Value::NUMBER;
    }
    else if( std::tolower(c) == 't' || std::tolower(c) == 'f' )
    {
        _bool = Value::parseBool(S);
        _type = Value::BOOL;
    }
    else if( c == '{' )
    {
        _object = Value::parseObject(S);//new gnl::json::Object();
        _type = Value::OBJECT;
    }
    else if( c == '[' )
    {
      //  std::cout << "ARRAY" << std::endl;
        _array = Value::parseArray(S);
        _type  = Value::ARRAY;

       // std::cout << "Array Found: " << V->_array->mValues.size() << " values." << std::endl;
    } else {
    }

}

bool gnl::json::Value::parseBool(std::istringstream & S)
{
    REMOVEWHITESPACE;
    char c = S.peek();
    if( std::tolower(c) == 't' )
    {
        while( !(c == ',' || c == '}' || c==']' || std::isspace(c)) )
        {
            c = S.get();
        }
        return true;
    }\
    while( !(c == ',' || c == '}' || c==']' || std::isspace(c)) )
    {
        c = S.get();
    }
    return false;

}

float gnl::json::Value::parseNumber(std::istringstream &S)
{
    REMOVEWHITESPACE;

    char c = S.peek();

    std::string num;

    while( std::isdigit(c) || c=='-' || c=='+' || c=='.' || c=='e' || c=='E')
    {
        S.get();
        num += c;
        c = S.peek();
    }

    return(  (float)std::atof(num.c_str() ));
}


std::vector<gnl::json::pValue>  gnl::json::Value::parseArray(std::istringstream &S)
{
    REMOVEWHITESPACE;
    char c = S.get();

    std::vector<gnl::json::pValue>  A;

    if( c == '[' )
    {
        while( c != ']' )
        {
            REMOVEWHITESPACE;

            gnl::json::pValue V = std::make_shared<Value>();
            V->parse(S);
            A.push_back( V );

            REMOVEWHITESPACE;

            c = S.get();
            if(c != ',' && c!=']')
            {
                std::cout << "comma not found\n";
            }
        }

    }

    return(A);
}

std::string gnl::json::Value::parseString(std::istringstream & S)
{
    REMOVEWHITESPACE;

    char c = S.get();

    // can be either:
    //'    usename   :  "gavin",  '
    //         or
    //'   "usename" s :  "gavin"   '

    std::string Key;

    if( c== '"')
    {

        c = S.get();

        while( c != '"' )
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
            Key += c;
            c = S.get();
        }

    }

    // std::cout << "String value Found: " << Key << "  next character (" << S.peek() << std::endl;
    return (Key);
}


std::string gnl::json::Value::parseKey(std::istringstream & S)
{
    REMOVEWHITESPACE;

    char c = S.get();

    // can be either:
    //'    usename   :  "gavin",  '
    //         or
    //'   "usename"  :  "gavin"   '
    std::string Key;

    if( c== '"')
    {
        c = S.get();
        while( c != '"' )
        {
            Key += c;
            c = S.get();
        }

    }
    else
    {
        Key += c;
        c    = S.peek();
        while( !std::isspace(c) && c !=':')
        {
            Key += c;
            S.get();
            c    = S.peek();
        }

    }

    return (Key);
}

std::map<std::string, gnl::json::pValue> gnl::json::Value::parseObject(std::istringstream &S)
{
    REMOVEWHITESPACE;

    //gnl::json::Object * O = new gnl::json::Object();

    char c = S.get();

    //std::cout << "Start Object: " << c << std::endl;

    std::map<std::string, gnl::json::pValue> vMap;

    while(c != '}')
    {
        std::string key = Value::parseKey(S);

        REMOVEWHITESPACE;

        c = S.get();

        if(c !=':')
        {
            std::cout << "Key: " << key << std::endl;
            throw parse_error();
        }

         vMap[key] = std::make_shared<Value>();
         vMap[key]->parse(S);

         REMOVEWHITESPACE;

         c = S.peek();
         if(c == ',' || c =='}' ) c = S.get();

    }

    return vMap;

}



#undef REMOVEWHITESPACE

#endif
