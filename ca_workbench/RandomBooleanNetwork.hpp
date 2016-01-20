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
	bool freshActivation;
	unsigned int booleanFunctionId;
	std::vector<unsigned int> inputSiteIds;
	std::vector<unsigned int> outputSiteIds;
	std::vector<float> color;
	unsigned int stateChangeCount;
};

const size_t SIZE_SITE_ID = sizeof(Site::siteId);
const size_t SIZE_CURRENT_STATE = sizeof(Site::currentState);
const size_t SIZE_WORKING_STATE = sizeof(Site::workingState);
const size_t SIZE_BOOLEAN_FUNCTION_ID = sizeof(Site::booleanFunctionId);

class RandomBooleanNetwork
{
public:
	// constructors
	RandomBooleanNetwork(
		unsigned int rows,
		unsigned int cols,
		unsigned int connectivity,
		unsigned int externalInputRowCount,
		unsigned int feedbackInputRowCount,
		unsigned int externalOutputRowCount,
		bool neighborhoodConnections,
		bool autoFeedForward
	);

	// public member functions
	void resetCellStates();
	bool setConnectivity(unsigned int connectivity);
	void setNeighborhoodConnections(bool neighborhoodConnections);
	void updateInputSites();
	void shiftInputData(int offset);
	bool iterate();
	void incrementExternalInputRows();
	void decrementExternalInputRows();
	void incrementFeedbackInputRows();
	void decrementFeedbackInputRows();
	void incrementExternalOutputRows();
	void decrementExternalOutputRows();
	void feedForward();
	void toggleAutoFeedForward();
	void toggleAutoNewInput();
	unsigned int getConnectivity();
	std::vector<Site>* getSites();

	// destructor
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
	unsigned int feedbackInputRowCount;
	unsigned int externalOutputRowCount;
	bool neighborhoodConnections;
	bool autoFeedForward;
	bool autoNewInput = false;

	unsigned int externalInputStartCellIndex;
	unsigned int externalInputEndCellIndex;
	unsigned int feedbackInputStartCellIndex;
	unsigned int feedbackInputEndCellIndex;
	unsigned int internalStartCellIndex;
	unsigned int internalEndCellIndex;
	unsigned int externalOutputStartCellIndex;
	unsigned int externalOutputEndCellIndex;
	unsigned int iteration = 0;

	// private member functions
	void initialize(
		unsigned int rows,
		unsigned int cols,
		unsigned int connectivity,
		unsigned int externalInputRowCount,
		unsigned int feedbackInputRowCount,
		unsigned int externalOutputRowCount,
		bool neighborhoodConnections,
		bool autoFeedForward
	);
	void printConfigurationState();
	void cleanUp();
};

// non-member functions
std::string siteToJson(const Site& site);

#endif
