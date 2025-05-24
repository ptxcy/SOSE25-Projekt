#ifndef BASE64_H
#define BASE64_H

#include <string>

namespace base64
{
    static const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    // Simple Base64 encode implementation to replace cppcodec
    inline std::string encode(const std::string &input)
    {
        std::string result;
        int val = 0;
        int valb = -6;

        for (unsigned char c : input)
        {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0)
            {
                result.push_back(chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }

        if (valb > -6)
        {
            result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
        }

        // Add padding
        while (result.size() % 4)
        {
            result.push_back('=');
        }

        return result;
    }
}

#endif // BASE64_H
