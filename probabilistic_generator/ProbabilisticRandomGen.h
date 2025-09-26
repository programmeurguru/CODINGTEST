#pragma once
#include <unordered_set>
#include <algorithm>
#include "DistributionData.h"
#include "NumAndProbability.h"
#include "Randomizer.h"

namespace probabilistic_generator
{
    class ProbabilisticRandomGen
    {
        DistributionData _distrib;
        IRandomizer & _randomizer;
    public:
        /// <summary>
		/// the constructor takes a randomizer and a vector of NumAndProbability
		/// it is passed as parameter to be able to mock it in test
        /// </summary>
        /// <param name="randomizer"></param>
        /// <param name="data"></param>
        ProbabilisticRandomGen(IRandomizer & randomizer, 
                                const std::vector<NumAndProbability>& data);

        int nextFromSample();

    };
}// namespace probabilistic_generator

