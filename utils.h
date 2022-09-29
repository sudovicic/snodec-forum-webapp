#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <random>

class Utils
{
public:
    Utils();
    static std::string GetFieldByName(unsigned char *body, std::string field);
    static std::string Gen_random(std::size_t length);
};

#endif // UTILS_H
