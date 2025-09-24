#pragma once
#include <unordered_set>
#include <algorithm>
#include "DistributionData.h"
#include "NumAndProbability.h"
#include "Randomizer.h"

namespace hsbctest
{
    class ProbabilisticRandomGen
    {
        DistributionData _distrib;
        IRandomizer & _randomizer;
    public:
        ProbabilisticRandomGen(IRandomizer & randomizer, 
                                const std::vector<NumAndProbability>& data);

        int nextFromSample();

    };
}

