#include <iostream>    //For std::ostream
#include <vector>      //For std::vector
#include "PowerSet.h"

using namespace std;

PowerSet::PowerSet(int sum, int minCardinality, int maxCardinality, int minValue, int maxValue)
{
    this->sum = sum;
    this->minCardinality = minCardinality;
    this->maxCardinality = maxCardinality;
    this->minValue = minValue;
    this->maxValue = maxValue;

    computePowerSet();
    sets.shrink_to_fit();
}

int PowerSet::getSetsNumber() const
{
    return sets.size();
}

const vector<int> &PowerSet::getSet(int i) const
{
    return sets.at(i);
}

void PowerSet::computePowerSet()
{
    vector<int> values(maxValue - minValue + 1, -1);
    vector<int> solution(maxCardinality, -1);

    for(int i=minValue; i<=maxValue; i++) values[i-minValue] = i;

    for(int k=minCardinality; k<=maxCardinality; k++)
    {
        repeatedCombinationsWithSum(maxValue - minValue + 1, k, values, solution, sum, 0, 0, 0);
    }
}

void PowerSet::addSet(const vector<int> &set, int n)
{
    vector<int> v(n, -1);
    for(int i=0; i<n; i++) v[i] = set[i];
    sets.emplace_back(v);
}

void PowerSet::repeatedCombinationsWithSum(int n, int k, const vector<int> &values, vector<int> &solution, int sum, int pos, int start, int itemsSum)
{
    if(itemsSum > sum) return;

    //Terminal case, when a solution is found
    if(pos == k)
    {
        if(itemsSum < sum) return;
        addSet(solution, k);
        return;
    }

    //Recursive part
    for(int i=start; i<n; i++)
    {
        solution[k-1-pos] = values[i];
        repeatedCombinationsWithSum(n, k, values, solution, sum, pos + 1, start, itemsSum + values[i]);
        start++;
    }
}

ostream &operator<<(ostream &os, const PowerSet &ps)
{
    for(vector<int> v : ps.sets)
    {
        for(int n : v) cout << n << " ";
        cout << endl;
    }
    return os;
}
