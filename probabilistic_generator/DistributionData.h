#pragma once
#include <vector>
#include "NumAndProbability.h"

namespace probabilistic_generator
{
    class DistributionData
    {
            std::vector< NumAndProbability >_distributionData;
            static bool compareProba(const NumAndProbability& a, const NumAndProbability& b);
            static bool check(const std::vector<NumAndProbability>& probaSet, double precision = 1e-8);
    public:
            DistributionData() = default;
            bool initialize(const std::vector<NumAndProbability>& vectNumAndProbability);
            int getIntFromDistrib(double d);
    };
}// namespace probabilistic_generator