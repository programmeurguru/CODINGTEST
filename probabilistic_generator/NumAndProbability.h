#pragma once
namespace probabilistic_generator
{

    class NumAndProbability
    {
    private:
        int _number;
        double _probabilityOfSample;

    public:
        NumAndProbability(int number, double probabilityOfSample)
            : _number(number), _probabilityOfSample(probabilityOfSample)
        {
        }
        int getNumber() const {
            return _number;
        }
        double getProbabilityOfSample() const {
            return _probabilityOfSample;
        }
    };
}//probabilistic_generator