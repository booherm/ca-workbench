#ifndef RANDOMBOOLEANNETWORK_HPP
#define RANDOMBOOLEANNETWORK_HPP

#include <iostream>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include "boost/crc.hpp"

struct Site {
	unsigned int siteId;
	bool currentState;
	bool workingState;
	unsigned int booleanFunctionId;
	std::vector<unsigned int> inputSiteIds;
	std::vector<unsigned int> outputSiteIds;
	std::vector<float> color;
};

const size_t SIZE_SITE_ID = sizeof(Site::siteId);
const size_t SIZE_CURRENT_STATE = sizeof(Site::currentState);
const size_t SIZE_WORKING_STATE = sizeof(Site::workingState);
const size_t SIZE_BOOLEAN_FUNCTION_ID = sizeof(Site::booleanFunctionId);

class RandomBooleanNetwork
{
public:
	RandomBooleanNetwork(
		unsigned int rows,
		unsigned int cols,
		unsigned int connectivity,
		unsigned int externalInputRowCount,
		unsigned int externalOutputRowCount,
		bool neighborhoodConnections
	);
	void resetCellStates();
	void setConnectivity(unsigned int connectivity);
	void setNeighborhoodConnections(bool neighborhoodConnections);
	void updateInputSites();
	bool iterate();
	unsigned int getConnectivity();
	std::vector<Site>* getSites();
	~RandomBooleanNetwork();

private:
	bool initialized;
	std::vector<Site> sites;
	std::default_random_engine rnGen;
	std::set<unsigned int> checkSums;

	unsigned int rows;
	unsigned int cols;
	unsigned int connectivity;
	unsigned int externalInputRowCount;
	unsigned int externalOutputRowCount;
	bool neighborhoodConnections;

	unsigned int iteration = 0;
	unsigned int getSitesCrc32();
	void cleanUp();
};

std::string siteToJson(const Site& site);

unsigned int siteCrc32(const Site& site);

#endif
