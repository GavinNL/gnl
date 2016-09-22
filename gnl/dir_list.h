#ifndef _MSC_VER
    #include <dirent.h>
    #include <stdio.h>
#else
    #include<windows.h>
#endif

#include<string>
#include<iostream>

#ifndef _MSC_VER
std::vector<std::string> GetFiles( const std::string & path)
{
    DIR           *d;
    struct dirent *dir;

    d             = opendir( path.c_str() );

    std::vector<std::string> files;

    if( d )
    {
        while ( (dir = readdir(d)) != NULL)
        {
            //printf("%s\n", dir->d_name);

            if (dir->d_type != DT_DIR)
            {
                std::cout << "(File)";
            }
            std::cout << std::string(dir->d_name) << std::endl;
            files.push_back( std::string(dir->d_name) );
            //std::cout << "File found: " << dir->d_name << "    Flags: " << dir->d_ino       << std::endl;
        }

        closedir(d);
    }

  return files;
}

#else

std::vector<std::string> GetFiles(const std::string & path )
{
    using namespace std;
    std::vector<std::string> names;

    string search_path = path + "*.*";

    std::cout << "Searching: " << search_path << std::endl;

    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile( search_path.c_str(), &fd);

    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                std::cout << "(File)";
            }
            std::cout << fd.cFileName << std::endl;
            names.push_back(fd.cFileName);
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}
#endif
