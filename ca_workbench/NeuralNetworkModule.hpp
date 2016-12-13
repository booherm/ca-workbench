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
	NeuralNetworkModule(unsigned int rows, unsigned int cols);

	// public configuration member functions
	void getConfigJson(Json::Value& configJson);
	void getStateJson(Json::Value& stateJson);

	// public logic state member functions
	void iterate();
	unsigned char getSiteState(unsigned int siteId);
	std::vector<float>* getSiteColor(unsigned int siteId);
	unsigned int getMaxSiteConnectionsCount();
	std::vector<SiteConnection*>* getSiteConnections(unsigned int siteId);

	// destructor
	~NeuralNetworkModule();

private:

	// general attributes
	std::vector<NeuronSite> neuronSites;
	std::vector<NeuralSynapse> neuralSynapses;
	std::set<unsigned int> checkSums;
	unsigned int connectivity;
	unsigned int externalInputRowCount;
	unsigned int feedbackInputRowCount;
	unsigned int externalOutputRowCount;
	bool autoFeedForward;
	bool autoNewInput;
	bool fadeStaleSites;
	unsigned int activeExternalInputSitePatternId;

	// neural net specific
	const unsigned int maxConnectivity = 20;
	float globalFiringRate;
	float targetFiringRate;
	float initialNeuronFiringThreshold;
	float initialExternalInputSynapseWeight;
	float initialInternalSynapseWeight;
	unsigned int firingRateSampleIterations;
	float firingRateThresholdAdjustmentDelta;
	float minSynapseWeight;
	float maxSynapseWeight;
	float synapseWeightStrengthenDelta;
	float synapseWeightWeakenDelta;
	double globalAverageFiringThreshold;
	double globalAverageInputWeight;
	unsigned int externalInputStartCellIndex;
	unsigned int externalInputEndCellIndex;
	unsigned int feedbackInputStartCellIndex;
	unsigned int feedbackInputEndCellIndex;
	unsigned int internalStartCellIndex;
	unsigned int internalEndCellIndex;
	unsigned int externalOutputStartCellIndex;
	unsigned int externalOutputEndCellIndex;

	// private configuration and input member functions
	bool setConfigurationValue(const ConfigSetting& setting);
	void processConfigChangeQueue();
	void handleInputAction(int action, int key);
	bool setExternalInputRowCount(unsigned int rowCount);
	bool setFeedbackInputRowCount(unsigned int rowCount);
	bool setExternalOutputRowCount(unsigned int rowCount);
	void shiftInputData(int offset);
	bool setActiveExternalInputSitePatternId(unsigned int patternId);
	bool setConnectivity(unsigned int connectivity);
	void setAutoFeedForward(bool autoFeedForward);
	void setAutoNewInput(bool autoNewInput);
	void setFadeStaleSites(bool fadeStaleSites);
	void setTargetFiringRate(float targetFiringRate);
	void setInitialNeuronFiringThreshold(float initialNeuronFiringThreshold);
	bool setInitialExternalInputSynapseWeight(float initialExternalInputSynapseWeight);
	bool setInitialInternalSynapseWeight(float initialInternalSynapseWeight);
	void setFiringRateSampleIterations(unsigned int firingRateSampleIterations);
	void setFiringRateThresholdAdjustmentDelta(float firingRateThresholdAdjustmentDelta);
	void setSynapseWeightStrengthenDelta(float synapseWeightStrengthenDelta);
	void setSynapseWeightWeakenDelta(float synapseWeightWeakenDelta);
	void setMinSynapseWeight(float minSynapseWeight);
	void setMaxSynapseWeight(float maxSynapseWeight);

	// private logical state member functions
	void initialize();
	void resetCellStates();
	void randomizeInputSites();
	void feedForward();
	void cleanUp();
};

#endif
