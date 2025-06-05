#include "Wordset.hpp"

#include <string>

namespace shindler::ics46::project3 {

// returns s, as a number in the given base, mod the given modulus
size_t polynomialHashFunction(const std::string &string, unsigned int base,
                              unsigned int mod) {

    size_t total = 0;
    int i = string.size() - 1;

    for (auto c : string) //grabs each char in the string
    {
        int b = 1;
        for (int x = 0; x < i; ++x)  //Gives the position of the char
        {
            b %= mod;  //Makes sure we don't go out of bounds
            b *= base; //Multiplying based off the position of the char
        }
        
        b %= mod; // Same as computation above

        total += ((c - 'a' + 1) * b) % mod; //Add it to the total, this is our hash

        --i;
    }



    return total % mod; //Return the final output, must be between [0, mod - 1]
}

}  // namespace shindler::ics46::project3
