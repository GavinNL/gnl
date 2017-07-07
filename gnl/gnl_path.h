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


namespace gnl
{
#if 0
    class Path;

    Path operator+(const Path & P1, const Path & P2);

    class Path
    {
        public:
            enum Style
            {
                UNIX_STYLE,
                WINDOWS_STYLE
            };

            /**
             * @brief Path
             * Construct a blank path
             */
            Path() : Path(std::string("/")) { }

            /**
             * @brief Path
             * @param path
             * Construct a path from a raw string
             */
            Path(const char * path) : Path( std::string(path) )
            {

            }

            /**
             * @brief Path
             * @param path - path name as a string.
             *
             * Construct a path from a std::string
             */
            Path(const std::string & path)
            {

                if(path=="")
                {
                  isfolder = false;
                  relative = true;
                  return;
                }

                dirs = Tokenize(path, "/\\");


                if( path[ path.size()-1] == '/' || path[ path.size()-1] == '\\')
                {
                    isfolder = true;
                } else {
                    isfolder = false;
                    filename = dirs[ dirs.size()-1];
                    dirs.pop_back();
                }


                if(path[0] == '/')
                {
                    relative = false;
                }
                if( path[0] != '/')
                {
                    relative = true;
                }

                if( std::isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\') )
                {
                    device   = path[0];
                    relative = false;
                    dirs.erase( dirs.begin(), dirs.begin()+1);
                }

            }

            Path(const Path & P) : isfolder(P.isfolder), relative(P.relative), device(P.device), filename(P.filename), dirs(P.dirs)
            {

            }

            Path & operator=(const Path & P)
            {
              if( this == &P ) return *this;

              isfolder = P.isfolder;
              relative = P.relative;
              device   = P.device;
              filename = P.filename;
              dirs     = P.dirs;

              return *this;

            }

            /**
             * @brief ToString
             * @param s - the string style for teh path (forward slashes to separate directories on UNIX)
             * @return A string representation of the path
             *
             */
            std::string ToString( Style s = UNIX_STYLE ) const
            {
                std::string out;

                if(IsAbsolute() )
                {
                    if( device.length() )
                    {
                        out += device + ":" + (s==WINDOWS_STYLE? '\\' : '/');
                    }
                    else
                    {
                        out += (s==Style::WINDOWS_STYLE? '\\' : '/');
                    }
                }


                for(auto & d : dirs)
                {
                    out += d +  (s==Style::WINDOWS_STYLE? '\\' : '/');
                }
                return out + filename;
            }


            /**
             * @brief operator std::string
             * Casting operator to string.
             */
            inline operator std::string()  const
            {
                return ToString();
            }


            /**
             * @brief ParentPath
             * @return The parent folder of the current directory. If the path is a file, it will give the
             * parent directry of the base path
             *
             */
            Path ParentPath() const
            {
                Path base = *this;

                base.filename="";

                base.dirs.pop_back();

                return base;

            }

            /**
             * @brief BasePath
             * @return The base path of the file. If the path is a folder, it will return the path to the same folder
             */
            Path BasePath() const
            {
                Path base = *this;

                if( base.IsFile() )
                {
                    base.isfolder = true;
                    base.filename = "";
                }

                return base;
            }




            void Report() const
            {
                std::cout << "-----------------------------------" << std::endl;
                std::cout << "Is Folder: "   << isfolder << std::endl;
                std::cout << "Is Relative: " << relative << std::endl;
                std::cout << "Directories: " <<  std::endl;

                for(auto d : dirs)
                {
                    std::cout <<  "    " << d << std::endl;
                }

                std::cout << "filename: " << filename << std::endl;

                std::cout << ToString() << std::endl;

            }


            /**
             * @brief IsFolder
             * @return True if the path represents a folder.
             */
            bool IsFolder()   const { return isfolder;  }

            /**
             * @brief IsFile
             * @return True if the path represents a file
             */
            bool IsFile()     const { return !isfolder; }

            /**
             * @brief IsAbsolute
             * @return true if the path is an abolute path
             */
            bool IsAbsolute() const { return !relative; }

            /**
             * @brief IsRelative
             * @return True if teh path is a relative path
             */
            bool IsRelative() const { return relative;  }

            /**
             * @brief FileName
             * @return Returns the filename as a string path.
             */
            std::string FileName() const { return filename; }

            /**
             * @brief Device
             * @return Returns the device of the path (windows only), the device is the drive letter, eg C:
             */
            std::string Device()   const { return device;   }

            /**
             * @brief FileBaseName
             * @return Returns the file's basename, the basepath is the name of the file, without the extension
             */
            std::string FileBaseName() const
            {
                auto end = filename.find_last_of('.');

                return filename.substr( 0, end);
            }

            /**
             * @brief FileExtension
             * @return The file's extention. The extension is the characters after the last . in the file name
             */
            std::string FileExtension() const
            {
               // std::cout << filename << std::endl;

                auto end = filename.find_last_of('.');

                return filename.substr( end+1, filename.size()-end);
            }


            /**
             * @brief operator +=
             * @param P - relative path to add to the current path
             * @return
             */
            Path & operator+=(const Path & P)
            {
                if( !IsFile() )
                {
                    if( !P.IsAbsolute() )
                    {
                        for(auto & d : P.dirs )
                        {
                            if( d == "..")
                            {
                                dirs.pop_back();
                            } else {
                                dirs.push_back( d );
                            }
                        }
                        filename = P.filename;
                        isfolder = P.isfolder;
                    } else {
                        throw std::runtime_error("Second Path operand must not be an absolute path");
                    }
                }
                else {
                    throw std::runtime_error("First Path operand must not be a file");
                }

                return *this;
            }

            bool operator == ( const Path & P)
            {
                return
                        isfolder == P.isfolder &&
                        relative == P.relative &&
                        device   == P.device &&
                        std::equal( dirs.begin(), dirs.end(), P.dirs.begin() );
            }
            bool operator != ( const Path & P)
            {
                return !(*this == P);
            }

            #define OPERATOR(op)                      \
            bool operator op (const Path & P)         \
            {                                         \
                return ToString() op P.ToString();    \
            }

            OPERATOR(<)
            OPERATOR(>)
            OPERATOR(<=)
            OPERATOR(>=)

            #undef OPERATOR
            /**
             * @brief GetDirectoryList
             * @param P
             * @return
             *
             * Returns a vector of files in a path
             */
            static std::vector<Path> GetFileList( const gnl::Path & dir_path)
            {
                std::vector<Path> files;

#ifndef _MSC_VER

                DIR           *d;
                struct dirent *dir;

                d   = opendir(  dir_path.BasePath().ToString(UNIX_STYLE).c_str() );

                if( d )
                {
                    while ( (dir = readdir(d)) != NULL)
                    {
                        //printf("%s\n", dir->d_name);

                       // std::cout << std::string(dir->d_name) << std::endl;

                        if (dir->d_type != DT_DIR)
                        {
                            //std::cout << "(File)";
                            files.push_back( dir_path.BasePath() + Path(std::string(dir->d_name) ) );
                        } else {
                            files.push_back( dir_path.BasePath() + Path(std::string(dir->d_name)+std::string("/") ) );
                        }

                        //std::cout << "File found: " << dir->d_name << "    Flags: " << dir->d_ino       << std::endl;
                    }

                    closedir(d);
                }

              return files;
#else
                using namespace std;

               // struct dirent *dir;
                string search_path = dir_path.BasePath().ToString(UNIX_STYLE) + "*.*";

                //std::cout << "Searching: " << search_path << std::endl;

                WIN32_FIND_DATA fd;
                HANDLE hFind = ::FindFirstFile( search_path.c_str(), &fd);

                if(hFind != INVALID_HANDLE_VALUE) {
                    do {
                        // read all (real) files in current folder
                        // , delete '!' read other 2 default folder . and ..
//                        std::cout << std::string(fd.cFileName) << std::endl;
                        if( std::string(fd.cFileName) != std::string("..") && std::string(fd.cFileName) != std::string("."))
                        {
                            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                            {
                                    files.push_back( dir_path.BasePath() + Path(fd.cFileName) );
                            } else {
                                files.push_back( dir_path.BasePath() + Path(std::string(fd.cFileName)+std::string("/") ) );
                            }
                        }
                        //std::cout << fd.cFileName << std::endl;
                        //names.push_back(fd.cFileName);
                    }while(::FindNextFile(hFind, &fd));
                    ::FindClose(hFind);
                }
                return files;

#endif
            }


         static std::string get_env_var(const std::string & var_name)
         {
#ifdef _WIN32
             char * buf = nullptr;
             size_t sz = 0;
             if (_dupenv_s(&buf, &sz, var_name.c_str()) == 0 && buf != nullptr)
             {
                 std::string s(buf);
                 free(buf);
                 return s;
             }
#else
             auto * p = std::getenv(var_name.c_str());
             return std::string(p);
#endif
             return std::string();

         }

         /**
         * @brief Home
         * @return  The home path of the current user
         */
        static Path Home()
        {
#ifdef _WIN32
            auto p = get_env_var("USERPROFILE");
#else
            auto p = get_env_var("HOME");
#endif
            return Path( p + "/");
        }

        /**
         * @brief Temp
         * @return A path used for temporary files.
         */
        static Path Temp()
        {
#ifdef _WIN32
            auto p = get_env_var("TMP");
            return Path(p + "/");
#else
            return Path( "/tmp/");
#endif
        }

        static std::FILE* fopen(const Path & P, const std::string & open_flags)
        {
          mkdir( P.BasePath() );
#ifdef _WIN32
          FILE * F;
          ::fopen_s(&F, P.ToString().c_str(), open_flags.c_str() );
          return F;
#else
          return std::fopen(P.ToString().c_str(), open_flags.c_str() );
#endif
        }

        static inline bool mkdir(const Path & P, std::uint32_t chmod=0766)
        {
          if( P.IsRelative() )
            throw std::runtime_error("Path must be absolute, not relative");

          gnl::Path p("/");

          bool success = false;

          for( auto & d : P.dirs)
          {
            p = p.ToString() + std::string("/") + d;
          #if defined(_WIN32)
          #else

                success = (::mkdir(p.ToString().c_str(), chmod) == 0);
                if( success )
                {
               //   std::cout << "Directory Created: " << p.ToString() << std::endl;
                } else {
               //   std::cout << "Error Creating Directory: " << p.ToString() << std::endl;
                }
          #endif
          }

          return success;
        }


        private:
            bool                       isfolder;   // does this point to a folder or a file?
            bool                       relative;

            std::string                device;
            std::string                filename;
            std::vector<std::string>   dirs;


            std::vector<std::string> Tokenize(const std::string& str,

                                  const std::string& delimiters = " ")
            {
                std::vector<std::string> tokens;

                auto lastPos = str.find_first_not_of(delimiters, 0);

                auto pos     = str.find_first_of(delimiters, lastPos);

                while (std::string::npos != pos || std::string::npos != lastPos)
                {
                    // Found a token, add it to the vector.
                    tokens.push_back(str.substr(lastPos, pos - lastPos));
                    // Skip delimiters.  Note the "not_of"
                    lastPos = str.find_first_not_of(delimiters, pos);
                    // Find next "non-delimiter"
                    pos = str.find_first_of(delimiters, lastPos);
                }

                return tokens;
            }
    };


    inline Path operator+(const Path & P1, const Path & P2)
    {
        Path P = P1;
        P += P2;
        return P;
    }

#endif


    class path2
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

            path2()
            {

            }

#if defined _WIN32
            path2(const std::string & p) : path2( string_type(p.begin(), p.end()))
            {

            }
            path2(const char * s) : path2( std::string(s) )
            {

            }
#endif


            explicit  path2(const string_type & p)
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

            path2(const value_type * x) : path2( string_type(x) )
            {

            }

            path2(const path2 & other) : m_path_elements(other.m_path_elements)
            {
            }

            path2(path2 && other) : m_path_elements( std::move(other.m_path_elements) )
            {
            }

            path2 & operator=(const path2 & other)
            {
                if(&other != this)
                {
                    m_path_elements = other.m_path_elements;
                }
                return *this;
            }

            path2 & operator=( path2 && other)
            {
                if(&other != this)
                {
                    m_path_elements = std::move(other.m_path_elements);
                }
                return *this;
            }

            ~path2()
            {

            }

            path2 filename() const
            {
                if( is_file() )
                {
                    return path2(m_path_elements.back());
                }

                return path2(strlit("."));

            }

            path2 root_name() const
            {
                if( m_path_elements.size()
                        &&

                    (( m_path_elements[0].size()   && m_path_elements[0][0]=='/' ) ||
                    ( m_path_elements[0].size()>1 && m_path_elements[0][1]==':' )))
                {
                    return path2( m_path_elements[0].substr(0, m_path_elements[0].size()-1) );
                }
                else
                {
                    return path2("");
                }


                return path2(std::string(""));
            }

            path2 root_directory() const
            {
                return root_path();
            }
            path2 root_path() const // tested
            {
                if( is_relative() )
                {
                    return path2("");
                } else {
                    return path2(m_path_elements.front());
                }
            }

            path2 relative_path() const // tested
            {
                if( is_absolute() )
                {
                    path2 p;

                    for(auto e = m_path_elements.begin()+1;
                        e != m_path_elements.end();++e)
                        p.m_path_elements.push_back( *e );

                    return p;
                }
                return *this;

            }

            path2 parent_path() const
            {
                if( m_path_elements.size() )
                {
                    path2 ret;
                    ret.m_path_elements = m_path_elements;
                    ret.m_path_elements.pop_back();
                    return ret;
                }
                return path2();
            }

            path2 stem() const // tested
            {
                if( size() > 0)
                {
                    auto p = m_path_elements.back().find_last_of('.');
                    return path2(m_path_elements.back().substr(0, p));
                }
                return path2("");
            }

            path2 extension() const // tested
            {
                if( is_file() )
                {

                    auto p = m_path_elements.back().find_last_of('.');
                    if( p == string_type::npos)
                        return path2("");
                    return path2(m_path_elements.back().substr(p));
                }

                return path2("");
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

            void append(const path2 & other)
            {
                for(auto & e: other.m_path_elements)
                {
                    if( e[0]  == '/' || e[0] == '\\')
                        m_path_elements.push_back( e.substr(1, e.size()-1) );
                    else
                        m_path_elements.push_back( e );
                }
            }

            void concat(const path2 & other)
            {
                auto S = to_string() + other.to_string();
                *this = path2(S);
            }

//            path2 operator / (const std::string & other) const
//            {
//                path2 p(*this);
//                p.append( path2(other) );
//                return p;
//            }

            path2 operator / (const path2 & other) const
            {
                path2 p(*this);
                p.append(other);
                return p;
            }

            path2 operator + (const path2 & other) const
            {
                path2 p(*this);
                p.concat(other);
                return p;
            }

            void clear()
            {
                m_path_elements.clear();
            }

            path2 & remove_filename()
            {
                if( is_file() )
                {
                    m_path_elements.pop_back();
                }
                return *this;
            }

            path2 & replace_filename(const path2 & filename)
            {
                remove_filename();
                append(filename);
                return *this;
            }

            path2 & replace_extension(const path2 & ext)
            {
                if( is_file() )
                {
                    auto S        = stem().to_string();
                    string_type e = ext.to_string();
                    if( e[0] != '.')
                        S += '.';
                    S += e;

                    remove_filename();

                    append( gnl::path2(S) );
                }
                return *this;
            }


            void swap( path2 & other)
            {
                std::swap(m_path_elements, other.m_path_elements);
            }


            static std::FILE* fopen(const path2 & P, const char * open_flags, bool create_dirs = false)
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



            static inline bool mkdir(const path2 & P, std::uint32_t chmod=0766)
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
           static path2 home_dir()
           {
   #ifdef _WIN32
               auto p = get_env_var("USERPROFILE");
   #else
               auto p = get_env_var("HOME");
   #endif
               return path2( p + strlit("/") );
           }

           /**
            * @brief Temp
            * @return A path used for temporary files.
            */
           static path2 temp_dir()
           {
   #ifdef _WIN32
               auto p = get_env_var("TMP");
               return path2(p + strlit("/"));
   #else
               return path2( "/tmp/");
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

    using path = gnl::path2;

}

inline std::ostream & operator<<(std::ostream &os, const gnl::path2 & p)
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

//inline bool operator==( const gnl::path2 & lhs, const gnl::path2 & rhs )
//{
//}

//inline bool operator!=( const gnl::path2& lhs, const gnl::path2 & rhs );

//inline bool operator<( const gnl::path2& lhs, const gnl::path2& rhs )
//{
//}

//inline bool operator<=( const gnl::path2& lhs, const gnl::path2& rhs );

//inline bool operator>( const gnl::path2& lhs, const gnl::path2& rhs );

//inline bool operator>=( const gnl::path2& lhs, const gnl::path2& rhs );


#endif


