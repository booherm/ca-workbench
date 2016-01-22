#ifndef RANDOMBOOLEANNETWORKMODULE_HPP
#define RANDOMBOOLEANNETWORKMODULE_HPP

#include "CaWorkbenchModule.hpp"
#include "RandomBooleanNetworkSite.hpp"
#include <glfw3.h>
#include <iostream>
#include <random>
#include <set>
#include <unordered_set>
#include "boost/crc.hpp"

class RandomBooleanNetworkModule : public CaWorkbenchModule
{
public:
	// constructors
	RandomBooleanNetworkModule(
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
	void toggleFadeInactiveSites();
	Site* getSite(unsigned int siteId);
	bool getSiteActive(unsigned int siteId);
	std::vector<float>* getSiteColor(unsigned int siteId);

	unsigned int getConnectivity();
	void handleInputAction(int action, int key);

	// destructor
	~RandomBooleanNetworkModule();

private:

	std::vector<RandomBooleanNetworkSite> sites;
	std::set<unsigned int> checkSums;
	unsigned int connectivity;
	unsigned int externalInputRowCount;
	unsigned int feedbackInputRowCount;
	unsigned int externalOutputRowCount;
	bool neighborhoodConnections;
	bool autoFeedForward;
	bool autoNewInput = false;
	bool fadeInactiveSites = true;

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

#endif
