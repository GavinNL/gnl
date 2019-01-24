/*************************************************************************
 * gnl_resource_path - www.glfw.org
 *
 * Similar to how the OS PATH works, simple add a bunch of folders
 * to the object, and then use the .get( ) method to find the first
 * occurance of the file/folder you have requested.
 *
 * Example:
 *
 *   gnl::gnl_resource_path R;
 *   R.add_path("/bin");
 *   R.add_path("/usr/bin");
 *
 *   R.get("ls") // returns "/bin/ls"
 *
 *------------------------------------------------------------------------
 * UNLICENSE
 *
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
 * For more information, please refer to <http://unlicense.org/>
 *************************************************************************/

#pragma once
#ifndef GNL_RESOURCE_PATH
#define GNL_RESOURCE_PATH


#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <exception>
#include <stdexcept>
#include <stdio.h>

#if defined __linux__
#include <unistd.h>
#include<sys/stat.h>
#endif

#if defined _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <stdlib.h>

#endif

namespace gnl
{

/**
 * @brief The gnl_resource_path class
 *
 * The resource_path class is used search for external resources which may reside in multiple
 * locations on the file system.
 *
 * gnl_resource_path R;
 *
 * R.add_resource( "/path/to/images" );
 * R.add_resource( "/path/to/other_images" );
 *
 * R.get("myimge.jpg"); // will return /path/to/images/myimage.jpg or /path/to/other_images/myimage.jpn
 *                      // depending on which one exists. If both exist, the first one will be returned
 *                      // due to the order it was added in the resource list.
 *
 */
class gnl_resource_path
{
    public:
        using path_type = std::string;

        static constexpr path_type::value_type separator = '/';

        /**
         * @brief add_path
         * @param path_
         * @param position
         * @return
         *
         * Add a path to the list of resources paths. Returns false if the path
         * already exists in the list.
         */
        bool add_path(path_type const & path_, std::size_t position = std::numeric_limits<std::size_t>::max())
        {
            auto path = format_path(path_);
            if( path_exists(path) ) return false;

            position = std::min( position, m_paths.size());
            m_paths.insert( std::begin(m_paths) + position, path);

            return true;
        }

        /**
         * @brief remove_path
         * @param path_
         *
         * Removes a path from the resource list.
         */
        void remove_path(path_type const & path_)
        {
            auto path = format_path(path_);
            m_paths.erase(std::remove(std::begin(m_paths), std::end(m_paths), path),
                           std::end(m_paths));
        }

        /**
         * @brief path_exists
         * @param path_
         * @return
         *
         * Returns true if the provided path exists in the
         * list of resource paths
         */
        bool path_exists(path_type const & path_) const
        {
            auto path = format_path(path_);
            return std::end(m_paths) != std::find( std::begin(m_paths), std::end(m_paths), path);
        }

        /**
          * @brief get
          * @param stem
          * @return
          *
          *  Given a filename, return the first path found
          *  in the resource lists that matches the stem.
          *  If the file does not exist, it will return an empty string.
          */
         path_type get(path_type const & stem) const
         {
             auto stm = format_path(stem);
             for(auto const & root : m_paths)
             {
                 auto p = join(root, stm);
                 if( fexists(p) )
                 {
                     return p;
                 }
             }
             return "";
         }


        /**
         * @brief get_exe_path
         * @return
         *
         * Gets the absolute path of the current executable.
         */
        static path_type get_exe_path()
        {
            char path[FILENAME_MAX];

#if defined __linux__
            auto i = readlink("/proc/self/exe", path, FILENAME_MAX );
            path[i] = 0;
            return path_type(path);
#elif defined _WIN32
            auto i = GetModuleFileNameA( NULL, path, FILENAME_MAX);
            auto p = path_type( path );
            return join( get_cwd(), path);
#endif
        }

        /**
         * @brief get_cwd
         * @return
         *
         * Gets the current working directory of the executable
         */
        static path_type get_cwd()
        {
#if defined __linux__
            char path[FILENAME_MAX];
            getcwd(path, FILENAME_MAX);

            return path_type(path);
#elif defined _WIN32
            char * cwd;
            if( (cwd = _getcwd( NULL, 0 )) != NULL )
            {
                return path_type(cwd);
            }
            return "";
#endif
        }


        /**
         * @brief join
         * @param root_folder
         * @param stem
         * @return
         *
         * Joins two paths together to form a single path. The stem must be
         * a relative path.
         */
        static path_type join(path_type const & root_folder, path_type const & stem)
        {
            if( is_absolute(stem) )
            {
                throw std::runtime_error("The stem path must be relative");
            }
            path_type N = format_path(root_folder + '/') + format_path(stem);
            __erase_duplicate_separators(N);
            return N;
        }

        /**
         * @brief is_absolute
         * @param path
         * @return
         *
         * Returns true if a path is an absolute path.
         * A path is an absolute path if it begins with '/'
         * or if its second character is : (windows based)
         */
        static bool is_absolute(path_type const & path)
        {
            if( path.size() >= 1 && path[0] == '/')
            {
                return true;
            }
            if( path.size() >= 2 && path[1] == ':') // windows
            {
                return true;
            }

            return false;;
        }

        /**
         * @brief format_path
         * @param path
         * @return
         *
         * Format a path to to have standard unix separators '/' since
         * they work on both windows and unixes.
         */
        static path_type format_path(path_type const & path)
        {
            path_type p = path;

            // replace any \ with /
            std::replace(p.begin(), p.end(), '\\', '/');

            __erase_duplicate_separators(p);
            return p;
        }



        static bool is_dir(path_type const & path_)
        {
            auto path = format_path(path_);
            struct stat st;
            if( stat(path.c_str(), &st) != 0)
            {
                return false;
            }
            bool isdir = S_ISDIR(st.st_mode);
            return isdir;
        }

        /**
         * @brief fexists
         * @param path
         * @return
         *
         * Return true if the path exists on the file system. The path
         * exists if it is a directory/regular file/symlink/block device
         */
        static bool fexists(path_type const & path)
        {
#if defined __linux__
            struct stat buffer;
            return (stat(path.c_str(), &buffer) == 0);
#elif defined _WIN32
            int fileExists(TCHAR * file)
            {
               WIN32_FIND_DATA FindFileData;
               HANDLE handle = FindFirstFile(file, &FindFileData) ;
               int found = handle != INVALID_HANDLE_VALUE;
               if(found)
               {
                   //FindClose(&handle); this will crash
                   FindClose(handle);
               }
               return found;
            }
#endif

        }


        /**
         * @brief __erase_duplicate_separators
         * @param path
         *
         * Erases any duplicate separators in the string.
         */
        static void __erase_duplicate_separators(path_type & path)
        {
                auto it =
                std::unique( std::begin(path),
                             std::end(path),
                             [](path_type::value_type & a, path_type::value_type & b)
                             {
                                return a==separator && a == b;
                             });
                path.resize( std::distance(path.begin(),it) );
        }

    public:
        std::vector<path_type> m_paths;
};



}

#endif
