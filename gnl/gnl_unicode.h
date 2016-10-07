#ifndef GNL_UNICODE
#define GNL_UNICODE


#include <string>
#include <locale>

namespace gnl
{
    namespace Unicode
    {
        static int sequence_length(char32_t lead_it)
        {
            uint8_t lead = 0xff & lead_it; //utf8::internal::mask8(*lead_it);
            if (lead < 0x80)
                return 1;
            else if ((lead >> 5) == 0x6)
                return 2;
            else if ((lead >> 4) == 0xe)
                return 3;
            else if ((lead >> 3) == 0x1e)
                return 4;
            else
                return 0;
        }

        static std::u32string UTF32(const std::string & utf8)
        {
            std::u32string out;

            for(auto it = utf8.begin(); it != utf8.end(); )
            {
                uint32_t cp = 0xff & (*it);//utf8::internal::mask8(*it);

                auto length = sequence_length(*it);
                std::cout << "length: " << length << std::endl;
                switch (length) {
                    case 1:
                        break;
                    case 2:
                        it++;
                        cp = ((cp << 6) & 0x7ff) + ((*it) & 0x3f);
                        break;
                    case 3:
                        ++it;
                        cp = ((cp << 12) & 0xffff) + (((0xff & (*it)) << 6) & 0xfff);
                        ++it;
                        cp += (*it) & 0x3f;
                        break;
                    case 4:
                        ++it;
                        cp = ((cp << 18) & 0x1fffff) + ( ( (0xff & (*it)) << 12) & 0x3ffff);
                        ++it;
                        cp += ( (0xff & (*it)) << 6) & 0xfff;
                        ++it;
                        cp += (*it) & 0x3f;
                        break;
                }
                out += cp;
                ++it;
            }
            return out;
        }



        static std::string UTF8(const std::u32string & unicode)
        {
            std::string result;

            for(auto & cp : unicode)
            {
                if (cp < 0x80)                        // one octet

                    result.push_back( static_cast<uint8_t>(cp) );
                else if (cp < 0x800) {                // two octets
                    result.push_back( static_cast<uint8_t>((cp >> 6)          | 0xc0) );
                    result.push_back( static_cast<uint8_t>((cp & 0x3f)        | 0x80) );
                }
                else if (cp < 0x10000) {              // three octets
                    result.push_back( static_cast<uint8_t>((cp >> 12)         | 0xe0) );
                    result.push_back( static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80) );
                    result.push_back( static_cast<uint8_t>((cp & 0x3f)        | 0x80) );
                }
                else {                                // four octets
                    result.push_back( static_cast<uint8_t>((cp >> 18)         | 0xf0) );
                    result.push_back( static_cast<uint8_t>(((cp >> 12) & 0x3f)| 0x80) );
                    result.push_back( static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80) );
                    result.push_back( static_cast<uint8_t>((cp & 0x3f)        | 0x80) );
                }
            }
            return result;
        }


    }
}

#endif
