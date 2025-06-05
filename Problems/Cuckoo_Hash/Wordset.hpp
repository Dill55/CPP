#ifndef __WORD_SET___HPP
#define __WORD_SET___HPP

#include <string>

namespace shindler::ics46::project3 {



// the given string consists solely of lower-case letters.
[[nodiscard]] size_t polynomialHashFunction(const std::string &string,
                                            unsigned base, unsigned mod);

// These are value template parameters. You can pass in a value rather than a
// type into the parameter and use its value. This allows them to be set
// dynamically rather than having static constants within the class.
// Think of them as class constants (you can just use them like a variables
// named H1, H2, and EvictThreshold)
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
class BaseWordSet {
   private:
   

    std::string * array1;
    std::string * array2;
    size_t num_possible;
    size_t num_entered;

    std::string insertIntoOne(const std::string & string);
    std::string insertIntoTwo(const std::string & string);
    void rehash(const std::string & string);
    size_t nextPrime(const size_t & the_size);

   public:
    explicit BaseWordSet(size_t initialCapacity);
    ~BaseWordSet();

    
    // here to ensure that we let the compiler know these functions
    // should not exist.
    BaseWordSet(const BaseWordSet &) = delete;
    BaseWordSet(BaseWordSet &&) = delete;
    BaseWordSet &operator=(const BaseWordSet &) = delete;
    BaseWordSet &operator=(BaseWordSet &&) = delete;

    // adds the given string to the WordSet, so that any
    // future calls to contains(s) will return true, unless it is later
    // removed.
    void insert(const std::string &string);

    [[nodiscard]] bool contains(const std::string &string) const;

    // return how many distinct strings are in the set
    [[nodiscard]] size_t size() const noexcept;

    // return how large the underlying array is.
    [[nodiscard]] size_t capacity() const noexcept;

    void erase(const std::string &string);
};

// You Should not use anything within the _impl namespace in your code.
// Instead reference the H1, H2, and EvictThreshold template parameters
namespace _impl {
const static unsigned int BASE_H1 = 37;
const static unsigned int BASE_H2 = 41;
const unsigned int BASE_EVICTION_THRESHOLD = 5;
}  // namespace _impl

// Add a usings statement with the default values for the project.
using WordSet =
    BaseWordSet<_impl::BASE_H1, _impl::BASE_H2, _impl::BASE_EVICTION_THRESHOLD>;

template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
BaseWordSet<H1, H2, EvictThreshold>::BaseWordSet(size_t initialCapacity) {
    // TODO: Implement this

    array1 = new std::string[initialCapacity]{""}; // These are our hash tables - Cuckoo hashing
    array2 = new std::string[initialCapacity]{""}; // requires 2 hash tables

    num_possible = initialCapacity; //Makes sure we don't insert into a full table
    num_entered = 0; // Keeps track of how many elements we have filled in O(1) 
}

template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
BaseWordSet<H1, H2, EvictThreshold>::~BaseWordSet() {
    // TODO: Implement this
    delete[] array1; //Deletes the space we allocated for our tables
    delete[] array2;
}

// Inserts an element into our arrays. Expected to be O(1) Could be O(n) or more
// due to rehashing
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
void BaseWordSet<H1, H2, EvictThreshold>::insert(const std::string &string) {
    // TODO: Implement this
    
    std::string temp = string;

    //Tries to insert elements EvictThreshold times.
    for(unsigned int i = 0; i < EvictThreshold; ++i)
    {
        if (i % 2 == 0) // Switches from inserting into array1, then array2
        {
            temp = this -> insertIntoOne(temp); // Replaces temp with the element it replaced
        }
        else
        {
            temp = this -> insertIntoTwo(temp);
        }
        if (temp == "") // If we replaced an empty space, we exit the loop
            i = EvictThreshold;
    }

    num_entered++; // We increment the number of unique strings we have

    if (temp != "") // If we exited due to exceeding the threshold rehash the array
    
        this -> rehash(temp); // Rehashes then inserts our new element

}


// Inserts into array1. O(1) time. Returns the string that got replaced
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
std::string BaseWordSet<H1, H2, EvictThreshold>::insertIntoOne(const std::string & string)
{
    size_t placement = polynomialHashFunction(string, H1, num_possible);
    
    std::string temp = "";

    if (array1[placement] !=  "")
        temp = array1[placement];
    
    array1[placement] = string;
        
    return temp;
}

// Inserts into array2. O(1) time. Returns the string that got replaced
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
std::string BaseWordSet<H1, H2, EvictThreshold>::insertIntoTwo(const std::string & string)
{
    size_t placement = polynomialHashFunction(string, H2, num_possible);
    std::string temp = "";

    if (array2[placement] !=  "")
        temp = array2[placement];
    
    array2[placement] = string;
        
    return temp;
}

// Rehashes the tables, putting everything out then back into it, 
// along with the given string that caused the rehash
// This portion runs in O(n) time but worst case is O(inf) if we can't rehash all the given elements
// Takes O(n) space
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
void BaseWordSet<H1, H2, EvictThreshold>::rehash(const std::string & string)
{
    std::string * tempArray1 = array1; // Saves the current arrays
    std::string * tempArray2 = array2;
    size_t currentSize = num_possible;

    num_possible = this -> nextPrime(num_possible * 2); // Finds the next prime value to rehash the tables
    num_entered = 0; // Resets the number of unique entered strings

    array1 = new std::string[num_possible]; // Fully replaces our arrays
    array2 = new std::string[num_possible];

    this -> insert(string); // Inserts the string that caused the rehash
    for (size_t i = 0; i < currentSize; ++i) // Inserts all the rest
    {
        if (tempArray1[i] != "")
            this -> insert(tempArray1[i]);
        if (tempArray2[i] != "")
            this -> insert(tempArray2[i]);
    }


    delete[] tempArray1;
    delete[] tempArray2;
}

// Returns the next prime given the current number
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
size_t BaseWordSet<H1, H2, EvictThreshold>::nextPrime(const size_t & num)
{
    size_t new_size = num + 1; //Made to compute the next prime given the current number

    for (size_t i = 2; i < new_size; ++i) //Very slow 
    {
        if (new_size % i == 0)
        {
            new_size++;
            i = 2; //Resets i if new_size is found to not be prime
        }
    }

    return new_size;
}


//Returns true if the string is in the hash tables runs in O(1) time
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
bool BaseWordSet<H1, H2, EvictThreshold>::contains(
    const std::string &string) const {
    // TODO: Implement this. 

    size_t placement = polynomialHashFunction(string, H1, num_possible);
    if (array1[placement] == string)
        return true;

    placement = polynomialHashFunction(string, H2, num_possible);
    if (array2[placement] == string)
        return true;
    return false; //if its in neither array return False
}

// return how many distinct strings are in the set
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
size_t BaseWordSet<H1, H2, EvictThreshold>::size() const noexcept {
    // TODO: Implement this. 
    
    return num_entered; 
}

// return how large the underlying array is.
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
size_t BaseWordSet<H1, H2, EvictThreshold>::capacity() const noexcept {
    // TODO: Implement this. 
    

    return num_possible; 
}

// removes this word if it is in the wordset. Runs O(1) time
template <unsigned int H1, unsigned int H2, unsigned int EvictThreshold>
void BaseWordSet<H1, H2, EvictThreshold>::erase(const std::string &string) {
    // TODO: Implement this.

    size_t placement = polynomialHashFunction(string, H1, num_possible);
    size_t placement2 = polynomialHashFunction(string, H2, num_possible);
    if (array1[placement] == string)
        {
        array1[placement] = ""; //Sets it to the default value
        num_entered--; // Decrements the number of unique strings we have
        }
    else if (array2[placement2] == string)
        {
        array2[placement2] = ""; // Sets it to the default value
        num_entered--; // Decrements the number of unique strings we have
        }
     
}

}  // namespace shindler::ics46::project3

#endif
