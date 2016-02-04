#include "NeuralNetworkModule.hpp"

using namespace std;

///////////////////////////////////////  constructors  //////////////////////////////////////////
NeuralNetworkModule::NeuralNetworkModule(unsigned int rows, unsigned int cols) : CaWorkbenchModule(rows, cols) {

	connectivity = 2;
	externalInputRowCount = (unsigned int) ((rows * cols) * 0.00025);
	feedbackInputRowCount = 0;
	externalOutputRowCount = (unsigned int)((rows * cols) * 0.00025);
	autoFeedForward = true;
	autoNewInput = false;
	fadeStaleSites = false;
	activeExternalInputSitePatternId = 1;

	if (externalInputRowCount < 1) {
		externalInputRowCount = 1;
	}
	if (externalOutputRowCount < 1) {
		externalOutputRowCount = 1;
	}


	initialize();
}

///////////////////////////////////////  public logic state member functions  //////////////////
void NeuralNetworkModule::iterate()
{
	// process any requested config changes
	processConfigChangeQueue();

	// update logical state

	if (autoFeedForward)
		feedForward();

	if (autoNewInput)
		this->randomizeInputSites();

	// global calculations variable initialization
	double globalAverageFiringThresholdTotal = 0.0f;
	double globalAverageInputWeightTotal = 0.0f;
	unsigned int globalFiringRateCalcActivations = 0;

	// synchronous read for current state of sites
	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		NeuronSite* s = &neuronSites[i];

		// sum synapse weights potentially contributed to this neuron firing
		float totalContributingInputWeight = 0.0f;
		bool atLeastOneInputFired = false;
		vector<NeuralSynapse*>* inputNeuralSynapses = (vector<NeuralSynapse*>*) &s->neuralSynapses;
		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {

			// source neuron of the input synapse
			NeuralSynapse* inputSynapse = inputNeuralSynapses->at(inputSiteIndex);

			// if source neuron fired, add to sum of weights contributing to this neuron potentially have being fired
			if (neuronSites[inputSynapse->sourceSiteId].currentState){
				totalContributingInputWeight += inputSynapse->connectionStrengthWeight;
				atLeastOneInputFired = true;
			}
		}
		
		// fire this neuron if total input weight exceeds its firing threshold
		bool newState = atLeastOneInputFired && (totalContributingInputWeight >= s->firingThreshold);
		if (newState) {
			s->firingRateCalcActivations++;    // increment counter indicating how many times this neuron has ever fired
			globalFiringRateCalcActivations++; // keep running total of neurons fired for global firing rate calc
		}

		// Update synapse weights.  Synapses that contibuted to neuron firing are strengthend (weight increased),
		// those that did not are weakened (weight decresed).
		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {

			// source neuron of the input synapse
			NeuralSynapse* inputSynapse = inputNeuralSynapses->at(inputSiteIndex);

			if (newState && neuronSites[inputSynapse->sourceSiteId].currentState) {
				// input synapse source neuron was active and this neuron fired, strengthen
				inputSynapse->connectionStrengthWeight += synapseWeightStrengthenDelta;
				if (inputSynapse->connectionStrengthWeight > maxSynapseWeight)
					inputSynapse->connectionStrengthWeight = maxSynapseWeight;
				inputSynapse->shouldRender = true;
				inputSynapse->color.at(3) = 1.0f;  // debug - need to somehow normalize this and have variable alpha
			}
			else {
				// input synapse did not contribute to activation of this neuron, weaken
				if (inputSynapse->sourceSiteId > externalInputStartCellIndex) {
					inputSynapse->connectionStrengthWeight += synapseWeightWeakenDelta;
					if (inputSynapse->connectionStrengthWeight < minSynapseWeight)
						inputSynapse->connectionStrengthWeight = minSynapseWeight;
				}
				inputSynapse->shouldRender = false;
			}

			// keep running total of synapse weight for global average
			globalAverageInputWeightTotal += inputSynapse->connectionStrengthWeight;
		}

		// refresh this neuron's firing rate
		s->firingRate = s->firingRateCalcActivations / (float)(iteration + 1);

		// Adjust this neuron's firing threshold according to it's current firing rate.  If it's firing rate is
		// above the global firing rate taget, increase the threshold it takes to fire the neuron.  Else if it's
		// firing rate is below the global firing rate target, decrease the threshold it takes to fire the neuron.
		if (s->firingRate > targetFiringRate)
			s->firingThreshold += firingRateThresholdAdjustmentDelta;
		else if (s->firingRate < targetFiringRate)
			s->firingThreshold -= firingRateThresholdAdjustmentDelta;

		// keep running total of firing rate for global firing rate calculation
		globalAverageFiringThresholdTotal += s->firingThreshold;

		// update working state
		s->workingState = newState;
	}

	// calculate global average synapse input weight
	globalAverageInputWeight = globalAverageInputWeightTotal / ((externalOutputEndCellIndex - internalStartCellIndex) * connectivity);

	// calculate the global average firing rate
	globalFiringRate = globalFiringRateCalcActivations / (float)(externalOutputEndCellIndex - internalStartCellIndex);

	// calculate global average neuron firing threshold
	globalAverageFiringThreshold = globalAverageFiringThresholdTotal / (externalOutputEndCellIndex - internalStartCellIndex);

	// site updates and calculate overall state checksum
	//boost::crc_32_type crcResult;
	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		NeuronSite* s = &neuronSites[i];

		if (s->currentState != s->workingState) {

			// new state is different from the current state, increment the change counter and reset it to a solid color
			s->stateChangeCount++;
			s->freshActivation = s->workingState;

			if (i >= externalOutputStartCellIndex) {  // external output cells
				s->color.at(0) = 1.0f;
				s->color.at(1) = 0.0f;
				s->color.at(2) = 0.0f;
			}
			else { // internal cells
				s->color.at(0) = 0.0f;
				s->color.at(1) = 0.0f;
				s->color.at(2) = 0.0f;
			}
		}
		else {

			if (fadeStaleSites) {
				// update cell color to fade out as it's non-updated age increases
				if (i >= externalOutputStartCellIndex) {  // external output cells
					float newColor = s->color.at(1) + 0.01f;
					if (newColor > 1.0f)
						newColor = 1.0f;

					s->color.at(0) = 1.0f;
					s->color.at(1) = newColor;
					s->color.at(2) = newColor;
				}
				else { // internal cells
					float newColor = s->color.at(0) + 0.01f;
					if (newColor > 1.0f)
						newColor = 1.0f;

					s->color.at(0) = newColor;
					s->color.at(1) = newColor;
					s->color.at(2) = newColor;
				}
			}

			s->freshActivation = false;
		}

		// apply the working state
		s->currentState = s->workingState;

		// add relevent state values to running checksum bytes
		//crcResult.process_bytes(&s->siteId, SIZE_SITE_ID);
		//crcResult.process_bytes(&s->currentState, SIZE_CURRENT_STATE);
		//crcResult.process_byte(s->currentState);
	}
	//unsigned int crcResultChecksum = crcResult.checksum();

	// store checksum in set, mark render complete if a cycle has been detected yet
	iteration++;
	/*
	std::set<unsigned int>::iterator csi = checkSums.find(crcResultChecksum);
	if (csi == checkSums.end()) {
	checkSums.insert(crcResultChecksum);
	}
	else {
	cout << "Cycle detected on iteration " << std::to_string(iteration) << endl;
	renderComplete = true;
	}

	bool settled = false; // global firing rate stable?

	if (renderComplete || settled) {
	randomizeInputSites();
	renderComplete = false;
	}
	*/
}

inline bool NeuralNetworkModule::getSiteActive(unsigned int siteId) {
	return neuronSites[siteId].currentState;
}

inline std::vector<float>* NeuralNetworkModule::getSiteColor(unsigned int siteId) {
	return &neuronSites[siteId].color;
}

inline unsigned int NeuralNetworkModule::getMaxSiteConnectionsCount() {
	return maxConnectivity * rows * cols;
}

inline std::vector<SiteConnection*>* NeuralNetworkModule::getSiteConnections(unsigned int siteId) {
	return (std::vector<SiteConnection*>*) &neuronSites[siteId].neuralSynapses;
}

///////////////////////////////////////  destructor   //////////////////////////////////////////
NeuralNetworkModule::~NeuralNetworkModule()
{
	cleanUp();
}

///////////////////////////////////////  private logical state member functions   //////////////
void NeuralNetworkModule::initialize() {

	// site type boundaries
	externalInputStartCellIndex = 0;
	externalInputEndCellIndex = externalInputStartCellIndex + (externalInputRowCount * cols) - 1;
	feedbackInputStartCellIndex = externalInputEndCellIndex + 1;
	feedbackInputEndCellIndex = feedbackInputStartCellIndex + (feedbackInputRowCount * cols) - 1;
	internalStartCellIndex = feedbackInputEndCellIndex + 1;
	internalEndCellIndex = internalStartCellIndex + ((rows - (externalInputRowCount + feedbackInputRowCount + externalOutputRowCount)) * cols) - 1;
	externalOutputStartCellIndex = internalEndCellIndex + 1;
	externalOutputEndCellIndex = externalOutputStartCellIndex + (externalOutputRowCount * cols) - 1;

	// neural net specific
	targetFiringRate = 0.25f;
	firingRateSampleIterations = 0;
	initialNeuronFiringThreshold = 0.1f;
	initialExternalInputSynapseWeight = 1.0f;
	initialInternalSynapseWeight = 0.0f;
	firingRateThresholdAdjustmentDelta = 0.1f;
	synapseWeightStrengthenDelta = 0.1f;
	synapseWeightWeakenDelta = 0.0f;
	minSynapseWeight = -1.0f;
	maxSynapseWeight = 1.0f;

	resetCellStates();
}

void NeuralNetworkModule::resetCellStates()
{
	iteration = 0;
	cleanUp();

	// initialize sites
	globalFiringRate = 0.0f;
	globalAverageFiringThreshold = 0.0f;
	globalAverageInputWeight = 0.0f;
	unsigned int siteCount = rows * cols;
	neuronSites.resize(siteCount);
	neuralSynapses.resize(siteCount * connectivity);
	unsigned int neuralSynapsesIndex = 0;
	bernoulli_distribution initialStateRandomDist(0.50);
	vector<float> initialConnectionColor(4);
	initialConnectionColor[0] = 0.0f;
	initialConnectionColor[1] = 1.0f;
	initialConnectionColor[2] = 0.0f;
	initialConnectionColor[3] = 0.75f;

	for (unsigned int i = 0; i < siteCount; i++) {

		// initialize neuron site
		NeuronSite s;
		s.siteId = i;
		s.firingRate = 0.0f;
		s.firingThreshold = initialNeuronFiringThreshold;
		s.firingRateCalcActivations = 0;
		s.freshActivation = false;
		s.workingState = false;
		s.stateChangeCount = 0;
		s.currentState = false;

		s.color.resize(3);
		if (i >= externalInputStartCellIndex && i <= externalInputEndCellIndex) {
			// external input site, blue
			s.color[0] = 0.0f;
			s.color[1] = 0.0f;
			s.color[2] = 1.0f;
		}
		else if (i >= feedbackInputStartCellIndex && i <= feedbackInputEndCellIndex) {
			// feedback input site, green
			s.color[0] = 0.0f;
			s.color[1] = 1.0f;
			s.color[2] = 0.0f;
		}
		else if (i >= externalOutputStartCellIndex && i <= externalOutputEndCellIndex) {
			// external output site, red
			s.color[0] = 1.0f;
			s.color[1] = 0.0f;
			s.color[2] = 0.0f;
		}
		else {
			// internal site, black
			s.color[0] = 0.0f;
			s.color[1] = 0.0f;
			s.color[2] = 0.0f;
		}

		neuronSites[i] = s;
	}
	setActiveExternalInputSitePatternId(activeExternalInputSitePatternId);

	// each input site must be connected to exactly one neuron
	uniform_int_distribution<unsigned int> randInternalSiteDist(internalStartCellIndex, internalEndCellIndex);
	for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
		NeuronSite* externalInputSite = &neuronSites[i];
		bool validSelection;
		unsigned int outputSiteId;
		NeuronSite* outputSite;
		do {
			validSelection = true;
			outputSiteId = randInternalSiteDist(rnGen);
			outputSite = &neuronSites[outputSiteId];

			// output site must not already have an input connection
			if (outputSite->neuralSynapses.size() != 0) {
				validSelection = false;
			}
		} while (!validSelection);

		// create synapse connection, store in the master list of synapse connections
		NeuralSynapse sc(i, outputSiteId, initialConnectionColor, initialExternalInputSynapseWeight, false);
		neuralSynapses[neuralSynapsesIndex] = sc;

		// store a reference to this new connection in the outputSite's list of input synapse connections
		vector<NeuralSynapse>::iterator scIt = neuralSynapses.begin() + neuralSynapsesIndex;
		outputSite->neuralSynapses.push_back((NeuralSynapse*)&(*scIt));
		neuralSynapsesIndex++;
	}

	// initialize remaining site connectivity, arbitrary sites
	uniform_int_distribution<unsigned int> randInputDist(feedbackInputStartCellIndex, externalOutputStartCellIndex - 1);
	uniform_real_distribution<float> randFloatDist(-1.0f, 1.0f);
	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		NeuronSite* s = &neuronSites[i];
		for (unsigned int k = 0; k < connectivity; k++) {
			unsigned int inputSite;
			bool validSelection;
			do {
				validSelection = true;
				inputSite = randInputDist(rnGen);
				// site cannot be it's own input
				if (inputSite == s->siteId) {
					validSelection = false;
					break;
				}
				else {
					// make sure this input site has not already been chosen as an input for this site
					for (unsigned int l = 0; l < s->neuralSynapses.size(); l++) {
						if (s->neuralSynapses[l]->sourceSiteId == inputSite) {
							validSelection = false;
							break;
						}
					}
				}
			} while (!validSelection);

			// create synapse connection, store in the master list of synapse connections
			NeuralSynapse sc(inputSite, s->siteId, initialConnectionColor, initialInternalSynapseWeight, false);
			neuralSynapses[neuralSynapsesIndex] = sc;

			// store a reference to this synapse connection in this site's list of input synapse connections
			vector<NeuralSynapse>::iterator scIt = neuralSynapses.begin() + neuralSynapsesIndex;
			s->neuralSynapses.push_back((NeuralSynapse*)&(*scIt));
			neuralSynapsesIndex++;
		}
	}
}

void NeuralNetworkModule::randomizeInputSites()
{
	bernoulli_distribution initialStateRandomDist(0.50);
	for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
		NeuronSite* s = &neuronSites[i];
		s->currentState = s->freshActivation = initialStateRandomDist(rnGen);
	}
}

void NeuralNetworkModule::feedForward() {
	// Copy feedbackInputRowCount rows from the external output into the feedback input sites.
	unsigned int outputSiteIndex = externalOutputStartCellIndex;
	for (unsigned int i = feedbackInputStartCellIndex; i <= feedbackInputEndCellIndex && outputSiteIndex <= externalOutputEndCellIndex; i++) {
		NeuronSite* feedbackSite = &neuronSites[i];
		NeuronSite* outputSite = &neuronSites[outputSiteIndex++];
		feedbackSite->currentState = outputSite->currentState;
	}
}

void NeuralNetworkModule::cleanUp()
{
	neuronSites.clear();
	neuralSynapses.clear();
	checkSums.clear();
}
