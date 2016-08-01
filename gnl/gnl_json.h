/*
   gnl_json.h - v1.00 - Public domain JSON reader

   This library is in the public domain and can be used as one sees fit but cannot
   hold the author responsible for any damages that may occur as a result of using
   this code.


   Example:  main.cpp

   json::Object myJSON;

   myJSON.parse( S );


 */


#ifndef GNL_JSON_H__
#define GNL_JSON_H__

 #ifndef GNL_NAMESPACE
 #define GNL_NAMESPACE gnl
 #endif

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <exception>
#include <memory>
#include <fstream>
#include <cstdlib>
#include <initializer_list>

namespace GNL_NAMESPACE
{
	namespace json {
		class Value;
	}
}
inline std::ostream & operator<<(std::ostream &os, const GNL_NAMESPACE::json::Value& p);

namespace GNL_NAMESPACE {

namespace json {

class Value;


typedef std::shared_ptr<Value> pValue;


class incorrect_type : public std::exception {
public:
    const char * what () const throw ()
    {
      return "Attempted to cast json Value \"as\" an incorrect type";
    }
};

class parse_error : public std::exception {
    const char * what () const throw ()
    {
      return "Error parsing the json string.";
    }
};

class Value
{

    public:

        typedef enum
        {
            UNKNOWN,
            BOOL,
            NUMBER,
            STRING,
            ARRAY,
            OBJECT
        } TYPE;

        Value() : _type(UNKNOWN)
        {
            Init(UNKNOWN);
         //   std::cout << "New constructor\n"    ;
        }

        Value( Value::TYPE T) : _type(UNKNOWN)
        {
            Init(T);
        }

        Value(const Value  & rhs) : _type(UNKNOWN)
        {
            *this = rhs;
        }

        Value(const std::string & rhs) : _type(UNKNOWN)
        {
          //  std::cout << "init as string" << std::endl;
            Init(STRING);
            *_Values._string = rhs;
        }

        Value(const char * rhs) : _type(UNKNOWN)
        {
          //  std::cout << "init as char*" << std::endl;
            Init(STRING);
            *_Values._string = std::string(rhs);
        }

        Value(const bool & f) : _type(UNKNOWN)
        {
           // std::cout << "init as float*" << std::endl;
            Init(BOOL);
            _Values._bool = f;
        }

        Value(const float & f) : _type(UNKNOWN)
        {
           // std::cout << "init as float*" << std::endl;
            Init(NUMBER);
            _Values._float = f;
        }

        Value( const std::initializer_list<json::Value> & l)
        {
            std::cout << "Initializer list: Jvalue" << std::endl;
            clear();
            Init(ARRAY);
            int i=0;
            for(auto a : l)
            {
                std::cout << a << std::endl;
                (*this)[i] = a;
                i++;
            }

        }


        Value & operator=(Value && T)
        {
           // std::cout << "Value:: Move operator\n";
            clear();

            switch( T._type )
            {
                BOOL:
                    _Values._bool   = T._Values._bool; break;
                NUMBER:
                    _Values._float  = T._Values._float; break;
                STRING:
                    _Values._string = T._Values._string; break;
                ARRAY:
                    _Values._array  = T._Values._array; break;
                OBJECT:
                    _Values._object = T._Values._object; break;
                UNKNOWN:
                    break;
                default:
                    break;
            }

            _type = T._type;

            T._type = UNKNOWN;
            T._Values._string = 0;
            T._Values._array  = 0;
            T._Values._object = 0;

            return *this;
        }

        Value(Value && T)
        {
            //std::cout << "Move constructor\n";
            *this = std::move( T );
        }

        ~Value()
        {
            clear();
        }



        void init(const Value & rhs);


        void merge( const Value & rhs )
        {

            if( _type == Value::OBJECT && rhs._type == Value::OBJECT)
            {
                for( auto & a : rhs.getValueMap() )
                {
                    (*this)[a.first].merge( a.second);
                }
            }
            else
            {
                *this = rhs;
            }
        }

        // clears the Value and sets it's type to UNKNOWN
        void clear()
        {
            switch( _type )
            {
                case Value::STRING:
                    if(_Values._string) delete _Values._string;
                    break;
                case Value::OBJECT:
                    if(_Values._object) delete _Values._object;
                    break;
                case Value::ARRAY:
                    if(_Values._array) delete _Values._array;
                    break;
                default: break;
            }
            _Values._bool = false;
            _type = Value::BOOL;
            //std::cout << "Finished clearing\n";
        }


        // Interprets the Value as a specific type: bool, float, string.
        template <typename T>
        const T & as() const;

        // returns a new object of type T
        template <typename T>
        T to() const;

        bool exists(const std::string & key) const;
        // gets the size of the Value. If bool, string or float, returns 1, if array or object, returns
        // the number of values inside the container.
        size_t size() const;

        Value & operator=(const float       & rhs)
        {
            Init(Value::NUMBER);

            _type = Value::NUMBER;
            _Values._float = rhs;

            return *this;
        }

        Value & operator=(const int         & rhs)
        {
            Init(Value::NUMBER);

            _Values._float = (float)rhs;

            return *this;
        }

        Value & operator=(const bool        & rhs)
        {
            Init(Value::BOOL);
            _Values._bool = rhs;
            return *this;
        }

        Value & operator=(const std::string & rhs)
        {
            Init(Value::STRING);
            *_Values._string = rhs;
            return *this;
        }


        Value & operator=(const char   *    rhs)
        {
            Init(Value::STRING);
            *_Values._string = std::string(rhs);
            return *this;
        }

        template<typename T>
        Value & operator=(const T & rhs);

        std::string strType() const
        {
            switch(_type)
            {
                case UNKNOWN: return "UNKNOWN";
                case NUMBER:  return "NUMBER";
                case ARRAY:   return "ARRAY";
                case OBJECT:  return "OBJECT";
                case BOOL:    return "BOOL";
            }
            return "unknown";
        }

        Value & operator=(const Value       & rhs)
        {
            if( this == &rhs ) return *this;

            if( _type != rhs._type) Init( rhs._type );

            switch( rhs._type )
            {
                case BOOL:
                    _Values._bool = rhs._Values._bool; break;
                case NUMBER:
                    _Values._float = rhs._Values._float; break;
                case STRING:
                    *_Values._string = *rhs._Values._string; break;
                case ARRAY:
                    *_Values._array = *rhs._Values._array; break;
                case OBJECT:
                    *_Values._object = *rhs._Values._object;
                    break;
                case UNKNOWN:
                default:
                    break;
            }
            _type = rhs._type;

            return *this;
        }


        Value & operator=(std::initializer_list<std::string> l)
        {
            clear();

            int i = 0;

            for(auto a : l)
            {
                (*this)[i] = a;
                i++;
            }

            return *this;
        }

        Value & operator=( std::initializer_list<float> l)
        {
            clear();

            int i=0;
            for(auto a : l)
            {
                (*this)[i] = a;
                i++;
            }

            return *this;
        }

        Value & operator=( std::initializer_list<int> l)
        {
            clear();

            int i=0;
            for(auto a : l)
            {
                (*this)[i] = a;
                i++;
            }

            return *this;
        }

        Value & operator=( std::initializer_list<json::Value> l)
        {
            clear();

            int i=0;
            for(auto a : l)
            {
                (*this)[i] = a;
                i++;
            }

            return *this;
        }

        //==========================================================================
        //        == operator
        //==========================================================================
        bool operator==(const bool & right)
        {
            if( _type == BOOL) return _Values._bool==right;

            return false;
        }

        bool operator==(const float & right)
        {
            if( _type == NUMBER) return _Values._float==right;

            return false;
        }

        bool operator==(const int & right)
        {
            if( _type == NUMBER) return _Values._float==right;

            return false;
        }

        bool operator==(const std::string & right)
        {
            if( _type == STRING) return *_Values._string==right;

            return false;
        }

        bool operator==(const char * right)
        {
            if( _type == STRING) return *_Values._string==std::string(right);

            return false;
        }

        bool operator==(const json::Value & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_Values._string == *right._Values._string;
                    case NUMBER: return _Values._float   ==  right._Values._float;
                    case BOOL:   return _Values._bool    ==  right._Values._bool;
                }
            }

            return false;
        }

        //==========================================================================
        //        != operator
        //==========================================================================
        bool operator!=(const bool & right)
        {
            if( _type == BOOL) return _Values._bool!=right;

            return false;
        }

        bool operator!=(const float & right)
        {
            if( _type == NUMBER) return _Values._float!=right;

            return false;
        }

        bool operator!=(const int & right)
        {
            if( _type == NUMBER) return _Values._float!=right;

            return false;
        }

        bool operator!=(const std::string & right)
        {
            if( _type == STRING) return *_Values._string!=right;

            return false;
        }

        bool operator!=(const char * right)
        {
            if( _type == STRING) return *_Values._string!=std::string(right);

            return false;
        }

        bool operator!=(const json::Value & right)
        {
            if( _type != right._type)
            {
                switch( _type )
                {
                    case STRING: return *_Values._string != *right._Values._string;
                    case NUMBER: return  _Values._float  !=  right._Values._float;
                    case BOOL:   return  _Values._bool   !=  right._Values._bool;
                }
            }

            return false;
        }

        //==========================================================================
        //        < operator
        //==========================================================================
        bool operator<(const bool & right)
        {
            if( _type == BOOL) return _Values._bool<right;

            return false;
        }

        bool operator<(const float & right)
        {
            if( _type == NUMBER) return _Values._float<right;

            return false;
        }

        bool operator<(const int & right)
        {
            if( _type == NUMBER) return _Values._float<right;

            return false;
        }

        bool operator<(const std::string & right)
        {
            if( _type == STRING) return *_Values._string<right;

            return false;
        }

        bool operator<(const char * right)
        {
            if( _type == STRING) return *_Values._string<std::string(right);

            return false;
        }

        bool operator<(const json::Value & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_Values._string < *right._Values._string;
                    case NUMBER: return _Values._float   <  right._Values._float;
                    case BOOL:   return _Values._bool    <  right._Values._bool;
                }
            }

            return false;
        }

        //==========================================================================
        //        > operator
        //==========================================================================
        bool operator>(const bool & right)
        {
            if( _type == BOOL) return _Values._bool>right;

            return false;
        }

        bool operator>(const float & right)
        {
            if( _type == NUMBER) return _Values._float>right;

            return false;
        }

        bool operator>(const int & right)
        {
            if( _type == NUMBER) return _Values._float>right;

            return false;
        }

        bool operator>(const std::string & right)
        {
            if( _type == STRING) return *_Values._string>right;

            return false;
        }

        bool operator>(const char * right)
        {
            if( _type == STRING) return *_Values._string>std::string(right);

            return false;
        }

        bool operator>(const json::Value & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_Values._string > *right._Values._string;
                    case NUMBER: return _Values._float   >  right._Values._float;
                    case BOOL:   return _Values._bool    >  right._Values._bool;
                }
            }

            return false;
        }

        //==========================================================================
        //        >= operator
        //==========================================================================
        bool operator>=(const bool & right)
        {
            if( _type == BOOL) return _Values._bool>=right;

            return false;
        }

        bool operator>=(const float & right)
        {
            if( _type == NUMBER) return _Values._float>=right;

            return false;
        }

        bool operator>=(const int & right)
        {
            if( _type == NUMBER) return _Values._float>=right;

            return false;
        }

        bool operator>=(const std::string & right)
        {
            if( _type == STRING) return *_Values._string>=right;

            return false;
        }

        bool operator>=(const char * right)
        {
            if( _type == STRING) return *_Values._string>=std::string(right);

            return false;
        }

        bool operator>=(const json::Value & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_Values._string >= *right._Values._string;
                    case NUMBER: return _Values._float   >=  right._Values._float;
                    case BOOL:   return _Values._bool    >=  right._Values._bool;
                }
            }

            return false;
        }

        //==========================================================================
        //        <= operator
        //==========================================================================
		

        bool operator<=(const bool & right)
        {
			
            if( _type == BOOL) return _Values._bool<=right;

            return false;
        }

        bool operator<=(const float & right)
        {
            if( _type == NUMBER) return _Values._float<=right;

            return false;
        }

        bool operator<=(const int & right)
        {
            if( _type == NUMBER) return _Values._float<=right;

            return false;
        }

        bool operator<=(const std::string & right)
        {
            if( _type == STRING) return *_Values._string<=right;

            return false;
        }

        bool operator<=(const char * right)
        {
            if( _type == STRING) return *_Values._string<=std::string(right);

            return false;
        }


        bool operator<=(const json::Value & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_Values._string <= *right._Values._string;
                    case NUMBER: return _Values._float   <=  right._Values._float;
                    case BOOL:   return _Values._bool    <=  right._Values._bool;
                }
            }

            return false;
        }




        void Init( TYPE T)
        {
            clear( );
            _type = T;
            switch(T)
            {
                case BOOL:   _Values._bool    = false;                              break;
                case NUMBER: _Values._float   = 0.0f;                               break;
                case STRING: _Values._string  = new std::string();                  break;
                case ARRAY:  _Values._array   = new std::vector<Value>();           break;
                case OBJECT: _Values._object  = new std::map<std::string, Value>(); break;
                default:
                    _type = UNKNOWN;
                    break;
            }
        }

		
        // Access the i'th element in the array. If the value is not an array, it will
        // discard any previous data in the value and create a blank array with at least
        // i+1 size.
        Value & operator[](int i);

        // Access the value with key 'i'. If the value is not an Object, it will discard
        // any previous data and create a blank object.
        Value & operator[](const std::string & i);
        Value & operator[](const char i[]);

        // get's a reference to the array index or a object name. Will throw an exception if they do not exist.
        const Value & get(const char * i) const;
        const Value & get(const std::string & i) const;
        const Value & get(              int   i) const;

        template<class T>
        T get(const std::string & i, const T & DefaultValue) const
        {
            if( _type != Value::OBJECT) return DefaultValue;//throw json::incorrect_type();

            auto f = _Values._object->find(i);
            if( f == _Values._object->end() ) return DefaultValue;

            return f->second.to<T>();
        }

        template<class T>
        T get(const int i, const T & DefaultValue) const
        {
            if( _type != Value::ARRAY) return DefaultValue;//throw json::incorrect_type();

			if( i < size() )
				return _Values._array->at(i).to<T>();

            return DefaultValue;
        }
        void erase(const std::string & i)
        {
            if(_type == OBJECT)
            {
                _Values._object->erase(i);
            }
        }

        void erase(              int   i)
        {
            if(_type==ARRAY)
            {
                _Values._array->erase(std::begin(*_Values._array) + i );
            }
        }

        template<TYPE type=UNKNOWN>
        bool has(const std::string & i) const
        {
            if( _type != OBJECT) return false;

            auto f = _Values._object->find(i);
            if( f == _Values._object->end() ) return false;

            if( type==UNKNOWN) return true;

            if( f->second.type() == type ) return true;

            return true;
        }

        // gets the type of the object.
        const  TYPE  & type() const {return _type;}

        // gets the key/value map if the Value is a json Object. Throws exception if it is not an object.
        const std::map<std::string, Value> & getValueMap()  const  { if( _type != OBJECT ) throw incorrect_type(); return *_Values._object; }

        // gets the vector of values if the Value object is a json Array. Throws exception if it is not an array.
        const std::vector<Value>           & getValueVector() const { if( _type != ARRAY  ) throw incorrect_type(); return *_Values._array;  }

        // Parses JSON text.
        void parse(std::istringstream & S);
        void parse(const std::string & S);
        bool parseFromPath(const std::string & path)
        {

            std::ifstream t(path.c_str());

            if( !t.good() )
            {
                throw std::runtime_error( std::string("Cannot open file: ") + path);
                return false;
            }

            std::string str((std::istreambuf_iterator<char>(t)),
                             std::istreambuf_iterator<char>());
            parse(str);

            return true;

        }

        template <class T>
        operator T() const;

        //operator float()  { return to<float>(); }


        uint                            _order;  // the order in the Object. In case the order of
                                                 // values in an object matter.
    public:
        union
        {
            float                            _float;
            bool                             _bool;
			unsigned long					 _long;
            std::vector<Value>              *_array;
            std::map<std::string, Value>    *_object;
            std::string                     *_string;
        }  _Values;


        TYPE _type;


        static std::string                   parseString(std::istringstream & S );
        static bool                          parseBool(  std::istringstream & S );
        static float                         parseNumber(std::istringstream & S );
        static std::vector<Value>            parseArray (std::istringstream & S );
        static std::string                   parseKey(   std::istringstream & S );
        static std::map<std::string, Value>  parseObject(std::istringstream & S );
};


template<>
inline Value::operator int()  const    { if(_type == Value::NUMBER) return (int)_Values._float;  return 0; }

template<>
inline Value::operator float()  const  { if(_type == Value::NUMBER) return _Values._float;  return 0.0f;}

template<>
inline Value::operator std::string() const   { if(_type == Value::STRING) return *_Values._string;  return "";}

template<>
inline Value::operator bool() const  { if(_type == Value::BOOL) return _Values._bool;  return 0;}

template<>
inline const std::string & Value::as<std::string>()  const {
    if( _type != Value::STRING) throw json::incorrect_type();
    return *_Values._string;
}

template<>
inline const  bool & Value::as<bool>()  const {
    if( _type != Value::BOOL) throw json::incorrect_type();
    return _Values._bool;
}

template<>
inline const  float & Value::as<float>()  const  {
    if( _type != Value::NUMBER) throw json::incorrect_type();
    return _Values._float;
}

template<>
inline const  Value & Value::as<json::Value>()  const  {
    return *this;
}


template<>
inline std::string Value::to<std::string>()  const {
    if( _type != Value::STRING ) return std::string();
    return *_Values._string;
}

template<>
inline  bool  Value::to<bool>()  const {
    if( _type != Value::BOOL) return false;
    return _Values._bool;
}

template<>
inline  float  Value::to<float>()  const  {
    if( _type != Value::NUMBER) return 0.0f;
    return _Values._float;
}








 
inline const json::Value & json::Value::get(const char *i) const
{
    if( _type != Value::OBJECT) throw json::incorrect_type();
  //  std::cout << "getting value: " << i << std::endl;
    return _Values._object->at( std::string(i) );
}

inline const json::Value & json::Value::get(const std::string & i) const
{
    if( _type != Value::OBJECT) throw json::incorrect_type();
  //  std::cout << "getting value: " << i << std::endl;
    return _Values._object->at(i);
}

inline const json::Value & json::Value::get(              int   i) const
{
    if( _type != Value::ARRAY) throw json::incorrect_type();
  //  std::cout << "getting index: " << i << "   size: " << _Values._array->size() << std::endl;
    return _Values._array->at(i);
}


};

};

 

#define REMOVEWHITESPACE { char a = S.peek(); while( std::isspace(a) ) {S.get(); a = S.peek(); } }

namespace GNL_NAMESPACE
{



inline bool json::Value::exists(const std::string & key) const
{
    if(_type != json::Value::OBJECT) return false;

    return( (bool)( _Values._object->count(key)>=1)  );
}

// json::Value::Value(const std::string & rhs)
// {
//     std::istringstream S( rhs );
//     parse( S );
// }

inline size_t json::Value::size() const
{
    switch( _type )
    {
        case json::Value::ARRAY:
            return _Values._array->size();
        case json::Value::OBJECT:
            return _Values._object->size();
        case json::Value::UNKNOWN:
            return 0;
        default:
            return 1;
    }

}


inline json::Value & json::Value::operator[](int i)
{

    if( _type != Value::ARRAY)
    {
        Init(Value::ARRAY);

    }

    if( _Values._array->size() <= i ) _Values._array->resize(i+1);


    return (*_Values._array)[i];
}

inline json::Value & json::Value::operator[](const std::string & i)
{
    if( _type != Value::OBJECT)
    {
        Init(Value::OBJECT);
    }

    //std::cout << "getting value: " << i << std::endl;
    return (*_Values._object)[i];
}

inline json::Value & json::Value::operator[](const char i[])
{
    if( _type != Value::OBJECT)
    {
        Init(Value::OBJECT);
    }

    return (*_Values._object)[i];
}




inline void json::Value::parse(const std::string &S)
{
    std::istringstream SS(S);
    parse(SS);
}

inline void json::Value::parse(std::istringstream &S )
{
    REMOVEWHITESPACE;

    char c = S.peek();
    //::cout << c << std::endl;
    switch(c)
    {
        case  '"': // string
            Init(Value::STRING);
            *_Values._string =  Value::parseString(S);
            break;

        case 't': // bool
        case 'f': // bool
            Init(Value::BOOL);
            _Values._bool = Value::parseBool(S);
            break;
        case '{': // object
            Init( Value::OBJECT );
            *_Values._object = std::move( Value::parseObject(S));//new json::Object();
            break;
        case '[': // array
            Init( Value::ARRAY );
            *_Values._array = std::move( Value::parseArray(S) );
            break;
        default: // number

            if( std::isdigit(c) || c=='-' || c=='+' || c=='.')
            {
                Init(Value::NUMBER);
                _Values._float = Value::parseNumber(S);
               // std::cout << "Number found: " << _Values._float << std::endl;
            }
            break;
    }

}

inline bool json::Value::parseBool(std::istringstream & S)
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
    }
    while( !(c == ',' || c == '}' || c==']' || std::isspace(c)) )
    {
        c = S.get();
    }
    return false;

}

 

inline float json::Value::parseNumber(std::istringstream &S)
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
    float N = (float)std::atof(num.c_str() );
    //std::cout << "Number found: " << N << "\n";
    return( N );
}


inline std::vector<json::Value>  json::Value::parseArray(std::istringstream &S)
{
 //   std::cout << "Array found\n";
    REMOVEWHITESPACE;
    char c = S.get();

    std::vector<json::Value>  A;

    if( c == '[' )
    {
        while( c != ']' )
        {
            REMOVEWHITESPACE;

            json::Value V;

            V.parse(S);

            // A.push_back( std::move(V) );
            A.push_back( V );

            REMOVEWHITESPACE;

            c = S.get();
            if(c != ',' && c!=']')
            {
           //     std::cout << "comma not found\n";
            }
        }

    }

   // for(auto & a : A)
    {
      //  std::cout << "=====" << a.to<float>() << std::endl;
    }



    return( std::move(A) );
}


inline std::string json::Value::parseString(std::istringstream & S)
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


inline std::string json::Value::parseKey(std::istringstream & S)
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

inline std::map<std::string, json::Value> json::Value::parseObject(std::istringstream &S)
{

    REMOVEWHITESPACE;

    //json::Object * O = new json::Object();

    char c = S.get();

  //  std::cout << "Start Object: " << c << std::endl;

    std::map<std::string, json::Value> vMap;
    int count = 0;
    while(c != '}')
    {
        std::string key = Value::parseKey(S);

        REMOVEWHITESPACE;

        c = S.get();

        if(c !=':')
        {
            //std::cout << "Key: " << key << std::endl;
            throw parse_error();
        }

        vMap[key].parse(S);
        vMap[key]._order = count;
        count++;

        REMOVEWHITESPACE;

         c = S.peek();
         if(c == ',' || c =='}' ) c = S.get();

    }

    return std::move(vMap);

}




}

inline std::ostream & __FormatOutput(std::ostream &os, const GNL_NAMESPACE::json::Value & p, std::string & spaces)
{
    switch( p._type )
    {
        case GNL_NAMESPACE::json::Value::UNKNOWN:   return os << "false";
        case GNL_NAMESPACE::json::Value::NUMBER:    return os << p._Values._float;
        case GNL_NAMESPACE::json::Value::STRING:    return os << '"' << *p._Values._string << '"' ;
        case GNL_NAMESPACE::json::Value::ARRAY:
        {
            os << "[";
			int s=0;
            for(auto & a : *p._Values._array)
            {
				if(a._type == GNL_NAMESPACE::json::Value::OBJECT)
				{
					__FormatOutput(os, a, spaces);
				} else {
                os << a;
				}
				s++;
                if( s < p.size() ) os << ",";
            }
            return os << "]";
        }
		case GNL_NAMESPACE::json::Value::OBJECT:
        {
			int maxsize=0;

			for(auto & a : *p._Values._object)
			{
				if( a.first.size() > maxsize ) maxsize = a.first.size();
			}

            os << std::endl << spaces << "{" << std::endl;
			spaces += "\t";
			int s = 0;
            for(auto & a : *p._Values._object)
            {
                os << spaces << '"' << a.first << '"' <<  std::string( maxsize-a.first.size(), ' ') << " : " ;
                __FormatOutput(os, a.second, spaces);
				s++;
				if( s < p.size() ) os << ",";
				os << std::endl;
            }
			spaces.resize( spaces.size()-1 );
            return os << spaces << "}";
        }
        case GNL_NAMESPACE::json::Value::BOOL:   
			
			return os << (p._Values._bool ? "true" : "false");
    }

	return os;
}

inline std::ostream & operator<<(std::ostream &os, const GNL_NAMESPACE::json::Value & p)
{
	std::string spaces;
	__FormatOutput(os, p, spaces);
	return os;
}

#undef REMOVEWHITESPACE
#undef GNL_NAMESPACE
#endif
