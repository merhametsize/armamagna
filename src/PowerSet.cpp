#include <iostream>    //For std::ostream
#include <vector>      //For std::vector
#include "PowerSet.h"

PowerSet::PowerSet(int sum, int minCardinality, int maxCardinality, std::vector<int> values)
    : sum(sum), minCardinality(minCardinality), maxCardinality(maxCardinality), values(values)
{
    computePowerSet();
    sets.shrink_to_fit();
}

size_t PowerSet::getSetsNumber() const
{
    return sets.size();
}

const std::vector<int> PowerSet::getSet(int i) const
{
    return sets.at(i);
}

void PowerSet::computePowerSet()
{
    int n = static_cast<int>(values.size());  //Number of values to pick
    std::vector<int> solution(maxCardinality, -1);

    for(int k=minCardinality; k<=maxCardinality; k++)
    {
        repeatedCombinationsWithSum(n, k, solution, 0, 0, 0);
    }
}

void PowerSet::addSet(const std::vector<int> &set, int n)
{
    std::vector<int> v(n, -1);
    for(int i=0; i<n; i++) v[i] = set[i];
    sets.emplace_back(std::move(v));
}

void PowerSet::repeatedCombinationsWithSum(int n, int k, std::vector<int> &solution, int pos, int start, int itemsSum)
{
    //Prune
    if(itemsSum > this->sum) return;

    //Terminal case, when a solution is found
    if(pos == k)
    {
        if(itemsSum == this->sum) addSet(solution, k);
        return;
    }

    //Recursive part
    for(int i=start; i<n; i++)
    {
        solution[pos] = values[i];
        repeatedCombinationsWithSum(n, k, solution, pos + 1, i, itemsSum + values[i]);
    }
}

std::ostream &operator<<(std::ostream &os, const PowerSet &ps)
{
    for(std::vector<int> v : ps.sets)
    {
        for(int n : v) os << n << " ";
        os << std::endl;
    }
    return os;
}
