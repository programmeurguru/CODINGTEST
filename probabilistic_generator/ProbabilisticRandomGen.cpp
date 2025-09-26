
#include <memory>
#include <cassert>
#include <algorithm>
#include "Randomizer.h"
#include "ProbabilisticRandomGen.h"
#include <stdexcept>

namespace probabilistic_generator
{

    ProbabilisticRandomGen::ProbabilisticRandomGen(IRandomizer & randomizer,
                                                   const std::vector<NumAndProbability>& data)
        : _randomizer(randomizer)
    {
        if (!_distrib.initialize(data))
        {
            throw std::invalid_argument("Invalid distribution data");
        }       
    }

    int ProbabilisticRandomGen::nextFromSample()
    {
        return _distrib.getIntFromDistrib(_randomizer.randomize());
    }
}// namespace probabilistic_generator
