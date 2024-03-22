
#include <random>
#include "Utils.h"



std::string Utils::generateRandomString(int length) {

    const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1); // Soustrayez 1 pour ignorer le caractère nul de fin

    std::random_device rd; // Obtient un seed aléatoire du dispositif ou du système
    std::mt19937 generator(rd()); // Utilise le générateur de nombres aléatoires Mersenne Twister
    std::uniform_int_distribution<size_t> distribution(0, max_index - 1);

    std::string random_string;
    random_string.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        random_string += charset[distribution(generator)];
    }

    return random_string;
}
