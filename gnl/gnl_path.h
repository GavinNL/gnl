#ifndef GNL_PATH_H
#define GNL_PATH_H


#include <string>
#include <vector>
#include <iostream>


#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif

namespace GNL_NAMESPACE
{

    struct Path
    {
        public:
            enum class Style
            {
                UNIX,
                WINDOWS
            };

            Path(const std::string & path = "")
            {

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
                if( path[0] == '.')
                {
                    relative = true;
                }

            }

            Path(const Path & P) : isfolder(P.isfolder), relative(P.relative), dirs(P.dirs), filename(P.filename)
            {

            }


            std::string ToString( Style s=Style::UNIX ) const
            {
                std::string out;

                if(IsAbsolute() ) out += (s==Style::WINDOWS? '\\' : '/');
                for(auto & d : dirs)
                {
                    out += d +  (s==Style::WINDOWS? '\\' : '/');
                }
                return out + filename;
            }

            Path ParentPath() const
            {
                Path base = *this;

                base.filename="";

                base.dirs.pop_back();

                return base;

            }

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

            bool IsFolder()   const { return isfolder;  }
            bool IsFile()     const { return !isfolder; }
            bool IsAbsolute() const { return !relative; }

            std::string FileName() const { return filename; }

            std::string FileBaseName() const
            {
              //  std::cout << filename << std::endl;

                auto end = filename.find_last_of('.');

                return filename.substr( 0, end);

            }

            std::string FileExtension() const
            {
               // std::cout << filename << std::endl;

                auto end = filename.find_last_of('.');

                return filename.substr( end+1, filename.size()-end);
            }


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

    private:
            bool                       isfolder;   // does this point to a folder or a file?
            bool                       relative;

            std::vector<std::string>   dirs;
            std::string                filename;
            std::string                device;


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



    Path operator+(const Path & P1, const Path & P2)
    {
        Path P = P1;
        P += P2;
        return P;
    }

}

inline std::ostream & operator<<(std::ostream &os, const GNL_NAMESPACE::Path & p)
{
    os <<  p.ToString();
    return os;
}

#undef GNL_NAMESPACE

#endif

