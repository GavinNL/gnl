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


#ifndef GNL_PATH_H
#define GNL_PATH_H

#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <cstdio>

#ifndef _MSC_VER
    #include <dirent.h>
//    #include <stdio.h>
#else
    #include<windows.h>
#endif

#if defined(_WIN32)

#else
#include <sys/stat.h>
#include <sys/types.h>
#endif


#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif
namespace GNL_NAMESPACE
{
    class path
    {
        public:

#if defined __linux__
        using value_type	= char;
#define strlit(A) A
#elif defined _WIN32
        using value_type    = wchar_t;
#define strlit(A) L ## A
#endif
        using string_type = std::basic_string<value_type>;


            static const char separator = '/';

            path()
            {

            }

#if defined _WIN32
            path(const std::string & p) : path( string_type(p.begin(), p.end()))
            {

            }
            path(const char * s) : path( std::string(s) )
            {

            }
#endif


            explicit  path(const string_type & p)
            {
                string_type::size_type s = 0;

                do
                {
                    auto e = p.find_first_of( strlit("/\\"), s);


                    m_path_elements.push_back( p.substr(s , e==std::string::npos?e:(e-s+1) ) );

                    if( m_path_elements.back().size())
                        if(m_path_elements.back().back() == '\\')
                            m_path_elements.back().back() = '/';

                    s = e+1;
                }
                while( s != std::string::npos+1);
                if( m_path_elements.back() == strlit("") ) m_path_elements.pop_back();

                //m_path = p;
            }

            path(const value_type * x) : path( string_type(x) )
            {

            }

            path(const path & other) : m_path_elements(other.m_path_elements)
            {
            }

            path(path && other) : m_path_elements( std::move(other.m_path_elements) )
            {
            }

            path & operator=(const path & other)
            {
                if(&other != this)
                {
                    m_path_elements = other.m_path_elements;
                }
                return *this;
            }

            path & operator=( path && other)
            {
                if(&other != this)
                {
                    m_path_elements = std::move(other.m_path_elements);
                }
                return *this;
            }

            ~path()
            {

            }

            path filename() const
            {
                if( is_file() )
                {
                    return path(m_path_elements.back());
                }

                return path(strlit("."));

            }

            path root_name() const
            {
                if( m_path_elements.size()
                        &&

                    (( m_path_elements[0].size()   && m_path_elements[0][0]=='/' ) ||
                    ( m_path_elements[0].size()>1 && m_path_elements[0][1]==':' )))
                {
                    return path( m_path_elements[0].substr(0, m_path_elements[0].size()-1) );
                }
                else
                {
                    return path("");
                }


                return path(std::string(""));
            }

            path root_directory() const
            {
                return root_path();
            }
            path root_path() const // tested
            {
                if( is_relative() )
                {
                    return path("");
                } else {
                    return path(m_path_elements.front());
                }
            }

            path relative_path() const // tested
            {
                if( is_absolute() )
                {
                    path p;

                    for(auto e = m_path_elements.begin()+1;
                        e != m_path_elements.end();++e)
                        p.m_path_elements.push_back( *e );

                    return p;
                }
                return *this;

            }

            path parent_path() const
            {
                if( m_path_elements.size() )
                {
                    path ret;
                    ret.m_path_elements = m_path_elements;
                    ret.m_path_elements.pop_back();
                    return ret;
                }
                return path();
            }

            path stem() const // tested
            {
                if( size() > 0)
                {
                    auto p = m_path_elements.back().find_last_of('.');
                    return path(m_path_elements.back().substr(0, p));
                }
                return path("");
            }

            path extension() const // tested
            {
                if( is_file() )
                {

                    auto p = m_path_elements.back().find_last_of('.');
                    if( p == string_type::npos)
                        return path("");
                    return path(m_path_elements.back().substr(p));
                }

                return path("");
            }

            size_t size() const
            {
                return m_path_elements.size();
            }

            bool is_folder() const
            {
                if( m_path_elements.size())
                    return m_path_elements.back().back()==separator;
                return false;
            }

            bool is_file() const
            {
                if( m_path_elements.size())
                    return m_path_elements.back().back()!=separator;
                return false;
            }
            bool is_relative() const
            {
                return(!is_absolute());
            }

            bool is_absolute() const
            {
                if( m_path_elements.size() >= 1)
                {
                    // C:/  (windows based)
                    auto & first = m_path_elements.front();

                    switch( first.size() )
                    {
                        case 0:
                            return false;
                        case 1:
                            return true;
                        default:
                            if( first[1]==':') return true;
                            return false;
                    }
                }
                return false;
            }


            std::string to_stdstring() const
            {
                std::string out;
                out.reserve(100);
                for(auto & p : m_path_elements)
                    out += std::string(p.begin(), p.end());
                return out;
            }

            string_type to_string() const
            {
                string_type out;
                out.reserve(100);
                for(auto & p : m_path_elements)
                    out += p;
                return out;
            }

#if defined _WIN32
            bool operator==(const std::string & other) const
            {
                return to_string()==string_type(other.begin(), other.end());
            }
#endif

            bool operator==(const string_type & other) const
            {
                return to_string()==other;
            }

            void append(const path & other)
            {
                for(auto & e: other.m_path_elements)
                {
                    if( e[0]  == '/' || e[0] == '\\')
                        m_path_elements.push_back( e.substr(1, e.size()-1) );
                    else
                        m_path_elements.push_back( e );
                }
            }

            void concat(const path & other)
            {
                auto S = to_string() + other.to_string();
                *this = path(S);
            }

            path& operator /= (const string_type & other)
            {
                append( path(other) );
                return *this;
            }

            path& operator /= (const path & other)
            {
                append( other );
                return *this;
            }

            path operator / (const path & other) const
            {
                path p(*this);
                p.append(other);
                return p;
            }

            path& operator += (const string_type & other)
            {
                concat( path(other) );
                return *this;
            }

            path& operator += (const path& other)
            {
                concat( other );
                return *this;
            }

            path operator+(const path & other) const
            {
                path p(*this);
                p.concat(other);
                return p;
            }

            void clear()
            {
                m_path_elements.clear();
            }

            path & remove_filename()
            {
                if( is_file() )
                {
                    m_path_elements.pop_back();
                }
                return *this;
            }

            path & replace_filename(const path & filename)
            {
                remove_filename();
                append(filename);
                return *this;
            }

            path & replace_extension(const path & ext)
            {
                if( is_file() )
                {
                    auto S        = stem().to_string();
                    string_type e = ext.to_string();
                    if( e[0] != '.')
                        S += '.';
                    S += e;

                    remove_filename();

                    append( gnl::path(S) );
                }
                return *this;
            }


            void swap( path & other)
            {
                std::swap(m_path_elements, other.m_path_elements);
            }


            static std::FILE* fopen(const path & P, const char * open_flags, bool create_dirs = false)
            {

               if( create_dirs )
               {
                if( P.is_file())
                    mkdir(P.parent_path());
               }

    #ifdef _WIN32
              FILE * F;
              string_type wstr = P.to_string();
              std::string p_str(wstr.begin(), wstr.end());
              ::fopen_s(&F, p_str.c_str(), open_flags );
//              F = std::fopen(  P.to_string().c_str(), open_flags.c_str() );
              return F;
    #else
              return std::fopen(P.to_string().c_str(), open_flags );
    #endif
            }



            static inline bool mkdir(const path & P, std::uint32_t chmod=0766)
            {
                if( P.empty() ) return false;

                if( !__mkdir( P.to_string(), chmod ) )
                {
                     mkdir( P.parent_path(), chmod);
                     return __mkdir( P.to_string(), chmod);
                }
                return true;

            }

            bool empty() const
            {
                return m_path_elements.size()==0;
            }




            /**
            * @brief Home
            * @return  The home path of the current user
            */
           static path home_dir()
           {
   #ifdef _WIN32
               auto p = get_env_var("USERPROFILE");
   #else
               auto p = get_env_var("HOME");
   #endif
               return path( p + strlit("/") );
           }

           /**
            * @brief Temp
            * @return A path used for temporary files.
            */
           static path temp_dir()
           {
   #ifdef _WIN32
               auto p = get_env_var("TMP");
               return path(p + strlit("/"));
   #else
               return path( "/tmp/");
   #endif
           }


           std::string string() const
           {
               auto s = to_string();
               return std::string(s.begin(), s.end());
           }

           std::wstring wstring() const
           {
               auto s = to_string();
               return std::wstring(s.begin(), s.end());
           }

           std::string u8string() const
           {
               auto s = to_string();
               return std::string(s.begin(), s.end());
           }

           std::u16string u16string() const
           {
               auto s = to_string();
               return std::u16string(s.begin(), s.end());
           }
           std::u32string u32string() const
           {
               auto s = to_string();
               return std::u32string(s.begin(), s.end());
           }

        private:
            std::vector<string_type>   m_path_elements;



            static bool __mkdir(string_type const & p, std::int32_t chmod=0766)
            {
#if defined(_WIN32)
                std::string s(p.begin(), p.end());
                return CreateDirectory ( s.c_str(), NULL) != 0;
#else
              auto success = (::mkdir(p.c_str(), chmod) == 0);
              if( success )
              {
              } else {
              }
              return success;
#endif

            }

            static string_type get_env_var(const std::string & var_name)
            {
   #ifdef _WIN32
                char * buf = nullptr;
                size_t sz  = 0;
                std::string var(var_name.begin(), var_name.end() );
                if (_dupenv_s(&buf, &sz, var.c_str()) == 0 && buf != nullptr)
                {
                    std::string s( buf );
                    free(buf);
                    return string_type(s.begin(), s.end());
                }
   #else
                auto * p = std::getenv(var_name.c_str());
                return string_type(p);
   #endif
                return string_type();

            }
    };

}

inline std::ostream & operator<<(std::ostream &os, const gnl::path & p)
{
    os <<  p.string();
    return os;
}

#if 0
inline std::ostream & operator<<(std::ostream &os, const gnl::Path & p)
{
    os <<  p.ToString();
    return os;
}
#endif

//inline bool operator==( const gnl::path & lhs, const gnl::path & rhs )
//{
//}

//inline bool operator!=( const gnl::path& lhs, const gnl::path & rhs );

//inline bool operator<( const gnl::path& lhs, const gnl::path& rhs )
//{
//}

//inline bool operator<=( const gnl::path& lhs, const gnl::path& rhs );

//inline bool operator>( const gnl::path& lhs, const gnl::path& rhs );

//inline bool operator>=( const gnl::path& lhs, const gnl::path& rhs );


#endif


