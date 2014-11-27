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

namespace gnl {

namespace json {

class Value;
class Array;
class Object;


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

        Value() {
            _type = UNKNOWN;
        }

        ~Value();

        // clears the Value and sets it's type to UNKNOWN
        void clear();

        // Interprets the Value as a specific type: bool, float, string.
        template <typename T>
        T & as();

        // returns a new object of type T
        template <typename T>
        T to() const;

        // Access the i'th element in the array. If the value is not an array, it will
        // discard any previous data in the value and create a blank array with at least
        // i+1 size.
        inline Value & operator[](int i);

        // Access the value with key 'i'. If the value is not an Object, it will discard
        // any previous data and create a blank object.
        inline Value & operator[](const std::string & i);

        // gets the size of the Value. If bool, string or float, returns 1, if array or object, returns
        // the number of values inside the container.
        size_t size();

        Value & operator=(const int         & rhs);
        Value & operator=(const float       & rhs);
        Value & operator=(const bool        & rhs);
        Value & operator=(const std::string & rhs);
        Value & operator=(const Value       & rhs);

        // gets the type of the object.
        const  TYPE  & type() {return _type;};

        // gets the key/value map if the Value is a json Object. Throws exception if it is not an object.
        const std::map<std::string, Value> & getValueMap()    { if( _type != OBJECT ) throw incorrect_type(); return *_object; };

        // gets the vector of values if the Value object is a json Array. Throws exception if it is not an array.
        const std::vector<Value>           & getValueVector() { if( _type != ARRAY  ) throw incorrect_type(); return *_array;  };

        // Parses JSON text.
        void parse(std::istringstream & S);

    private:
        union {
            float                           _float;
            bool                            _bool;
            std::vector<Value>           * _array;
            std::map<std::string, Value> * _object;
            std::string                   * _string;
        };
        TYPE _type;


        static std::string          parseString(std::istringstream & S );
        static bool                 parseBool(  std::istringstream & S );
        static float                parseNumber(std::istringstream & S );
        static std::vector<Value>* parseArray (std::istringstream & S );

        static std::string                   parseKey(std::istringstream & S);
        static std::map<std::string, Value>* parseObject(std::istringstream & S);
};

template<>
inline std::string & Value::as<std::string>()  {
    if( _type != Value::STRING) throw gnl::json::incorrect_type();
    return *_string;
}

template<>
inline bool & Value::as<bool>()  {
    if( _type != Value::BOOL) throw gnl::json::incorrect_type();
    return _bool;
}

template<>
inline float & Value::as<float>()   {
    if( _type != Value::NUMBER) throw gnl::json::incorrect_type();
    return _float;
}

template<>
inline Value & Value::as<gnl::json::Value>()   {
    return *this;
}

//template<>
//inline Object & Value::as<gnl::json::Object>()   {
//    if( _type != Value::OBJECT) throw gnl::json::incorrect_cast();
//    return *_object;
//}

//class Object
//{
//public:

//    ~Object();
//    Value & operator[](const std::string & i);
//    void clear();
//    Object & operator=(const Object      & rhs);

//    std::map<std::string, Value*> & getValues() { return mValues; }

//    void parse(const std::string & input);
//    void parse(std::istringstream &S);

//    static std::string  parseKey(std::istringstream & S);

//    std::map<std::string, Value*> mValues;
//};



};

};



#endif


#ifdef GNL_JSON_IMPLEMENT

#define REMOVEWHITESPACE { char a = S.peek(); while( std::isspace(a) ) {S.get(); a = S.peek(); } }


size_t gnl::json::Value::size()
{
    switch( _type )
    {
        case gnl::json::Value::ARRAY:
            return _array->size();
        case gnl::json::Value::OBJECT:
            return _object->size();
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
    _string = new std::string( rhs );
    _type   = Value::STRING;
    return *this;
}



gnl::json::Value & gnl::json::Value::operator=(const json::Value & rhs)
{
    clear();
    _type   = rhs._type;
    switch( _type )
    {
        case Value::BOOL:
            _bool   = rhs._bool; break;
        case Value::STRING:
            _string = new std::string( *rhs._string ); break;
        case Value::NUMBER:
            _float = rhs._float; break;
        case Value::ARRAY:
            _array = new std::vector<json::Value>();
            for(auto i=0; i < rhs._array->size(); i++)
            {
                //json::Value * V = new json::Value();
                //*V = rhs._array->at(i);
                _array->push_back( rhs._array->at(i) );
            }
            break;
        case Value::OBJECT:
            _object = new std::map<std::string,gnl::json::Value>();
            for(auto a : *rhs._object)
            {
                //(*_object)[a.first] = new gnl::json::Value();
                (*_object)[a.first] = a.second;
            }

//            _object = new gnl::json::Object();
//            *_object = *rhs._object;
            break;
        case Value::UNKNOWN:
        break;
    }

    return *this;
}

void gnl::json::Value::clear()
{
    switch( _type )
    {
        case Value::STRING: delete(_string); break;
        case Value::OBJECT:
            {
               // for(auto a : *_object) delete( a.second );
            }
            delete(_object); break;

        case Value::ARRAY:
            //for(auto a : *_array) delete(a);
            delete(_array);
            break;
        default: break;
    }
    _type = Value::UNKNOWN;
}


gnl::json::Value::~Value()
{
    clear();
}


inline gnl::json::Value & gnl::json::Value::operator[](int i)
{

    if( _type != Value::ARRAY)
    {
        clear();
        _array  = new std::vector<gnl::json::Value>();
        _array->resize(i+1);
        _type   = gnl::json::Value::ARRAY;
    }

    return (*_array)[i];
}

inline gnl::json::Value & gnl::json::Value::operator[](const std::string & i)
{
    if( _type == Value::UNKNOWN)
    {
        _object = new std::map<std::string, gnl::json::Value>();
        _type   = gnl::json::Value::OBJECT;
    }

    if( _type != Value::OBJECT) throw gnl::json::incorrect_type();

    if( !_object->count(i) ) (*_object)[i] = new gnl::json::Value();

    std::cout << "getting value: " << i << std::endl;
    return (*_object)[i];
}


void gnl::json::Value::parse(std::istringstream &S)
{
    REMOVEWHITESPACE;

    char c = S.peek();

    if(c=='"')
    {
        _string = new std::string( Value::parseString(S) );
       // std::cout << "STRING: " <<  *_string << std::endl;
        _type = Value::STRING;
    }
    else if( std::isdigit(c) || c=='-' || c=='+' || c=='.')
    {
        _float = Value::parseNumber(S);
        _type = Value::NUMBER;
      //  std::cout << "NUMBER: " << _float << std::endl;
      //  std::cout << "Number found: " << V->_float << std::endl;
    }
    else if( std::tolower(c) == 't' || std::tolower(c) == 'f' )
    {
       // std::cout << "BOOL" << std::endl;
        _bool = Value::parseBool(S);
        _type = Value::BOOL;
       // std::cout << "Bool found: " << V->_bool << std::endl;
    }
    else if( c == '{' )
    {
       // std::cout << "OBJECT" << std::endl;
        _object = Value::parseObject(S);//new gnl::json::Object();
        //_object->parse(S);
        _type = Value::OBJECT;
       // std::cout << "Object Complete: " << (char)S.peek() << " values." << std::endl;
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

    while( std::isdigit(c) || c=='-' || c=='+' || c=='.')
    {
        S.get();
        num += c;
        c = S.peek();
    }

    return(  (float)std::atof(num.c_str() ));
}


std::vector<gnl::json::Value>* gnl::json::Value::parseArray(std::istringstream &S)
{
    REMOVEWHITESPACE;
    char c = S.get();

    std::vector<gnl::json::Value> * A = new std::vector<gnl::json::Value>();

    if( c == '[' )
    {
        while( c != ']' )
        {
            REMOVEWHITESPACE;

            gnl::json::Value V;
            A->push_back(V);
            A->at( A->size()-1 ).parse(S);

            REMOVEWHITESPACE;

            c = S.get();
            if(c != ',' && c!=']')
            {
                std::cout << "comma not found\n";
            }
        }

    }

    //std::cout << "Array finished: size: " << A->size() << std::endl;
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
    //std::cout << "Key Found: " << Key << "  next character (" << S.peek() << std::endl;
    return (Key);
}

std::map<std::string, gnl::json::Value>* gnl::json::Value::parseObject(std::istringstream &S)
{
    REMOVEWHITESPACE;

    //gnl::json::Object * O = new gnl::json::Object();

    char c = S.get();

    //std::cout << "Start Object: " << c << std::endl;

    std::map<std::string, gnl::json::Value> * vMap = new std::map<std::string, gnl::json::Value>();

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

         //Value *     V   = new Value();
         //std::cout << key << ": ";
         //V->Value::parse(S);

         (*vMap)[key].parse(S);

         REMOVEWHITESPACE;

         c = S.peek();
         if(c == ',' || c =='}' ) c = S.get();

    }

    return vMap;

}















//void gnl::json::Object::parse(const std::string & input) {
//  std::istringstream is( input );
//  parse( is );
//}


//void gnl::json::Object::clear()
//{
//    for(auto a : mValues){
//        delete a.second;
//    }
//    mValues.clear();
//}


//gnl::json::Object::~Object()
//{
//    clear();
//}


//gnl::json::Object & gnl::json::Object::operator=(const gnl::json::Object      & rhs)
//{
//    for(auto a : rhs.mValues)
//    {
//        gnl::json::Value * V = new gnl::json::Value();
//        *V = *a.second;
//        mValues[a.first] = V;
//    }
//}


//std::string gnl::json::Object::parseKey(std::istringstream & S)
//{
//    REMOVEWHITESPACE;

//    char c = S.get();

//    // can be either:
//    //'    usename   :  "gavin",  '
//    //         or
//    //'   "usename"  :  "gavin"   '

//    std::string Key;

//    if( c== '"')
//    {
//        c = S.get();
//        while( c != '"' )
//        {
//            Key += c;
//            c = S.get();
//        }

//    }
//    else
//    {
//        Key += c;
//        c    = S.peek();
//        while( !std::isspace(c) && c !=':')
//        {
//            Key += c;
//            S.get();
//            c    = S.peek();
//        }

//    }
//    //std::cout << "Key Found: " << Key << "  next character (" << S.peek() << std::endl;
//    return (Key);
//}

//void gnl::json::Object::parse(std::istringstream &S)
//{

//    REMOVEWHITESPACE;

//    //gnl::json::Object * O = new gnl::json::Object();

//    char c = S.get();

//    //std::cout << "Start Object: " << c << std::endl;

//    while(c != '}')
//    {
//        std::string key = Object::parseKey(S);

//        REMOVEWHITESPACE;

//        c = S.get();
//        if(c !=':')
//        {
//            std::cout << "Key: " << key << std::endl;
//            throw parse_error();
//        }

//         Value *     V   = new Value();
//         //std::cout << key << ": ";
//         V->Value::parse(S);

//         mValues[key] = V;


//         REMOVEWHITESPACE;

//         c = S.peek();
//         if(c == ',' || c =='}' ) c = S.get();

//    }

//}

#undef REMOVEWHITESPACE

#endif
