#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include <utility>

namespace statistics_monitor
{

	class MeanCalculator
	{
		int _lastComputeSize;
		double _dataSum;
		double _mean;

	public:
		// calculate is called by acquire so at least one element so never throw  divid by 0
		MeanCalculator() : _lastComputeSize(0), _dataSum(0), _mean(0.0) {}

		double AddAndcalculate(int i) noexcept;
		double get() const
		{
			return _mean;
		}
	};
	// when 2 mod are equal i take the first one (I dont take the mean of the two because it is not an int)
	class ModCalculator
	{
		std::unordered_map<int, int> _modMap;
		int currentMod;
	public:
		ModCalculator() : currentMod(0) {}
		int AddAndcalculate(int i) noexcept;
		int get() const
		{
			return currentMod;
		}
	};

	// this class calculate the percentile and the mode 
	class PctileCalculator
	{
		std::map<int, int> _modMap;
		int totalItems;
	public:
		PctileCalculator() : totalItems(0) {}
		void AddAndcalculate(int value) noexcept;

		int get(unsigned int pctile) const;
	};


	class Statistics
	{
		double _mean;
		int _mode;
		unsigned int _pctile;
		int _pctilethreshold;

	public:
		Statistics(double mean, int mode, unsigned int pctile, int pctilet) :
			_mean(mean), _mode(mode), _pctile(pctile), _pctilethreshold(pctilet)
		{
		}

		Statistics() : _mean(0.0), _mode(0), _pctile(0), _pctilethreshold(0)
		{
		}

		void setMean(double d) { _mean = d; }
		void setMode(int i) { _mode = i; }
		void setPctilethreshold(int i) { _pctilethreshold = i; } //it is the threshold
		void setPctile(unsigned int i) { _pctile = i; };// it is the value at a percetage threshold

		double getMean() const { return _mean; }
		int getMode() const { return _mode; };
		unsigned int getPctile() const { return _pctile; }// it is the value at a percetage threshold
		int getPctilethreshold() const { return _pctilethreshold; }// it is the value at a percetage threshold
	};

	class StatisticsProvider
	{
		MeanCalculator _meanCalc;
		ModCalculator _modCalc;
		PctileCalculator _pctileCalc;

		// calculate is called by acquire so at least one element so never throw  divid by 0

		void AddAndcalculate(int val) noexcept;

	public:
		void aquire(int val)
		{
			AddAndcalculate(val);
		}

		double getMean() const
		{
			return _meanCalc.get();
		}

		int getMode() const
		{
			return _modCalc.get();
		}

		double getPctile(int pctile) const
		{
			return _pctileCalc.get(pctile);
		};

		const PctileCalculator getPctileCalculator() const
		{
			return _pctileCalc;
		};
	};


	class EventStatistics
	{
		double _mean;
		int _mode;
		PctileCalculator _pctileCalc;

	public:
		EventStatistics() : _mean(0), _mode(0), _pctileCalc(PctileCalculator())
		{
		}

		EventStatistics(double mean, int mode, const PctileCalculator& _pctileCalc) :
			_mean(mean), _mode(mode), _pctileCalc(_pctileCalc)
		{
		}

		double getMean() const
		{
			return _mean;
		}

		int getMode() const
		{
			return _mode;
		}

		double getPctile(int pctile) const
		{
			return _pctileCalc.get(pctile);
		};
	};

}
