#pragma once
#include <algorithm>

namespace probabilistic_generator
{
    class IRandomizer
    {
    public:
        virtual double randomize() = 0;

    };

    class RandomizerProxy : public IRandomizer
    {
    public:
        double randomize() noexcept override
        {
            return double(std::rand()) / RAND_MAX;
        };
    };
}// namespace probabilistic_generator

