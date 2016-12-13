#ifndef TWODIMCAMODULE_HPP
#define TWODIMCAMODULE_HPP

#include "CaWorkbenchModule.hpp"
#include <iostream>
#include <boost/crc.hpp>
#include <set>

class TwoDimCaModule : public CaWorkbenchModule
{
public:
	enum NeighborhoodType {
		VON_NEUMANN = 0,
		MOORE = 1
	};

	TwoDimCaModule(unsigned int rows, unsigned int cols);

	// module interface
	void iterate();
	void getConfigJson(Json::Value& configJson);
	~TwoDimCaModule();

private:

	unsigned int stateCount;
	NeighborhoodType activeNeighborhoodType;
	std::string stateSpecifierString;
	std::set<unsigned int> checkSums;

	unsigned int getSiteIndex(unsigned int row, unsigned int col);
	void resetCellStates();
	void printConfigurationState();
	bool setStateCount(unsigned int stateCount);
	bool setActiveNeighborhoodType(NeighborhoodType neighborhoodType);
	bool setStateSpecifierString(const std::string& stateSpecifierString);
	bool setConfigurationValue(const ConfigSetting& setting);
	void processConfigChangeQueue();
	bool validateConfigurationCompatibility(NeighborhoodType neighborhoodType, unsigned int stateCount, const std::string& stateSpecifierString);
};

#endif
