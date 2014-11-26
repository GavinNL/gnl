/*  gnl_filesystem.h
 *
 *  Public License. Do whatever you want with it, just don't hold me responsible for anything.
 *
 *
 *  Implements simple filesystem operations and queries.
 */

#ifndef __GNL_FILESYSTEM__
#define __GNL_FILESYSTEM__

#include <vector>
#include <string>

namespace gnl {

    typedef std::string string;

    class Path
    {

        public:

            // constructs a path from a string.
            // set isFile=true if the filename does not have an extension
            Path(const gnl::string & path);
            Path(  );

            Path getFolderPath();               // gets the folder
            Path getParent();                   // gets the path to the parent folder

            gnl::string getFileExtension();     // gets the extension of the file (eg: .png)
            gnl::string getFileName();          // gets the file name (eg: displaypic.png)
            gnl::string getFileBasename();      // gets the file base name (eg: displaypic)

            gnl::string toString();

            bool        isFile()     { return  mIsFile;    }
            bool        isDirectory(){ return !mIsFile;    }
            bool        isRelative() { return !mIsAbsolute;}
            bool        isAbsolute() { return  mIsAbsolute;}

            bool        exists();

            void clear(); // clears the path
            void setDevice(const gnl::string & device );
            void setNode(  const gnl::string & device );

        private:
            void append(gnl::string & object);

            gnl::string              mNodeName;
            gnl::string              mDeviceName;
            gnl::string              mFileName;

            std::vector<gnl::string> mFolders;

            bool                     mIsAbsolute;
            bool                     mIsFile;

    };



};
#endif



#ifdef GNL_IMPLEMENTATION

gnl::Path::Path()
{
    mIsAbsolute = false;
    mIsFile     = false;
}

gnl::Path::Path( const gnl::string & path )
{
        mIsAbsolute = false;
        mIsFile = false;
        std::size_t start = 0;
        std::size_t end   = path.find_first_of("/\\");

        do {

            end = path.find_first_of("/\\", start + 1 );

            gnl::string F = path.substr(start, end-start);

            append( F );

           // std::cout << F << std::endl;
            start = end;

        } while ( end != gnl::string::npos );


}

void gnl::Path::setDevice(const gnl::string &device)
{
    mDeviceName = device;
    std::cout << "Setting device: " << mDeviceName << std::endl;
}

void gnl::Path::setNode(const gnl::string & node )
{
    mNodeName = node;
}


void gnl::Path::clear()
{
    mFolders.clear();
    mNodeName   = "";
    mDeviceName = "";
    mFileName   = "";
}

void gnl::Path::append(gnl::string & object)
{
    // std::cout << "Appending: " << object << std::endl;
    if(  object.size() )
    {

        if( mFolders.size() == 0)
        {
            if( object[object.size()-1]==':')
            {
                setDevice(  object.substr(0, object.size()-1)  );
                mIsAbsolute = true;
                return;
            }

            if( object[0]=='/' || object[0]=='\\')
            {
                mIsAbsolute = true;
            }
        }

        if( object[0]=='/' || object[0]=='\\')
        {

            mFolders.push_back( object.substr(1,gnl::string::npos) );
        }
        else
        {
            mFolders.push_back(object);
        }
    }

}

gnl::string gnl::Path::getFileExtension()
{
    if( mFolders.size() )
    {

        gnl::string & file = mFolders[ mFolders.size()-1 ];

        return file.substr( file.find_last_of("."), gnl::string::npos);
    }

    return std::string("");
}

gnl::string gnl::Path::getFileName()
{
    if( mFolders.size() )
    {
        gnl::string & file = mFolders[ mFolders.size()-1 ];
        return file;
    }
    return std::string("");
}

gnl::string gnl::Path::getFileBasename()
{
    if( mFolders.size() )
    {
        gnl::string & file = mFolders[ mFolders.size()-1 ];
        return file.substr( 0, file.find_last_of("."));
    }
    return std::string("");
}

gnl::Path gnl::Path::getParent()
{
    Path P;
    P.mDeviceName = mDeviceName;
    P.mNodeName   = mNodeName;
    P.mIsAbsolute = mIsAbsolute;
    P.mIsFile     = false;
    for(std::size_t i=0; i < mFolders.size()-1;i++ )
    {
        P.append( mFolders[i] );
    }
    return P;
}

gnl::string gnl::Path::toString()
{
    gnl::string S = mNodeName;

    if(mDeviceName.size()  )
    {
        S   += mDeviceName + ":";
        for(std::size_t i=0; i < mFolders.size();i++ )
        {
            S += "/" + mFolders[i];
        }
    } else {

        for(std::size_t i=0; i < mFolders.size();i++ )
        {
            if( mIsAbsolute || i>0) S += "/";
            S += mFolders[i];
        }
    }


    return S;
}

#endif
