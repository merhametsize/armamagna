#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include <iostream> //For std::ostream
#include <cstddef> //For size_t
#include <vector> //For std::vector

class RepeatedCombinationsWithSum
{
    //Output debug function
    friend std::ostream &operator<<(std::ostream &os, const RepeatedCombinationsWithSum &ps);

public:
    explicit RepeatedCombinationsWithSum(int sum, int minCardinality, int maxCardinality, std::vector<int> values);
    RepeatedCombinationsWithSum() = delete;

    //Getters
    size_t getSetsNumber() const;
    const std::vector<int>& getSet(size_t i) const;

private:
    std::vector<std::vector<int>> sets;
    const int sum, minCardinality, maxCardinality;
    const std::vector<int> values;
    
    void generateSets();
    void addSet(const std::vector<int> &set, int n);
    void combine(const int n, const int k, std::vector<int> &solution, const int pos, const int start, const int itemsSum); //Recursive function
};

#endif
