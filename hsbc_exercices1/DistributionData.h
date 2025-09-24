#pragma once
#include <vector>
#include "NumAndProbability.h"

namespace hsbctest
{
    
    class DistributionData
    {
            std::vector< NumAndProbability >_distributionData;
        public:
            static bool compareProba(const NumAndProbability& a, const NumAndProbability& b);
            static bool check(const std::vector<NumAndProbability>& probaSet, double precision = 1e-8);

            DistributionData() = default;
            bool initialize(const std::vector<NumAndProbability>& vectNumAndProbability);
            int getIntFromDistrib(double d);
    };

}