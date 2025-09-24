#include <cassert>
#include "Statistics.h"

namespace hsbctest4
{
	// calculate is called by acquire so at least one element so never throw  divid by 0
	double MeanCalculator::AddAndcalculate(int i)
	{
		_dataSum += i; //update dataSum
		_lastComputeSize++;
		_mean = _dataSum / (_lastComputeSize);
		return _mean;
	}

	int ModCalculator::AddAndcalculate(int i)
	{
		// collection start at 0 so lastdataSize is the postion of the new one 
		_modMap[i] += 1;

		if (_modMap[i] > _modMap[currentMod])
		{
			currentMod = i;
		}
		return currentMod;
	}

	void PctileCalculator::AddAndcalculate(int value)
	{
		// try to add one if alrady exists increment it
		auto it = _modMap.insert(std::make_pair(value, 1));
		if (!it.second)
		{
			it.first->second++;// increment the counter if already exists
		}
		totalItems++;
	}

	// it is not the academic caculus, I just want to send a value as an iteger un the list of value (0 if empty)
	int PctileCalculator::get(int pctile) const
	{
		assert(pctile >= 0 && pctile <= 100);
		int threshold = pctile * totalItems / 100;
		auto it = _modMap.begin();
		if (it != _modMap.end())
		{
			int pctilecalc = it->second;
			while (pctilecalc < threshold && it != _modMap.end())
			{
				pctilecalc += (++it)->second;// increment the counter if already exists
			}
			return it->first; // it is the first items above the pctile 
		}
		else
			return 0;// if empty return 0
	}

	void StatisticsProvider::AddAndcalculate(int val) noexcept
	{
		_meanCalc.AddAndcalculate(val);
		_modCalc.AddAndcalculate(val);
		_pctileCalc.AddAndcalculate(val);
	}
}