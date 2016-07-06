#ifndef GNL_BASE64_H_
#define GNL_BASE64_H_

// temprorary implementation by someone else

#include <string>

#ifndef GNL_NAMESPACE
    #define GNL_NAMESPACE gnl
#endif

namespace GNL_NAMESPACE
{

struct Base64
{
    template<typename chartype>
    static std::string Encode(const std::basic_string<chartype> &in) {

        std::string out;

        int val=0, valb=-6;
        for (char c : in) {
            val = (val<<8) + c;
            valb += 8;
            while (valb>=0) {
                out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
                valb-=6;
            }
        }

        if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
        while (out.size()%4) out.push_back('=');
        return out;
    }

    template<typename chartype=char>
    static std::basic_string<chartype> Decode(const std::string &in) {

        std::basic_string<chartype> out;
        static bool init=false;
        static std::vector<int> T(256,-1);
        if(!init)
        {
            init = true;
            for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
        }


        int val=0, valb=-8;
        for (unsigned char c : in)
        {
            if (T[c] == -1) break;
            val = (val<<6) + T[c];
            valb += 6;
            if (valb>=0) {
                out.push_back( static_cast<chartype>( (val>>valb)&0xFF) );
                valb-=8;
            }
        }
        return out;
    }

    template<typename chartype=char>
    static std::basic_string<chartype> Decode(const char * in, int size) {

        std::basic_string<chartype> out;
        static bool init=false;
        static std::vector<int> T(256,-1);
        if(!init)
        {
            init = true;
            for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
        }


        int val=0, valb=-8;

        //for (unsigned char c : in)
        for(int i=0;i<size;i++)
        {
            unsigned char c = in[i];
            if (T[c] == -1) break;
            val = (val<<6) + T[c];
            valb += 6;
            if (valb>=0) {
                out.push_back( static_cast<chartype>( (val>>valb)&0xFF) );
                valb-=8;
            }
        }
        return out;
    }
};

}

#endif
