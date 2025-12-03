#include <iostream>    //For std::ostream
#include <cstddef>    //For size_t
#include <cassert>   //For assert
#include <vector>   //For std::vector

#include "Combinations.h"

RepeatedCombinationsWithSum::RepeatedCombinationsWithSum(int sum, int minCardinality, int maxCardinality, std::vector<int> values)
    : sum(sum), minCardinality(minCardinality), maxCardinality(maxCardinality), values(values)
{
    assert(sum>0 && minCardinality>0 && maxCardinality>0 && values.size()>0);

    generateSets();
    sets.shrink_to_fit();
}

size_t RepeatedCombinationsWithSum::getSetsNumber() const
{
    return sets.size();
}

const std::vector<int>& RepeatedCombinationsWithSum::getSet(size_t i) const
{
    assert(i < this->sets.size());
    return sets.at(i);
}

void RepeatedCombinationsWithSum::generateSets()
{
    int n = static_cast<int>(values.size());  //Number of values to pick
    std::vector<int> solution(maxCardinality, -1);

    for(int k=minCardinality; k<=maxCardinality; k++)
    {
        combine(n, k, solution, 0, 0, 0);
    }
}

void RepeatedCombinationsWithSum::addSet(const std::vector<int> &set, int n)
{
    sets.emplace_back(set.begin(), set.begin() + n); //Creates the set on the go
}

void RepeatedCombinationsWithSum::combine(const int n, const int k, std::vector<int> &solution, const int pos, const int start, const int itemsSum)
{
    //Prune
    if(itemsSum > this->sum) return;

    //Terminal case, when a solution is found
    assert(pos<=k);
    if(pos == k)
    {
        if(itemsSum == this->sum) addSet(solution, k);
        return;
    }

    //Recursive part
    for(int i=start; i<n; i++)
    {
        solution[pos] = values[i];
        combine(n, k, solution, pos + 1, i, itemsSum + values[i]);
    }
}

std::ostream &operator<<(std::ostream &os, const RepeatedCombinationsWithSum &ps)
{
    for(const std::vector<int>& v : ps.sets)
    {
        for(int n : v) os << n << " ";
        os << '\n';
    }
    return os;
}
