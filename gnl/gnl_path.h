/*
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/


#ifndef GNL_PATH_H
#define GNL_PATH_H

#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <cstdlib>


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

         /**
         * @brief Home
         * @return  The home path of the current user
         */
        static Path Home()
        {
#ifdef _WIN32
            auto * p = std::getenv("USERPROFILE");
#else
            auto * p = std::getenv("HOME");
#endif
            return Path( std::string(p) + "/");
        }

        /**
         * @brief Temp
         * @return A path used for temporary files.
         */
        static Path Temp()
        {
#ifdef _WIN32
            auto * p = std::getenv("TMP");
            return Path( std::string(p) + "/");
#else
            return Path( "/tmp/");
#endif
        }

        static FILE* fopen(const Path & P, const std::string & open_flags)
        {
          mkdir( P.BasePath() );
          return ::fopen( P.ToString().c_str(), open_flags.c_str() );
        }

        static inline bool mkdir(const Path & P, uint32_t chmod=0766)
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

}

inline std::ostream & operator<<(std::ostream &os, const gnl::Path & p)
{
    os <<  p.ToString();
    return os;
}



#endif

