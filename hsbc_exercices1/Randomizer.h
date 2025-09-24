#pragma once
#include <algorithm>

class IRandomizer
{
    public :
        virtual double randomize() = 0;

};

class RandomizerProxy : public IRandomizer
{
    public :
        double randomize() noexcept override
        {
            return double (std::rand()) / RAND_MAX;
        };
};


