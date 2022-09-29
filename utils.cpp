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
     std::replace(result.begin(), result.end(), '+', ' ');
     return result;

 }

std::string Utils::Gen_random(std::size_t length) {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

       std::random_device random_device;
       std::mt19937 generator(random_device());
       std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

       std::string random_string;

       for (std::size_t i = 0; i < length; ++i)
       {
           random_string += CHARACTERS[distribution(generator)];
       }

       return random_string;
}
