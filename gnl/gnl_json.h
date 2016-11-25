/*
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/



#ifndef GNL_JSON_H
#define GNL_JSON_H

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
#include <limits>
#include <cmath>
#include <cstdint>
#include <cctype>

namespace GNL_NAMESPACE
{
    class json;
}

std::ostream & operator<<(std::ostream & os, GNL_NAMESPACE::json const & p);

namespace GNL_NAMESPACE {

class json;


class incorrect_type : public std::exception {
public:
    const char * what () const throw ()
    {
      return "Attempted to cast json json \"as\" an incorrect type";
    }
};

class parse_error : public std::exception {
    const char * what () const throw ()
    {
      return "Error parsing the json string.";
    }
};

class json
{
    private:
    template<class T>
    typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
        almost_equal(T x, T y, int ulp)
    {
        // the machine epsilon has to be scaled to the magnitude of the values used
        // and multiplied by the desired precision in ULPs (units in the last place)
        return std::abs(x-y) < std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
        // unless the result is subnormal
               || std::abs(x-y) < std::numeric_limits<T>::min();
    }


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

        json() : _type(BOOL)
        {
            Init(BOOL);
        }

        json( json::TYPE T) : _type(BOOL)
        {
            Init(T);
        }

        json(const json  & rhs) : _type(BOOL)
        {
            *this = rhs;
        }

        json(const std::string & rhs) : _type(BOOL)
        {
          //  std::cout << "init as string" << std::endl;
            Init(STRING);
            *_jsons._string = rhs;
        }

        json(const char * rhs) : _type(BOOL)
        {
          //  std::cout << "init as char*" << std::endl;
            Init(STRING);
            *_jsons._string = std::string(rhs);
        }

        json(const bool & f) : _type(BOOL)
        {
           // std::cout << "init as float*" << std::endl;
            Init(BOOL);
            _jsons._bool = f;
        }

        json(const int & f) : _type(BOOL)
        {
           // std::cout << "init as float*" << std::endl;
            Init(NUMBER);
            _jsons._float = static_cast<float>(f);
        }

        json(const double & f) : _type(BOOL)
        {
           // std::cout << "init as float*" << std::endl;
            Init(NUMBER);
            _jsons._float = static_cast<float>(f);
        }

        json(const float & f) : _type(BOOL)
        {
           // std::cout << "init as float*" << std::endl;
            Init(NUMBER);
            _jsons._float = f;
        }

        json( const std::initializer_list<json> & l)
        {
           // std::cout << "Initializer list: Jjson" << std::endl;
            clear();
            Init(ARRAY);
            int i=0;
            for(auto a : l)
            {
                //std::cout << a << std::endl;
                (*this)[i] = a;
                i++;
            }

        }


        json & operator=(json && T)
        {
            clear();

            _jsons = T._jsons;


            _type   = T._type;
            T._type = BOOL;
            T._jsons._string = 0;
            T._jsons._array  = 0;
            T._jsons._object = 0;

            return *this;
        }

        json(json && T)
        {
            //std::cout << "Move constructor\n";
            *this = std::move( T );
        }

        ~json()
        {
            clear();
        }



        void init(const json & rhs);


        void merge( const json & rhs )
        {

            if( _type == json::OBJECT && rhs._type == json::OBJECT)
            {
                for( auto & a : rhs.getjsonMap() )
                {
                    (*this)[a.first].merge( a.second);
                }
            }
            else
            {
                *this = rhs;
            }
        }

        // clears the json and sets it's type to BOOL
        void clear()
        {
            switch( _type )
            {
                case json::STRING:
                    if(_jsons._string) delete _jsons._string;
                    break;
                case json::OBJECT:
                    if(_jsons._object) delete _jsons._object;
                    break;
                case json::ARRAY:
                    if(_jsons._array)  delete _jsons._array;
                    break;
                case json::UNKNOWN:
                case json::BOOL:
                case json::NUMBER:
                default:
                    break;
            }
            _jsons._bool = false;
            _type = json::BOOL;
            //std::cout << "Finished clearing\n";
        }


        // Interprets the json as a specific type: bool, float, string.
        template <typename T>
        const T & as() const;

        // returns a new object of type T
        template <typename T>
        T to() const;

        bool exists(const std::string & key) const;
        // gets the size of the json. If bool, string or float, returns 1, if array or object, returns
        // the number of jsons inside the container.
        size_t size() const;

        json & operator=(const float       & rhs)
        {
            Init(json::NUMBER);

            _type = json::NUMBER;
            _jsons._float = rhs;

            return *this;
        }

        json & operator=(const int         & rhs)
        {
            Init(json::NUMBER);

            _jsons._float = (float)rhs;

            return *this;
        }

        json & operator=(const bool        & rhs)
        {
            Init(json::BOOL);
            _jsons._bool = rhs;
            return *this;
        }

        json & operator=(const std::string & rhs)
        {
            Init(json::STRING);
            *_jsons._string = rhs;
            return *this;
        }


        json & operator=(const char   *    rhs)
        {
            Init(json::STRING);
            *_jsons._string = std::string(rhs);
            return *this;
        }

        template<typename T>
        json & operator=(const T & rhs);

        std::string strType() const
        {
            switch(_type)
            {
                default:
                case UNKNOWN: return "UNKNOWN";
                case NUMBER:  return "NUMBER";
                case ARRAY:   return "ARRAY";
                case OBJECT:  return "OBJECT";
                case STRING:  return "STRING";
                case BOOL:    return "BOOL";
            }

        }

        json & operator=(const json       & rhs)
        {
            if( this == &rhs ) return *this;

            if( _type != rhs._type) Init( rhs._type );

            switch( rhs._type )
            {
                case BOOL:
                    _jsons._bool = rhs._jsons._bool; break;
                case NUMBER:
                    _jsons._float = rhs._jsons._float; break;
                case STRING:
                    *_jsons._string = *rhs._jsons._string; break;
                case ARRAY:
                    *_jsons._array = *rhs._jsons._array; break;
                case OBJECT:
                    *_jsons._object = *rhs._jsons._object;
                    break;
                case UNKNOWN:
                default:
                    break;
            }
            _type = rhs._type;

            return *this;
        }

        json & operator=( const std::initializer_list<json> & l)
        {
            clear();

            int i=0;
            for(auto & a : l)
            {
                (*this)[i] = a;
                i++;
            }

            return *this;
        }

        //==========================================================================
        //        == operator
        //==========================================================================

        #define OPERATOR(op) \
        template <typename T>                        \
        bool operator op (const T & right) const     \
        {                                            \
            return *this op json(right);             \
        }

        OPERATOR( == )
        OPERATOR( != )
        OPERATOR( <  )
        OPERATOR( >  )
        OPERATOR( >= )
        OPERATOR( <= )

        bool operator==(const json & right) const
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_jsons._string == *right._jsons._string;
                    case NUMBER: return fabs(_jsons._float-right._jsons._float)<1e-7;
                    case BOOL:   return _jsons._bool    ==  right._jsons._bool;
                    case ARRAY:
                        return _jsons._array->size() == right._jsons._array->size()
                            && std::equal(_jsons._array->begin(), _jsons._array->end(),
                                          right._jsons._array->begin());

                    case UNKNOWN:
                    case OBJECT:
                    return _jsons._object->size() == right._jsons._object->size()
                        && std::equal(_jsons._object->begin(), _jsons._object->end(),
                                      right._jsons._object->begin());
                    default:
                        return false;
                }
            }

            return false;
        }


        //==========================================================================
        //        != operator
        //==========================================================================
        bool operator != (const json & right) const
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_jsons._string != *right._jsons._string;
                    case NUMBER: return  fabs(_jsons._float - right._jsons._float) > 1e-7;
                    case BOOL  : return  _jsons._bool   !=  right._jsons._bool;
                    case ARRAY :
                        return _jsons._array->size() != right._jsons._array->size()
                            || !std::equal(_jsons._array->begin(), _jsons._array->end(),
                                          right._jsons._array->begin());


                    case OBJECT:
                    case UNKNOWN:
                    default:
                        return false;
                }
            }

            return true;
        }

        //==========================================================================
        //        < operator
        //==========================================================================

        bool operator<(const json & right) const
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_jsons._string < *right._jsons._string;
                    case NUMBER: return _jsons._float   <  right._jsons._float;
                    case BOOL:   return _jsons._bool    <  right._jsons._bool;
                    case ARRAY:
                    case OBJECT:
                    case UNKNOWN:
                    default:
                        return false;
                }
            }

            return false;
        }

        //==========================================================================
        //        > operator
        //==========================================================================

        bool operator>(const json & right) const
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_jsons._string > *right._jsons._string;
                    case NUMBER: return _jsons._float   >  right._jsons._float;
                    case BOOL:   return _jsons._bool    >  right._jsons._bool;
                case ARRAY:
                case OBJECT:
                case UNKNOWN:
                default:
                    return false;
                }
            }

            return false;
        }

        //==========================================================================
        //        >= operator
        //==========================================================================
        bool operator>=(const json & right) const
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_jsons._string >= *right._jsons._string;
                    case NUMBER: return _jsons._float   >=  right._jsons._float;
                    case BOOL:   return _jsons._bool    >=  right._jsons._bool;
                case ARRAY:
                case OBJECT:
                case UNKNOWN:
                default:
                    return false;
                }
            }

            return false;
        }

        //==========================================================================
        //        <= operator
        //==========================================================================
        bool operator<=(const json & right) const
        {
            if( _type == right._type)
            {
                switch( _type )
                {
                    case STRING: return *_jsons._string <= *right._jsons._string;
                    case NUMBER: return _jsons._float   <=  right._jsons._float;
                    case BOOL:   return _jsons._bool    <=  right._jsons._bool;
                case ARRAY:
                case OBJECT:
                case UNKNOWN:
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
                case BOOL:   _jsons._bool    = false;                             break;
                case NUMBER: _jsons._float   = 0.0f;                              break;
                case STRING: _jsons._string  = new std::string();                 break;
                case ARRAY:  _jsons._array   = new std::vector<json>();           break;
                case OBJECT: _jsons._object  = new std::map<std::string, json>(); break;
                case UNKNOWN:
                default:
                    _type = UNKNOWN;
                    break;
            }
        }


        // Access the i'th element in the array. If the json is not an array, it will
        // discard any previous data in the json and create a blank array with at least
        // i+1 size.
        json & operator[](int i);

        // Access the json with key 'i'. If the json is not an Object, it will discard
        // any previous data and create a blank object.
        json & operator[](const std::string & i);
        json & operator[](const char i[]);

        // get's a reference to the array index or a object name. Will throw an exception if they do not exist.
        const json & get(const char * i) const;
        const json & get(const std::string & i) const;
        const json & get(              int   i) const;

        template<class T>
        T get(const std::string & i, const T & Defaultjson) const
        {
            if( _type != json::OBJECT) return Defaultjson;//throw incorrect_type();

            auto f = _jsons._object->find(i);
            if( f == _jsons._object->end() ) return Defaultjson;

            return f->second.to<T>();
        }

        template<class T>
        T get(const int i, const T & Defaultjson) const
        {
            if( _type != json::ARRAY) return Defaultjson;//throw incorrect_type();

            if( i < size() )
                return _jsons._array->at(i).to<T>();

            return Defaultjson;
        }

        void erase(const std::string & i)
        {
            if(_type == OBJECT)
            {
                _jsons._object->erase(i);
            }
        }

        void erase( int i)
        {
            if(_type==ARRAY)
            {
                _jsons._array->erase(std::begin(*_jsons._array) + i );
            }
        }


        bool has(const std::string & i, TYPE type_=UNKNOWN) const
        {
            if( _type != OBJECT) return false;

            auto f = _jsons._object->find(i);
            if( f == _jsons._object->end() ) return false;

            if( type_ == UNKNOWN) return true;

            if( f->second.type() == type_ ) return true;

            return true;
        }

        // gets the type of the object.
        const  TYPE  & type() const {return _type;}

        // gets the key/json map if the json is a json Object. Throws exception if it is not an object.
        const std::map<std::string, json> & getjsonMap()  const  { if( _type != OBJECT ) throw std::runtime_error("json is not an OBJECT"); return *_jsons._object; }

        // gets the vector of jsons if the json object is a json Array. Throws exception if it is not an array.
        const std::vector<json>           & getjsonVector() const { if( _type != ARRAY  ) throw std::runtime_error("json is not an ARRAY"); return *_jsons._array;  }

        // Parses json text.
        void parse(std::istringstream & S);
        void parse(const std::string  & S);
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
            parse( str );

            return true;

        }


#ifdef _MSC_VER
        //operator int()  const    { if(_type == json::NUMBER) return (int)_jsons._float;  return 0; }
        //operator float()  const  { if(_type == json::NUMBER) return _jsons._float;  return 0.0f;}
        //operator std::string() const   { if(_type == json::STRING) return *_jsons._string;  return "";}
        //operator bool() const  { if(_type == json::BOOL) return _jsons._bool;  return 0;}

        template<class T>
        operator T() const
        {
            return to<T>();
        }
#else
        template <class T>
        operator T() const;
#endif


        std::uint32_t                         _order;  // the order in the Object. In case the order of
                                                 // jsons in an object matter.
    public:
        union
        {
            float                            _float;
            bool                             _bool;
            unsigned long					 _long;
            std::vector<json>               *_array;
            std::map<std::string, json>     *_object;
            std::string                     *_string;
        }  _jsons;


        TYPE _type;


        static std::string                   parseString(std::istringstream & S );
        static bool                          parseBool(  std::istringstream & S );
        static float                         parseNumber(std::istringstream & S );
        static std::vector<json>             parseArray (std::istringstream & S );
        static std::string                   parseKey(   std::istringstream & S );
        static std::map<std::string, json>   parseObject(std::istringstream & S );
};

#ifndef _MSC_VER
template<>
inline json::operator int()  const    { if(_type == json::NUMBER) return (int)_jsons._float;  return 0; }

template<>
inline json::operator float()  const  { if(_type == json::NUMBER) return _jsons._float;  return 0.0f;}

template<>
inline json::operator std::string() const   { if(_type == json::STRING) return *_jsons._string;  return "";}

template<>
inline json::operator bool() const  { if(_type == json::BOOL) return _jsons._bool;  return 0;}
#endif



template<>
inline const std::string & json::as<std::string>()  const {
    if( _type != json::STRING) throw incorrect_type();
    return *_jsons._string;
}

template<>
inline const  bool & json::as<bool>()  const {
    if( _type != json::BOOL) throw incorrect_type();
    return _jsons._bool;
}

template<>
inline const  float & json::as<float>()  const  {
    if( _type != json::NUMBER) throw incorrect_type();
    return _jsons._float;
}

template<>
inline const  json & json::as<json>()  const  {
    return *this;
}


template<>
inline std::string json::to<std::string>()  const {
    if( _type != json::STRING ) return std::string();
    return *_jsons._string;
}

template<>
inline  bool  json::to<bool>()  const {
    if( _type != json::BOOL) return false;
    return _jsons._bool;
}

template<>
inline  float  json::to<float>()  const  {
    if( _type != json::NUMBER) return 0.0f;
    return _jsons._float;
}

template<>
inline  int  json::to<int>()  const  {
    if( _type != json::NUMBER) return 0;
    return static_cast<int>(_jsons._float);
}







inline const json & json::get(const char *i) const
{
    if( _type != json::OBJECT) throw incorrect_type();
  //  std::cout << "getting json: " << i << std::endl;
    return _jsons._object->at( std::string(i) );
}

inline const json & json::get(const std::string & i) const
{
    if( _type != json::OBJECT) throw incorrect_type();
  //  std::cout << "getting json: " << i << std::endl;
    return _jsons._object->at(i);
}

inline const json & json::get(              int   i) const
{
    if( _type != json::ARRAY) throw incorrect_type();
  //  std::cout << "getting index: " << i << "   size: " << _jsons._array->size() << std::endl;
    return _jsons._array->at(i);
}




}



#define REMOVEWHITESPACE { auto a = S.peek(); while( std::isspace(a) ) {S.get(); a = S.peek(); } }

namespace GNL_NAMESPACE
{



inline bool json::exists(const std::string & key) const
{
    if(_type != json::OBJECT) return false;

    return( (bool)( _jsons._object->count(key)>=1)  );
}

// json::json(const std::string & rhs)
// {
//     std::istringstream S( rhs );
//     parse( S );
// }

inline size_t json::size() const
{
    switch( _type )
    {
        case json::ARRAY:
            return _jsons._array->size();
        case json::OBJECT:
            return _jsons._object->size();
        case json::STRING:
            return _jsons._string->size();
        case json::UNKNOWN:
            return 0;
        case json::BOOL:
        case json::NUMBER:

        default:
            return 1;
    }

}


inline json & json::operator[](int i)
{

    if( _type != json::ARRAY)
    {
        Init(json::ARRAY);

    }

    if( (int)_jsons._array->size() <= i ) _jsons._array->resize(i+1);


    return (*_jsons._array)[i];
}

inline json & json::operator[](const std::string & i)
{
    if( _type != json::OBJECT)
    {
        Init(json::OBJECT);
    }

    //std::cout << "getting json: " << i << std::endl;
    return (*_jsons._object)[i];
}

inline json & json::operator[](const char i[])
{
    if( _type != json::OBJECT)
    {
        Init(json::OBJECT);
    }

    return (*_jsons._object)[i];
}




inline void json::parse(const std::string &S)
{
    std::istringstream SS(S);
    parse(SS);
}

inline void json::parse(std::istringstream &S )
{
    REMOVEWHITESPACE;

    char c = (char)S.peek();
    //::cout << c << std::endl;
    switch(c)
    {
        case  '"': // string
            Init(json::STRING);
            *_jsons._string =  json::parseString(S);
            break;

        case 't': // bool
        case 'f': // bool
            Init(json::BOOL);
            _jsons._bool = json::parseBool(S);
            break;
        case '{': // object
            Init( json::OBJECT );
            *_jsons._object = std::move( json::parseObject(S));//new Object();
            break;
        case '[': // array
            Init( json::ARRAY );
            *_jsons._array = std::move( json::parseArray(S) );
            break;
        default: // number

            if( std::isdigit(c) || c=='-' || c=='+' || c=='.')
            {
                Init(json::NUMBER);
                _jsons._float = json::parseNumber(S);
               // std::cout << "Number found: " << _jsons._float << std::endl;
            }
            break;
    }

}

inline bool json::parseBool(std::istringstream & S)
{
    REMOVEWHITESPACE;
    char c = (char)S.peek();
    if( std::tolower(c) == 't' )
    {
        while( !(c == ',' || c == '}' || c==']' || std::isspace(c)) )
        {
            c = (char)S.get();
        }
        return true;
    }
    while( !(c == ',' || c == '}' || c==']' || std::isspace(c)) )
    {
        c = (char)S.get();
    }
    return false;

}



inline float json::parseNumber(std::istringstream &S)
{
    REMOVEWHITESPACE;

    char c = (char)S.peek();

    std::string num;

    while( std::isdigit(c) || c=='-' || c=='+' || c=='.' || c=='e' || c=='E')
    {
        S.get();

        num += c;

        c = (char)S.peek();
    }
    float N = (float)std::atof(num.c_str() );
    //std::cout << "Number found: " << N << "\n";
    return( N );
}


inline std::vector<json>  json::parseArray(std::istringstream &S)
{
 //   std::cout << "Array found\n";
    REMOVEWHITESPACE;
    char c = (char)S.get();

    std::vector<json>  A;

    if( c == '[' )
    {
        while( c != ']' )
        {
            REMOVEWHITESPACE;

            json V;

            V.parse(S);

            // A.push_back( std::move(V) );
            A.push_back( V );

            REMOVEWHITESPACE;

            c = (char)S.get();
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


inline std::string json::parseString(std::istringstream & S)
{
    REMOVEWHITESPACE;

    char c = (char)S.get();

    // can be either:
    //'    usename   :  "gavin",  '
    //         or
    //'   "usename" s :  "gavin"   '

    std::string Key;

    if( c== '"')
    {

        c = (char)S.get();

        while( c != '"' )
        {
            if( c == '\\')
            {
                c = (char)S.get();
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
            c = (char)S.get();
        }

    }

    // std::cout << "String json Found: " << Key << "  next character (" << S.peek() << std::endl;
    return (Key);
}


inline std::string json::parseKey(std::istringstream & S)
{
    REMOVEWHITESPACE;

    char c = (char)S.get();

    // can be either:
    //'    usename   :  "gavin",  '
    //         or
    //'   "usename"  :  "gavin"   '
    std::string Key;

    if( c== '"')
    {
        c = (char)S.get();
        while( c != '"' )
        {
            Key += c;
            c = (char)S.get();
        }

    }
    else
    {
        Key += c;
        c    = (char)S.peek();
        while( !std::isspace(c) && c !=':')
        {
            Key += c;
            S.get();
            c    = (char)S.peek();
        }

    }

    return (Key);
}

inline std::map<std::string, json> json::parseObject(std::istringstream &S)
{

    REMOVEWHITESPACE;

    //Object * O = new Object();

    char c = (char)S.get();

    std::map<std::string, json> vMap;
    int count = 0;
    while(c != '}')
    {
        std::string key = json::parseKey(S);

        REMOVEWHITESPACE;

        c = (char)S.get();

        if(c != ':' )
        {
            //std::cout << "Key: " << key << std::endl;
            throw parse_error();
        }

        vMap[key].parse(S);
        vMap[key]._order = count;
        count++;

        REMOVEWHITESPACE;

         c = (char)S.peek();
         if(c == ',' || c =='}' ) c = (char)S.get();

    }

    return std::move(vMap);

}




}

inline std::ostream & __FormatOutput(std::ostream &os, const GNL_NAMESPACE::json & p, std::string & spaces)
{
    switch( p._type )
    {
        case GNL_NAMESPACE::json::UNKNOWN:   return os << "false";
        case GNL_NAMESPACE::json::NUMBER:    return os << p._jsons._float;
        case GNL_NAMESPACE::json::STRING:    return os << '"' << *p._jsons._string << '"' ;
        case GNL_NAMESPACE::json::ARRAY:
        {
            os << "[";
            int s=0;
            for(auto & a : *p._jsons._array)
            {
                if(a._type == GNL_NAMESPACE::json::OBJECT)
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
        case GNL_NAMESPACE::json::OBJECT:
        {
            size_t maxsize=0;

            for(auto & a : *p._jsons._object)
            {
                if( a.first.size() > maxsize ) maxsize = a.first.size();
            }

            os << std::endl << spaces << "{" << std::endl;
            spaces += "\t";
            int s = 0;
            for(auto & a : *p._jsons._object)
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
        case GNL_NAMESPACE::json::BOOL:
            return os << (p._jsons._bool ? "true" : "false");
        default:
            return os<<"false";
    }


}

inline std::ostream & operator << (std::ostream &os, GNL_NAMESPACE::json const & p)
{
    std::string spaces;
    __FormatOutput(os, p, spaces);
    return os;
}

#undef REMOVEWHITESPACE
#undef GNL_NAMESPACE
#endif

