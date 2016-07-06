#ifndef GNL_PATH_H
#define GNL_PATH_H


#include <string>
#include <vector>
#include <iostream>

namespace gnl
{

    struct Path
    {
        public:
            Path(const std::string & path = "")
            {
                if( path == "" ) return;

                auto last = path.find_last_of("/\\");

                dirs = split( path, '/');

                isfolder = dirs[ dirs.size()-1 ] == "";
                if(!isfolder)
                {
                    filename = dirs[ dirs.size()-1];
                }
                dirs.pop_back();



                relative = dirs[0]!="";
                if(!relative)
                {
                    dirs.erase( dirs.begin(), dirs.begin()+1);
                }

            }

            Path(const Path & P) : isfolder(P.isfolder), relative(P.relative), dirs(P.dirs), filename(P.filename)
            {

            }


            std::string ToString() const
            {
                return BasePath() + filename;
            }

            std::string BasePath() const
            {
                std::string path;
                if(IsAbsolute())
                {
                    path += "/";
                }
                for(auto & d : dirs)
                {
                    path += d ;
                    path += "/";
                }
                return path;
            }

            std::vector<std::string> split(const std::string &text, char sep) {
              std::vector<std::string> tokens;
              std::size_t start = 0, end = 0;
              while ((end = text.find(sep, start)) != std::string::npos) {
                tokens.push_back(text.substr(start, end - start));
                start = end + 1;
              }
              tokens.push_back(text.substr(start));
              return tokens;
            }

            void Report()
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

    private:
            bool                       isfolder;   // does this point to a folder or a file?
            bool                       relative;
            std::vector<std::string>   dirs;
            std::string                filename;
    };

}

#endif

