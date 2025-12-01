#ifndef POWERSET_H
#define POWERSET_H

#include <iostream> //For std::ostream
#include <vector>   //For std::vector

class PowerSet
{
    //Output debug function
    friend std::ostream &operator<<(std::ostream &os, const PowerSet &ps);

public:
    PowerSet(int sum, int minCardinality, int maxCardinality, std::vector<int> values);
    PowerSet() = delete;

    //Getters
    size_t getSetsNumber() const;
    const std::vector<int> getSet(int i) const;

private:
    std::vector<std::vector<int>> sets;
    int sum, minCardinality, maxCardinality;
    std::vector<int> values;
    
    void computePowerSet();
    void addSet(const std::vector<int> &set, int n);
    void repeatedCombinationsWithSum(int n, int k, std::vector<int> &solution, int pos, int start, int itemsSum);
};

#endif
