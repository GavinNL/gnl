/*
   gnl_json.h - v1.00 - Public domain JSON reader

   This library is in the public domain and can be used as one sees fit but cannot
   hold the author responsible for any damages that may occur as a result of using
   this code.


   Example:  main.cpp

   Object myJSON;

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
#include <type_traits>

namespace GNL_NAMESPACE
{
    class JSON;
}
inline std::ostream & operator<<(std::ostream &os, const GNL_NAMESPACE::JSON& p);

namespace GNL_NAMESPACE {

class JSON;


class incorrect_type : public std::exception {
public:
    const char * what () const throw ()
    {
      return "Attempted to cast json JSON \"as\" an incorrect type";
    }
};

class parse_error : public std::exception {
    const char * what () const throw ()
    {
      return "Error parsing the json string.";
    }
};

class JSON
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

        JSON() : _type(BOOL)
        {
            Init(BOOL);
        }

        JSON( JSON::TYPE T) : _type(BOOL)
        {
            Init(T);
        }

        JSON(const JSON  & rhs) : _type(BOOL)
        {
            *this = rhs;
        }

        JSON(const std::string & rhs) : _type(BOOL)
        {
          //  std::cout << "init as string" << std::endl;
            Init(STRING);
            *_JSONs._string = rhs;
        }

        JSON(const char * rhs) : _type(BOOL)
        {
          //  std::cout << "init as char*" << std::endl;
            Init(STRING);
            *_JSONs._string = std::string(rhs);
        }

        JSON(const bool & f) : _type(BOOL)
        {
           // std::cout << "init as float*" << std::endl;
            Init(BOOL);
            _JSONs._bool = f;
        }

        JSON(const float & f) : _type(BOOL)
        {
           // std::cout << "init as float*" << std::endl;
            Init(NUMBER);
            _JSONs._float = f;
        }

        JSON( const std::initializer_list<JSON> & l)
        {
            std::cout << "Initializer list: JJSON" << std::endl;
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


        JSON & operator=(JSON && T)
        {
            clear();

            switch( T._type )
            {
                case BOOL:
                    _JSONs._bool   = T._JSONs._bool; break;
                case NUMBER:
                    _JSONs._float  = T._JSONs._float; break;
                case STRING:
                    _JSONs._string = T._JSONs._string; break;
                case ARRAY:
                    _JSONs._array  = T._JSONs._array; break;
                case OBJECT:
                    _JSONs._object = T._JSONs._object; break;
//                UNKNOWN:
//                    break;
                default:
                    break;
            }

            _type = T._type;

         //   T._type = UNKNOWN;
            T._JSONs._string = 0;
            T._JSONs._array  = 0;
            T._JSONs._object = 0;

            return *this;
        }

        JSON(JSON && T)
        {
            //std::cout << "Move constructor\n";
            *this = std::move( T );
        }

        ~JSON()
        {
            clear();
        }



        void init(const JSON & rhs);


        void merge( const JSON & rhs )
        {

            if( _type == JSON::OBJECT && rhs._type == JSON::OBJECT)
            {
                for( auto & a : rhs.getJSONMap() )
                {
                    (*this)[a.first].merge( a.second);
                }
            }
            else
            {
                *this = rhs;
            }
        }

        // clears the JSON and sets it's type to BOOL
        void clear()
        {
            switch( _type )
            {
                case JSON::STRING:
                    if(_JSONs._string) delete _JSONs._string;
                    break;
                case JSON::OBJECT:
                    if(_JSONs._object) delete _JSONs._object;
                    break;
                case JSON::ARRAY:
                    if(_JSONs._array) delete _JSONs._array;
                    break;
                default: break;
            }
            _JSONs._bool = false;
            _type = JSON::BOOL;
            //std::cout << "Finished clearing\n";
        }


        // Interprets the JSON as a specific type: bool, float, string.
        template <typename T>
        const T & as() const;

        // returns a new object of type T
        template <typename T>
        T to() const;

        bool exists(const std::string & key) const;
        // gets the size of the JSON. If bool, string or float, returns 1, if array or object, returns
        // the number of JSONs inside the container.
        size_t size() const;

        JSON & operator=(const float       & rhs)
        {
            Init(JSON::NUMBER);

            _type = JSON::NUMBER;
            _JSONs._float = rhs;

            return *this;
        }

        JSON & operator=(const int         & rhs)
        {
            Init(JSON::NUMBER);

            _JSONs._float = (float)rhs;

            return *this;
        }

        JSON & operator=(const bool        & rhs)
        {
            Init(JSON::BOOL);
            _JSONs._bool = rhs;
            return *this;
        }

        JSON & operator=(const std::string & rhs)
        {
            Init(JSON::STRING);
            *_JSONs._string = rhs;
            return *this;
        }


        JSON & operator=(const char   *    rhs)
        {
            Init(JSON::STRING);
            *_JSONs._string = std::string(rhs);
            return *this;
        }

        template<typename T>
        JSON & operator=(const T & rhs);

        std::string strType() const
        {
            switch(_type)
            {
                case UNKNOWN: return "UNKNOWN";
                case NUMBER:  return "NUMBER";
                case ARRAY:   return "ARRAY";
                case OBJECT:  return "OBJECT";
                case STRING:  return "STRING";
                case BOOL:    return "BOOL";
            }
            return "unknown";
        }

        JSON & operator=(const JSON       & rhs)
        {
            if( this == &rhs ) return *this;

            if( _type != rhs._type) Init( rhs._type );

            switch( rhs._type )
            {
                case BOOL:
                    _JSONs._bool = rhs._JSONs._bool; break;
                case NUMBER:
                    _JSONs._float = rhs._JSONs._float; break;
                case STRING:
                    *_JSONs._string = *rhs._JSONs._string; break;
                case ARRAY:
                    *_JSONs._array = *rhs._JSONs._array; break;
                case OBJECT:
                    *_JSONs._object = *rhs._JSONs._object;
                    break;
             //   case UNKNOWN:
                default:
                    break;
            }
            _type = rhs._type;

            return *this;
        }


        JSON & operator=(std::initializer_list<std::string> l)
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

        JSON & operator=( std::initializer_list<float> l)
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

        JSON & operator=( std::initializer_list<int> l)
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

        JSON & operator=( std::initializer_list<JSON> l)
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
        //        Generic Operators
        //==========================================================================
        template<TYPE t, typename op_type>
        bool op_equals( const op_type & right )
        {
            if( t != type() ) return false;
            return this->as<t>()==right;
        }
        template<TYPE t, typename op_type>
        bool op_nequals( const op_type & right )
        {
            if( t != type() ) return false;
            return this->as<t>()!=right;
        }
        template<TYPE t, typename op_type>
        bool op_less( const op_type & right )
        {
            if( t != type() ) return false;
            return this->as<t>()<right;
        }
        template<TYPE t, typename op_type>
        bool op_less_qual( const op_type & right )
        {
            if( t != type() ) return false;
            return this->as<t>()<=right;
        }
        template<TYPE t, typename op_type>
        bool op_grtr( const op_type & right )
        {
            if( t != type() ) return false;
            return this->as<t>()>right;
        }
        template<TYPE t, typename op_type>
        bool op_grtr_equal( const op_type & right )
        {
            if( t != type() ) return false;
            return this->as<t>()>=right;
        }
        //==========================================================================
        //        == operator
        //==========================================================================
        bool operator==(const bool & right)
        {
            if( _type == BOOL) return _JSONs._bool==right;

            return false;
        }

        bool operator==(const float & right)
        {
            if( _type == NUMBER) return _JSONs._float==right;

            return false;
        }

        bool operator==(const int & right)
        {
            if( _type == NUMBER) return _JSONs._float==right;

            return false;
        }

        bool operator==(const std::string & right)
        {
            if( _type == STRING) return *_JSONs._string==right;

            return false;
        }

        bool operator==(const char * right)
        {
            if( _type == STRING) return *_JSONs._string==std::string(right);

            return false;
        }

        bool operator==(const JSON & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_JSONs._string == *right._JSONs._string;
                    case NUMBER: return _JSONs._float   ==  right._JSONs._float;
                    case BOOL:   return _JSONs._bool    ==  right._JSONs._bool;
                    default:
                        return false;
                }
            }

            return false;
        }




        //==========================================================================
        //        != operator
        //==========================================================================
        bool operator!=(const bool & right)
        {
            if( _type == BOOL) return _JSONs._bool!=right;

            return false;
        }

        bool operator!=(const float & right)
        {
            if( _type == NUMBER) return _JSONs._float!=right;

            return false;
        }

        bool operator!=(const int & right)
        {
            if( _type == NUMBER) return _JSONs._float!=right;

            return false;
        }

        bool operator!=(const std::string & right)
        {
            if( _type == STRING) return *_JSONs._string!=right;

            return false;
        }

        bool operator!=(const char * right)
        {
            if( _type == STRING) return *_JSONs._string!=std::string(right);

            return false;
        }

        bool operator!=(const JSON & right)
        {
            if( _type != right._type)
            {
                switch( _type )
                {
                    case STRING: return *_JSONs._string != *right._JSONs._string;
                    case NUMBER: return  _JSONs._float  !=  right._JSONs._float;
                    case BOOL:   return  _JSONs._bool   !=  right._JSONs._bool;
                    default:
                        return false;
                }
            }

            return false;
        }


        //==========================================================================
        //        < operator
        //==========================================================================
        bool operator<(const bool & right)
        {
            if( _type == BOOL) return _JSONs._bool<right;

            return false;
        }

        bool operator<(const float & right)
        {
            if( _type == NUMBER) return _JSONs._float<right;

            return false;
        }

        bool operator<(const int & right)
        {
            if( _type == NUMBER) return _JSONs._float<right;

            return false;
        }

        bool operator<(const std::string & right)
        {
            if( _type == STRING) return *_JSONs._string<right;

            return false;
        }

        bool operator<(const char * right)
        {
            if( _type == STRING) return *_JSONs._string<std::string(right);

            return false;
        }

        bool operator<(const JSON & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_JSONs._string < *right._JSONs._string;
                    case NUMBER: return _JSONs._float   <  right._JSONs._float;
                    case BOOL:   return _JSONs._bool    <  right._JSONs._bool;
                    default:
                        return false;
                }
            }

            return false;
        }

        //==========================================================================
        //        > operator
        //==========================================================================
        bool operator>(const bool & right)
        {
            if( _type == BOOL) return _JSONs._bool>right;

            return false;
        }

        bool operator>(const float & right)
        {
            if( _type == NUMBER) return _JSONs._float>right;

            return false;
        }

        bool operator>(const int & right)
        {
            if( _type == NUMBER) return _JSONs._float>right;

            return false;
        }

        bool operator>(const std::string & right)
        {
            if( _type == STRING) return *_JSONs._string>right;

            return false;
        }

        bool operator>(const char * right)
        {
            if( _type == STRING) return *_JSONs._string>std::string(right);

            return false;
        }

        bool operator>(const JSON & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_JSONs._string > *right._JSONs._string;
                    case NUMBER: return _JSONs._float   >  right._JSONs._float;
                    case BOOL:   return _JSONs._bool    >  right._JSONs._bool;
                default:
                    return false;
                }
            }

            return false;
        }

        //==========================================================================
        //        >= operator
        //==========================================================================

        bool operator>=(const bool & right)
        {
            if( _type == BOOL) return _JSONs._bool>=right;

            return false;
        }

        bool operator>=(const float & right)
        {
            if( _type == NUMBER) return _JSONs._float>=right;

            return false;
        }

        bool operator>=(const int & right)
        {
            if( _type == NUMBER) return _JSONs._float>=right;

            return false;
        }

        bool operator>=(const std::string & right)
        {
            if( _type == STRING) return *_JSONs._string>=right;

            return false;
        }

        bool operator>=(const char * right)
        {
            if( _type == STRING) return *_JSONs._string>=std::string(right);

            return false;
        }

        bool operator>=(const JSON & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_JSONs._string >= *right._JSONs._string;
                    case NUMBER: return _JSONs._float   >=  right._JSONs._float;
                    case BOOL:   return _JSONs._bool    >=  right._JSONs._bool;
                default:
                    return false;
                }
            }

            return false;
        }

        //==========================================================================
        //        <= operator
        //==========================================================================
		

        bool operator<=(const bool & right)
        {	
            if( _type == BOOL) return _JSONs._bool<=right;

            return false;
        }

        bool operator<=(const float & right)
        {
            if( _type == NUMBER) return _JSONs._float<=right;

            return false;
        }

        bool operator<=(const int & right)
        {
            if( _type == NUMBER) return _JSONs._float<=right;

            return false;
        }

        bool operator<=(const std::string & right)
        {
            if( _type == STRING) return *_JSONs._string<=right;

            return false;
        }

        bool operator<=(const char * right)
        {
            if( _type == STRING) return *_JSONs._string<=std::string(right);

            return false;
        }


        bool operator<=(const JSON & right)
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_JSONs._string <= *right._JSONs._string;
                    case NUMBER: return _JSONs._float   <=  right._JSONs._float;
                    case BOOL:   return _JSONs._bool    <=  right._JSONs._bool;
                default:
                    return false;
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
                case BOOL:   _JSONs._bool    = false;                              break;
                case NUMBER: _JSONs._float   = 0.0f;                               break;
                case STRING: _JSONs._string  = new std::string();                  break;
                case ARRAY:  _JSONs._array   = new std::vector<JSON>();           break;
                case OBJECT: _JSONs._object  = new std::map<std::string, JSON>(); break;
                default:
                    _type = UNKNOWN;
                    break;
            }
        }

		
        // Access the i'th element in the array. If the JSON is not an array, it will
        // discard any previous data in the JSON and create a blank array with at least
        // i+1 size.
        JSON & operator[](int i);

        // Access the JSON with key 'i'. If the JSON is not an Object, it will discard
        // any previous data and create a blank object.
        JSON & operator[](const std::string & i);
        JSON & operator[](const char i[]);

        // get's a reference to the array index or a object name. Will throw an exception if they do not exist.
        const JSON & get(const char * i) const;
        const JSON & get(const std::string & i) const;
        const JSON & get(              int   i) const;

        template<class T>
        T get(const std::string & i, const T & DefaultJSON) const
        {
            if( _type != JSON::OBJECT) return DefaultJSON;//throw incorrect_type();

            auto f = _JSONs._object->find(i);
            if( f == _JSONs._object->end() ) return DefaultJSON;

            return f->second.to<T>();
        }

        template<class T>
        T get(const int i, const T & DefaultJSON) const
        {
            if( _type != JSON::ARRAY) return DefaultJSON;//throw incorrect_type();

			if( i < size() )
                return _JSONs._array->at(i).to<T>();

            return DefaultJSON;
        }

        void erase(const std::string & i)
        {
            if(_type == OBJECT)
            {
                _JSONs._object->erase(i);
            }
        }

        void erase(              int   i)
        {
            if(_type==ARRAY)
            {
                _JSONs._array->erase(std::begin(*_JSONs._array) + i );
            }
        }


        bool has(const std::string & i, TYPE type=UNKNOWN) const
        {
            if( _type != OBJECT) return false;

            auto f = _JSONs._object->find(i);
            if( f == _JSONs._object->end() ) return false;

            if( type==UNKNOWN) return true;

            if( f->second.type() == type ) return true;

            return true;
        }

        // gets the type of the object.
        const  TYPE  & type() const {return _type;}

        // gets the key/JSON map if the JSON is a json Object. Throws exception if it is not an object.
        const std::map<std::string, JSON> & getJSONMap()  const  { if( _type != OBJECT ) throw std::runtime_error("JSON JSON is not an OBJECT"); return *_JSONs._object; }

        // gets the vector of JSONs if the JSON object is a json Array. Throws exception if it is not an array.
        const std::vector<JSON>           & getJSONVector() const { if( _type != ARRAY  ) throw std::runtime_error("JSON JSON is not an ARRAY"); return *_JSONs._array;  }

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
                                                 // JSONs in an object matter.
    public:
        union
        {
            float                            _float;
            bool                             _bool;
			unsigned long					 _long;
            std::vector<JSON>              *_array;
            std::map<std::string, JSON>    *_object;
            std::string                     *_string;
        }  _JSONs;


        TYPE _type;


        static std::string                   parseString(std::istringstream & S );
        static bool                          parseBool(  std::istringstream & S );
        static float                         parseNumber(std::istringstream & S );
        static std::vector<JSON>             parseArray (std::istringstream & S );
        static std::string                   parseKey(   std::istringstream & S );
        static std::map<std::string, JSON>   parseObject(std::istringstream & S );
};


template<>
inline JSON::operator int()  const    { if(_type == JSON::NUMBER) return (int)_JSONs._float;  return 0; }

template<>
inline JSON::operator float()  const  { if(_type == JSON::NUMBER) return _JSONs._float;  return 0.0f;}

template<>
inline JSON::operator std::string() const   { if(_type == JSON::STRING) return *_JSONs._string;  return "";}

template<>
inline JSON::operator bool() const  { if(_type == JSON::BOOL) return _JSONs._bool;  return 0;}




template<>
inline const std::string & JSON::as<std::string>()  const {
    if( _type != JSON::STRING) throw incorrect_type();
    return *_JSONs._string;
}

template<>
inline const  bool & JSON::as<bool>()  const {
    if( _type != JSON::BOOL) throw incorrect_type();
    return _JSONs._bool;
}

template<>
inline const  float & JSON::as<float>()  const  {
    if( _type != JSON::NUMBER) throw incorrect_type();
    return _JSONs._float;
}

template<>
inline const  JSON & JSON::as<JSON>()  const  {
    return *this;
}


template<>
inline std::string JSON::to<std::string>()  const {
    if( _type != JSON::STRING ) return std::string();
    return *_JSONs._string;
}

template<>
inline  bool  JSON::to<bool>()  const {
    if( _type != JSON::BOOL) return false;
    return _JSONs._bool;
}

template<>
inline  float  JSON::to<float>()  const  {
    if( _type != JSON::NUMBER) return 0.0f;
    return _JSONs._float;
}








 
inline const JSON & JSON::get(const char *i) const
{
    if( _type != JSON::OBJECT) throw incorrect_type();
  //  std::cout << "getting JSON: " << i << std::endl;
    return _JSONs._object->at( std::string(i) );
}

inline const JSON & JSON::get(const std::string & i) const
{
    if( _type != JSON::OBJECT) throw incorrect_type();
  //  std::cout << "getting JSON: " << i << std::endl;
    return _JSONs._object->at(i);
}

inline const JSON & JSON::get(              int   i) const
{
    if( _type != JSON::ARRAY) throw incorrect_type();
  //  std::cout << "getting index: " << i << "   size: " << _JSONs._array->size() << std::endl;
    return _JSONs._array->at(i);
}




};

 

#define REMOVEWHITESPACE { char a = S.peek(); while( std::isspace(a) ) {S.get(); a = S.peek(); } }

namespace GNL_NAMESPACE
{



inline bool JSON::exists(const std::string & key) const
{
    if(_type != JSON::OBJECT) return false;

    return( (bool)( _JSONs._object->count(key)>=1)  );
}

// JSON::JSON(const std::string & rhs)
// {
//     std::istringstream S( rhs );
//     parse( S );
// }

inline size_t JSON::size() const
{
    switch( _type )
    {
        case JSON::ARRAY:
            return _JSONs._array->size();
        case JSON::OBJECT:
            return _JSONs._object->size();
        case JSON::UNKNOWN:
            return 0;
        default:
            return 1;
    }

}


inline JSON & JSON::operator[](int i)
{

    if( _type != JSON::ARRAY)
    {
        Init(JSON::ARRAY);

    }

    if( (int)_JSONs._array->size() <= i ) _JSONs._array->resize(i+1);


    return (*_JSONs._array)[i];
}

inline JSON & JSON::operator[](const std::string & i)
{
    if( _type != JSON::OBJECT)
    {
        Init(JSON::OBJECT);
    }

    //std::cout << "getting JSON: " << i << std::endl;
    return (*_JSONs._object)[i];
}

inline JSON & JSON::operator[](const char i[])
{
    if( _type != JSON::OBJECT)
    {
        Init(JSON::OBJECT);
    }

    return (*_JSONs._object)[i];
}




inline void JSON::parse(const std::string &S)
{
    std::istringstream SS(S);
    parse(SS);
}

inline void JSON::parse(std::istringstream &S )
{
    REMOVEWHITESPACE;

    char c = S.peek();
    //::cout << c << std::endl;
    switch(c)
    {
        case  '"': // string
            Init(JSON::STRING);
            *_JSONs._string =  JSON::parseString(S);
            break;

        case 't': // bool
        case 'f': // bool
            Init(JSON::BOOL);
            _JSONs._bool = JSON::parseBool(S);
            break;
        case '{': // object
            Init( JSON::OBJECT );
            *_JSONs._object = std::move( JSON::parseObject(S));//new Object();
            break;
        case '[': // array
            Init( JSON::ARRAY );
            *_JSONs._array = std::move( JSON::parseArray(S) );
            break;
        default: // number

            if( std::isdigit(c) || c=='-' || c=='+' || c=='.')
            {
                Init(JSON::NUMBER);
                _JSONs._float = JSON::parseNumber(S);
               // std::cout << "Number found: " << _JSONs._float << std::endl;
            }
            break;
    }

}

inline bool JSON::parseBool(std::istringstream & S)
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

 

inline float JSON::parseNumber(std::istringstream &S)
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


inline std::vector<JSON>  JSON::parseArray(std::istringstream &S)
{
 //   std::cout << "Array found\n";
    REMOVEWHITESPACE;
    char c = S.get();

    std::vector<JSON>  A;

    if( c == '[' )
    {
        while( c != ']' )
        {
            REMOVEWHITESPACE;

            JSON V;

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


inline std::string JSON::parseString(std::istringstream & S)
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

    // std::cout << "String JSON Found: " << Key << "  next character (" << S.peek() << std::endl;
    return (Key);
}


inline std::string JSON::parseKey(std::istringstream & S)
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

inline std::map<std::string, JSON> JSON::parseObject(std::istringstream &S)
{

    REMOVEWHITESPACE;

    //Object * O = new Object();

    char c = S.get();

  //  std::cout << "Start Object: " << c << std::endl;

    std::map<std::string, JSON> vMap;
    int count = 0;
    while(c != '}')
    {
        std::string key = JSON::parseKey(S);

        REMOVEWHITESPACE;

        c = S.get();

        if(c != ':' )
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

inline std::ostream & __FormatOutput(std::ostream &os, const GNL_NAMESPACE::JSON & p, std::string & spaces)
{
    switch( p._type )
    {
        case GNL_NAMESPACE::JSON::UNKNOWN:   return os << "false";
        case GNL_NAMESPACE::JSON::NUMBER:    return os << p._JSONs._float;
        case GNL_NAMESPACE::JSON::STRING:    return os << '"' << *p._JSONs._string << '"' ;
        case GNL_NAMESPACE::JSON::ARRAY:
        {
            os << "[";
			int s=0;
            for(auto & a : *p._JSONs._array)
            {
                if(a._type == GNL_NAMESPACE::JSON::OBJECT)
				{
					__FormatOutput(os, a, spaces);
				} else {
                os << a;
				}
				s++;
                if( s < (int)p.size() ) os << ",";
            }
            return os << "]";
        }
        case GNL_NAMESPACE::JSON::OBJECT:
        {
			int maxsize=0;

            for(auto & a : *p._JSONs._object)
			{
                if( (int)a.first.size() > maxsize ) maxsize = a.first.size();
			}

            os << std::endl << spaces << "{" << std::endl;
			spaces += "\t";
			int s = 0;
            for(auto & a : *p._JSONs._object)
            {
                os << spaces << '"' << a.first << '"' <<  std::string( maxsize-a.first.size(), ' ') << " : " ;
                __FormatOutput(os, a.second, spaces);
				s++;
                if( s < (int)p.size() ) os << ",";
				os << std::endl;
            }
			spaces.resize( spaces.size()-1 );
            return os << spaces << "}";
        }
        case GNL_NAMESPACE::JSON::BOOL:
			
            return os << (p._JSONs._bool ? "true" : "false");
    }

	return os;
}

inline std::ostream & operator<<(std::ostream &os, const GNL_NAMESPACE::JSON & p)
{
	std::string spaces;
	__FormatOutput(os, p, spaces);
	return os;
}

#undef REMOVEWHITESPACE
#undef GNL_NAMESPACE
#endif
