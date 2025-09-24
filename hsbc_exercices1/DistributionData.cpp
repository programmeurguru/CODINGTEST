#include <cassert>
#include <algorithm>
#include <functional>
#include "NumAndProbability.h"
#include "DistributionData.h"
#include <stdexcept>
#include <string>

namespace hsbctest
{
    bool DistributionData::compareProba (const NumAndProbability & a, const NumAndProbability & b)
    {
        return a.getProbabilityOfSample() < b.getProbabilityOfSample();
    }

    bool DistributionData::check(const std::vector<NumAndProbability>& probaSet, double precision)
    {
        if(probaSet.empty())
        {
            return false;
		}   

        double total = 0.0;
        for (auto& p : probaSet)
        {
            total += p.getProbabilityOfSample();
        }

        return abs(total - 1.0) < precision;
    }

    bool DistributionData::initialize(const std::vector<NumAndProbability>& vectNumAndProbability)
    {
        if(!check(vectNumAndProbability))
        {
            return false;
		}
        _distributionData.reserve(vectNumAndProbability.size() + 1);
        double proba = 0.0;
        size_t count = vectNumAndProbability.size();
        for (int i = 0; i < count -1; i++)
        {
            proba += vectNumAndProbability[i].getProbabilityOfSample();
            _distributionData.emplace_back(vectNumAndProbability[i].getNumber(), proba);
        }
        // for the last we enforce the proba to 1.0;
        _distributionData.emplace_back(vectNumAndProbability[count-1].getNumber(), 1.0);
		return true;
    }

    int DistributionData::getIntFromDistrib(double d)
    {
        if(_distributionData.empty())
        {
            throw std::logic_error("error : distribution data not initialized");
        }

        if (d < 0.0 || d > 1.0)
        {
            throw std::out_of_range("error : proba out of range ]0,1]");
        }
        else if (d == 0.0)
        {
            return _distributionData.front().getNumber();
        }
        auto it = std::lower_bound(_distributionData.begin(), _distributionData.end(), NumAndProbability(1, d), &compareProba);
        if (it == _distributionData.end())
        {
            std::string s;
            s.reserve(64);
            s += "error : no range for proba : ";
            s += std::to_string(d);
            throw std::exception(s.c_str());
        }
        return it->getNumber();
    }


}