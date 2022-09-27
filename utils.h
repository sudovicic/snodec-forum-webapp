#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils
{
public:
    Utils();
    static std::string GetFieldByName(unsigned char *body, std::string field);
    static std::string Gen_random(const int len);
};

#endif // UTILS_H
