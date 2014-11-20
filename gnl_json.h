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


class incorrect_cast : public std::exception {
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

        template <typename T>
        T & as();

        inline Value & operator[](int i);
        inline Value & operator[](const std::string & i);
        const  TYPE  & type() {return _type;};


        union {
            float                 _float;
            bool                  _bool;
            std::vector<Value*> * _array;
            Object              * _object;
            std::string         * _string;
        };

        TYPE _type;

        void parse(std::istringstream & S);

        static std::string          parseString(std::istringstream & S );
        static bool                 parseBool(  std::istringstream & S );
        static float                parseNumber(std::istringstream & S );
        static std::vector<Value*>* parseArray (std::istringstream & S );
};

template<>
inline std::string & Value::as<std::string>() {
    if( _type != Value::STRING) throw gnl::json::incorrect_cast();
    return *_string;
}

template<>
inline bool & Value::as<bool>() {
    if( _type != Value::BOOL) throw gnl::json::incorrect_cast();
    return _bool;
}

template<>
inline float & Value::as<float>() {
    if( _type != Value::NUMBER) throw gnl::json::incorrect_cast();
    return _float;
}

template<>
inline Value & Value::as<Value>() {
    return *this;
}


class Object
{
public:
    inline Value & operator[](const std::string & i) { return *mValues[i];}

    void parse(std::istringstream &S);

    static std::string  parseKey(std::istringstream & S);

    std::map<std::string, Value*> mValues;
};



};

};



#endif


#ifdef GNL_JSON_IMPLEMENT

#define REMOVEWHITESPACE { char a = S.peek(); while( std::isspace(a) ) {S.get(); a = S.peek(); } }

gnl::json::Value::~Value()
{
    switch( _type )
    {
        case Value::STRING: delete(_string); break;
        case Value::OBJECT: delete(_object); break;
        case Value::ARRAY:  for(auto a : *_array) delete(a);  break;

        default: break;

    }
}


inline gnl::json::Value & gnl::json::Value::operator[](int i)
{
    if( _type != Value::ARRAY) throw gnl::json::incorrect_cast();
    return *(*_array)[i];
}

inline gnl::json::Value & gnl::json::Value::operator[](const std::string & i)
{
    if( _type != Value::OBJECT) throw gnl::json::incorrect_cast();
    return *_object->mValues[i];
}


void gnl::json::Value::parse(std::istringstream &S)
{
    REMOVEWHITESPACE;

    char c = S.peek();
    //gnl::json::Value * V = new Value();

    if(c=='"')
    {
        _string = new std::string( Value::parseString(S) );
        _type = Value::STRING;
    }
    else if( std::isdigit(c) || c=='-' || c=='+' || c=='.')
    {
        _float = Value::parseNumber(S);
        _type = Value::NUMBER;
      //  std::cout << "Number found: " << V->_float << std::endl;
    }
    else if( std::tolower(c) == 't' || std::tolower(c) == 'f' )
    {
        _bool = Value::parseBool(S);
        _type = Value::BOOL;
       // std::cout << "Bool found: " << V->_bool << std::endl;
    }
    else if( c == '{' )
    {
        _object = new gnl::json::Object();
        _object->parse(S);
        _type = Value::OBJECT;
       // std::cout << "Object Found: " << V->_object->mValues.size() << " values." << std::endl;
    }
    else if( c == '[' )
    {
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


std::vector<gnl::json::Value*>* gnl::json::Value::parseArray(std::istringstream &S)
{
    REMOVEWHITESPACE;
    char c = S.get();

    std::vector<gnl::json::Value*> * A = new std::vector<gnl::json::Value*>();

    if( c == '[' )
    {
        while( c != ']' )
        {
            REMOVEWHITESPACE;

            gnl::json::Value * V = new Value();
            V->parse(S);
            A->push_back(V);

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

std::string gnl::json::Object::parseKey(std::istringstream & S)
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
        c = S.peek();
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

void gnl::json::Object::parse(std::istringstream &S)
{


    REMOVEWHITESPACE;

    //gnl::json::Object * O = new gnl::json::Object();

    char c = S.get();

    //std::cout << "Start Object: " << c << std::endl;

    while(c != '}')
    {
        std::string key = Object::parseKey(S);

        REMOVEWHITESPACE;

        c = S.get();
        if(c !=':')
        {
            throw parse_error();
        }

         Value *     V   = new Value();
         V->Value::parse(S);
         REMOVEWHITESPACE;

         mValues[key] = V;
         std::cout << "(" << key << ")" << std::endl;
         c = S.peek();
         if(c == ',') c = S.get();

    }
   // std::cout << "end Object\n";
    //return O;

}

#undef REMOVEWHITESPACE

#endif
