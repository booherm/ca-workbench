#include "NeuralNetworkModule.hpp"

using namespace std;

///////////////////////////////////////  public configuration memeber functions  ////////////////
void NeuralNetworkModule::getConfigJson(Json::Value& configJson) {
	configJson.clear();
	configJson["rows"] = rows;
	configJson["columns"] = cols;
	configJson["connectivity"] = connectivity;
	configJson["externalInputRowCount"] = externalInputRowCount;
	configJson["feedbackInputRowCount"] = feedbackInputRowCount;
	configJson["externalOutputRowCount"] = externalOutputRowCount;
	configJson["autoFeedForward"] = autoFeedForward;
	configJson["autoNewInput"] = autoNewInput;
	configJson["fadeStaleSites"] = fadeStaleSites;
	configJson["targetFiringRate"] = targetFiringRate;
	configJson["initialNeuronFiringThreshold"] = initialNeuronFiringThreshold;
	configJson["initialExternalInputSynapseWeight"] = initialExternalInputSynapseWeight;
	configJson["initialInternalSynapseWeight"] = initialInternalSynapseWeight;
	configJson["firingRateSampleIterations"] = firingRateSampleIterations;
	configJson["firingRateThresholdAdjustmentDelta"] = firingRateThresholdAdjustmentDelta;
	configJson["synapseWeightStrengthenDelta"] = synapseWeightStrengthenDelta;
	configJson["synapseWeightWeakenDelta"] = synapseWeightWeakenDelta;
	configJson["minSynapseWeight"] = minSynapseWeight;
	configJson["maxSynapseWeight"] = maxSynapseWeight;
	configJson["activeExternalInputSitePatternId"] = activeExternalInputSitePatternId;
}

void NeuralNetworkModule::getStateJson(Json::Value& stateJson) {
	stateJson.clear();
	stateJson["iteration"] = iteration;
	stateJson["renderComplete"] = renderComplete;
	stateJson["globalFiringRate"] = globalFiringRate;
	stateJson["globalAverageFiringThreshold"] = globalAverageFiringThreshold;
	stateJson["globalAverageInputWeight"] = globalAverageInputWeight;
}

///////////////////////////////////////  private configuration memeber functions  ////////////////
bool NeuralNetworkModule::setConfigurationValue(const ConfigSetting& setting) {
	if (setting.key == "connectivity") {
		return setConnectivity(stoul(setting.value));
	}
	else if (setting.key == "externalInputRowCount") {
		return setExternalInputRowCount(stoul(setting.value));
	}
	else if (setting.key == "feedbackInputRowCount") {
		return setFeedbackInputRowCount(stoul(setting.value));
	}
	else if (setting.key == "externalOutputRowCount") {
		return setExternalOutputRowCount(stoul(setting.value));
	}
	else if (setting.key == "autoFeedForward") {
		setAutoFeedForward(setting.value == "true" ? true : false);
		return true;
	}
	else if (setting.key == "autoNewInput") {
		setAutoNewInput(setting.value == "true" ? true : false);
		return true;
	}
	else if (setting.key == "targetFiringRate") {
		setTargetFiringRate(stof(setting.value));
		return true;
	}
	else if (setting.key == "initialNeuronFiringThreshold") {
		setInitialNeuronFiringThreshold(stof(setting.value));
		return true;
	}
	else if (setting.key == "initialExternalInputSynapseWeight") {
		return setInitialExternalInputSynapseWeight(stof(setting.value));
	}
	else if (setting.key == "initialInternalSynapseWeight") {
		return setInitialInternalSynapseWeight(stof(setting.value));
	}
	else if (setting.key == "firingRateSampleIterations") {
		setFiringRateSampleIterations(stoul(setting.value));
		return true;
	}
	else if (setting.key == "firingRateThresholdAdjustmentDelta") {
		setFiringRateThresholdAdjustmentDelta(stof(setting.value));
		return true;
	}
	else if (setting.key == "synapseWeightStrengthenDelta") {
		setSynapseWeightStrengthenDelta(stof(setting.value));
		return true;
	}
	else if (setting.key == "synapseWeightWeakenDelta") {
		setSynapseWeightWeakenDelta(stof(setting.value));
		return true;
	}
	else if (setting.key == "minSynapseWeight") {
		setMinSynapseWeight(stof(setting.value));
		return true;
	}
	else if (setting.key == "maxSynapseWeight") {
		setMaxSynapseWeight(stof(setting.value));
		return true;
	}
	else if (setting.key == "activeExternalInputSitePatternId") {
		return setActiveExternalInputSitePatternId(stoul(setting.value));
	}
	else if (setting.key == "reset") {
		resetCellStates();
		return true;
	}
	else if (setting.key == "fadeStaleSites") {
		setFadeStaleSites(setting.value == "true" ? true : false);
		return true;
	}

	return false;
}

void NeuralNetworkModule::processConfigChangeQueue() {
	if (!configUpdateQueue.empty()) {
		configUpdateMutex.lock();
		while (!configUpdateQueue.empty()) {
			ConfigSetting setting = configUpdateQueue.front();
			configUpdateQueue.pop();
			setConfigurationValue(setting);
		}
		configUpdateMutex.unlock();
	}
}

void NeuralNetworkModule::handleInputAction(int action, int key) {

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_SPACE:
			resetCellStates();
			break;
		case GLFW_KEY_F:
			feedForward();
			break;
		case GLFW_KEY_INSERT:
			shiftInputData(0);
			break;
		}
	}
}

bool NeuralNetworkModule::setExternalInputRowCount(unsigned int rowCount) {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (internalRowCount == 0 && rowCount > externalInputRowCount)
	{
		cout << "not enough space for requsted row count, ignoring" << endl;
		return false;
	}

	if (rowCount < 1)  // debug, fix this so 0 is allowed
	{
		cout << "at least one external input row is required, ignoring" << endl;
		return false;
	}

	externalInputRowCount = rowCount;
	initialize();

	return true;
}

bool NeuralNetworkModule::setFeedbackInputRowCount(unsigned int rowCount) {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (internalRowCount == 1 && rowCount > feedbackInputRowCount)
	{
		cout << "not enough space for requsted row count, ignoring" << endl;
		return false;
	}

	feedbackInputRowCount = rowCount;
	initialize();

	return true;
}

bool NeuralNetworkModule::setExternalOutputRowCount(unsigned int rowCount) {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (internalRowCount == 1 && rowCount > externalOutputRowCount)
	{
		cout << "not enough space for requsted row count, ignoring" << endl;
		return false;
	}

	externalOutputRowCount = rowCount;
	initialize();

	return true;
}

void NeuralNetworkModule::shiftInputData(int offset) {
	bool t1;
	bool t2 = neuronSites[externalInputEndCellIndex].currentState;

	for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
		NeuronSite* s = &neuronSites[i];
		t1 = s->currentState;
		s->currentState = t2;
		t2 = t1;
	}

	checkSums.clear();
}

bool NeuralNetworkModule::setActiveExternalInputSitePatternId(unsigned int patternId) {

	switch (patternId) {
		case 0: {
			// random 50% on
			randomizeInputSites();
			activeExternalInputSitePatternId = patternId;
			return true;
		}
		case 1: {
			// left third all on
			unsigned int leftBoundary = cols / 3;
			for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
				unsigned int col = i % cols;
				NeuronSite* s = &neuronSites[i];
				s->currentState = s->freshActivation = col < leftBoundary;
			}
			activeExternalInputSitePatternId = patternId;
			return true;
		}
		case 2: {
			// middle third all on
			unsigned int leftBoundary = cols / 3;
			unsigned int rightBoundary = leftBoundary + (cols / 3);
			for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
				unsigned int col = i % cols;
				NeuronSite* s = &neuronSites[i];
				s->currentState = s->freshActivation = col > leftBoundary && col < rightBoundary;
			}
			activeExternalInputSitePatternId = patternId;
			return true;
		}
		case 3: {
			// right third all on
			unsigned int rightBoundary = cols - (cols / 3);
			for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
				unsigned int col = i % cols;
				NeuronSite* s = &neuronSites[i];
				s->currentState = s->freshActivation = col > rightBoundary;
			}
			activeExternalInputSitePatternId = patternId;
			return true;
		}
		default: {
			cout << "unrecognized specific input pattern ID " << patternId << endl;
			return false;
		}
	}
}

bool NeuralNetworkModule::setConnectivity(unsigned int connectivity) {
	if (connectivity <= maxConnectivity) {
		this->connectivity = connectivity;
		resetCellStates();
	}
	else {
		cout << "connectivity out of bounds, ignoring" << endl;
		return false;
	}

	return true;
}

void NeuralNetworkModule::setAutoFeedForward(bool autoFeedForward) {
	this->autoFeedForward = autoFeedForward;
}

void NeuralNetworkModule::setAutoNewInput(bool autoNewInput) {
	this->autoNewInput = autoNewInput;
}

void NeuralNetworkModule::setFadeStaleSites(bool fadeStaleSites) {
	this->fadeStaleSites = fadeStaleSites;
}

void NeuralNetworkModule::setTargetFiringRate(float targetFiringRate) {
	this->targetFiringRate = targetFiringRate;
}

void NeuralNetworkModule::setInitialNeuronFiringThreshold(float initialNeuronFiringThreshold) {
	this->initialNeuronFiringThreshold = initialNeuronFiringThreshold;
}

bool NeuralNetworkModule::setInitialExternalInputSynapseWeight(float initialExternalInputSynapseWeight) {
	if (initialExternalInputSynapseWeight < minSynapseWeight || initialExternalInputSynapseWeight > maxSynapseWeight) {
		cout << "initial external input synapse weight out of bounds" << endl;
		return false;
	}

	this->initialExternalInputSynapseWeight = initialExternalInputSynapseWeight;
	return true;
}

bool NeuralNetworkModule::setInitialInternalSynapseWeight(float initialInternalSynapseWeight) {
	if (initialInternalSynapseWeight < minSynapseWeight || initialInternalSynapseWeight > maxSynapseWeight) {
		cout << "initial internal synapse weight out of bounds" << endl;
		return false;
	}

	this->initialInternalSynapseWeight = initialInternalSynapseWeight;
	return true;
}

void NeuralNetworkModule::setFiringRateSampleIterations(unsigned int firingRateSampleIterations) {
	this->firingRateSampleIterations = firingRateSampleIterations;
}

void NeuralNetworkModule::setFiringRateThresholdAdjustmentDelta(float firingRateThresholdAdjustmentDelta) {
	this->firingRateThresholdAdjustmentDelta = firingRateThresholdAdjustmentDelta;
}

void NeuralNetworkModule::setSynapseWeightStrengthenDelta(float synapseWeightStrengthenDelta) {
	this->synapseWeightStrengthenDelta = synapseWeightStrengthenDelta;
}

void NeuralNetworkModule::setSynapseWeightWeakenDelta(float synapseWeightWeakenDelta) {
	this->synapseWeightWeakenDelta = synapseWeightWeakenDelta;
}

void NeuralNetworkModule::setMinSynapseWeight(float minSynapseWeight) {
	this->minSynapseWeight = minSynapseWeight;
}

void NeuralNetworkModule::setMaxSynapseWeight(float maxSynapseWeight) {
	this->maxSynapseWeight = maxSynapseWeight;
}
