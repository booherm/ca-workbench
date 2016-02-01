#ifndef NEURALNETWORKMODULE_HPP
#define NEURALNETWORKMODULE_HPP

#include "CaWorkbenchModule.hpp"
#include "NeuronSite.hpp"
#include "NeuralSynapse.hpp"
#include <iostream>
#include <set>
#include <unordered_set>
#include <boost/crc.hpp>

class NeuralNetworkModule : public CaWorkbenchModule
{
public:
	// constructors
	NeuralNetworkModule(
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
	void iterate();
	bool getSiteActive(unsigned int siteId);
	std::vector<float>* getSiteColor(unsigned int siteId);
	unsigned int getMaxSiteConnectionsCount();
	std::vector<SiteConnection*>* getSiteConnections(unsigned int siteId);
	void getConfigJson(Json::Value& configJson);
	void getStateJson(Json::Value& stateJson);

	// destructor
	~NeuralNetworkModule();

private:

	std::vector<NeuronSite> neuronSites;
	std::vector<NeuralSynapse> neuralSynapses;
	std::set<unsigned int> checkSums;
	unsigned int connectivity;
	unsigned int externalInputRowCount;
	unsigned int feedbackInputRowCount;
	unsigned int externalOutputRowCount;
	bool neighborhoodConnections;
	bool autoFeedForward;
	bool autoNewInput = false;
	bool fadeInactiveSites = true;

	// neural net specific
	float globalFiringRate;
	float targetFiringRate;
	float initialNeuronFiringThreshold;
	unsigned int firingRateSampleIterations;
	unsigned int globalFiringRateCalcActivations;
	float firingRateThresholdAdjustmentDelta;
	float minSynapseWeight;
	float maxSynapseWeight;
	float synapseWeightAdjustmentDelta;
	double globalAverageFiringThreshold;
	double globalAverageInputWeight;
	bool neuronFiringThresholdOn;

	unsigned int externalInputStartCellIndex;
	unsigned int externalInputEndCellIndex;
	unsigned int feedbackInputStartCellIndex;
	unsigned int feedbackInputEndCellIndex;
	unsigned int internalStartCellIndex;
	unsigned int internalEndCellIndex;
	unsigned int externalOutputStartCellIndex;
	unsigned int externalOutputEndCellIndex;

	// private member functions
	bool setConfigurationValue(const ConfigSetting& setting);
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
	unsigned int getConnectivity();
	void resetCellStates();
	bool setConnectivity(unsigned int connectivity);
	void setNeighborhoodConnections(bool neighborhoodConnections);
	void updateInputSites();
	void shiftInputData(int offset);
	void setSpecificInputPattern(unsigned int patternId);
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
	void handleInputAction(int action, int key);
	void printConfigurationState();
	void printWorkingState();
	void processConfigChangeQueue();
	void cleanUp();
};

#endif
