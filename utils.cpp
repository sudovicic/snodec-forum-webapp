#include "utils.h"

Utils::Utils()
{
}

std::string Utils::GetFieldByName(unsigned char *bodyData, std::string fieldName) {

     std::string bodyString(reinterpret_cast<char *>(bodyData));

     int index = bodyString.find(fieldName);
     int index2 = bodyString.find('=', index) + 1;
     int index3 = bodyString.find('&', index);

     std::string result = bodyString.substr(index2, index3 - (index2));
     return result;

 }

std::string Utils::Gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}
