#include "NeuralNetworkModule.hpp"

using namespace std;

NeuralNetworkModule::NeuralNetworkModule(
	unsigned int rows,
	unsigned int cols,
	unsigned int connectivity,
	unsigned int externalInputRowCount,
	unsigned int feedbackInputRowCount,
	unsigned int externalOutputRowCount,
	bool neighborhoodConnections,
	bool autoFeedForward
) : CaWorkbenchModule(rows, cols) {

	initialize(
		rows,
		cols,
		connectivity,
		externalInputRowCount,
		feedbackInputRowCount,
		externalOutputRowCount,
		neighborhoodConnections,
		autoFeedForward
	);
}

void NeuralNetworkModule::initialize(
	unsigned int rows,
	unsigned int cols,
	unsigned int connectivity,
	unsigned int externalInputRowCount,
	unsigned int feedbackInputRowCount,
	unsigned int externalOutputRowCount,
	bool neighborhoodConnections,
	bool autoFeedForward
) {
	this->rows = rows;
	this->cols = cols;
	this->connectivity = connectivity;
	this->externalInputRowCount = externalInputRowCount;
	this->feedbackInputRowCount = feedbackInputRowCount;
	this->externalOutputRowCount = externalOutputRowCount;
	this->neighborhoodConnections = neighborhoodConnections;
	this->autoFeedForward = autoFeedForward;

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
	targetFiringRate = 0.5f;
	firingRateSampleIterations = 20;
	initialNeuronFiringThreshold = 100.0f;
	firingRateThresholdAdjustmentDelta = 1.0f;
	synapseWeightAdjustmentDelta = 0.05f;
	minSynapseWeight = -5000.0f;
	maxSynapseWeight = 5000.0f;

	resetCellStates();
	printConfigurationState();
}

void NeuralNetworkModule::updateInputSites()
{
	bernoulli_distribution initialStateRandomDist(0.50);
	for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
		NeuronSite* s = &neuronSites[i];
		s->currentState = initialStateRandomDist(rnGen);
	}
	checkSums.clear();
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

void NeuralNetworkModule::setSpecificInputPattern(unsigned int patternId) {

	switch (patternId) {
		case 0: {
			// left third all on
			unsigned int leftBoundary = cols / 3;
			for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
				unsigned int col = i % cols;
				NeuronSite* s = &neuronSites[i];
				s->currentState = col < leftBoundary;
			}
			break;
		}
		case 1: {
			// middle third all on
			unsigned int leftBoundary = cols / 3;
			unsigned int rightBoundary = leftBoundary + (cols / 3);
			for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
				unsigned int col = i % cols;
				NeuronSite* s = &neuronSites[i];
				s->currentState = col > leftBoundary && col < rightBoundary;
			}
			break;
		}
		case 2: {
			// right third all on
			unsigned int rightBoundary = cols - (cols / 3);
			for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
				unsigned int col = i % cols;
				NeuronSite* s = &neuronSites[i];
				s->currentState = col > rightBoundary;
			}
			break;
		}
		default:{
			break;
		}
	}
}

void NeuralNetworkModule::resetCellStates()
{
	iteration = 0;
	cleanUp();

	// initialize sites
	globalFiringRateCalcActivations = 0;
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
			// external input site, blue, random initial state
			s.color[0] = 0.0f;
			s.color[1] = 0.0f;
			s.color[2] = 1.0f;
			s.currentState = initialStateRandomDist(rnGen);
			s.freshActivation = s.currentState;
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

	if (neighborhoodConnections)
	{
		unsigned int internalSiteStartRow = internalStartCellIndex / cols;
		unsigned int externalOutputSiteEndRow = externalOutputEndCellIndex / cols;

		// neighbor connectivity
		if (connectivity <= 4) // von Neumann neighborhood (k <= 4)
		{
			for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {

				NeuronSite* s = &neuronSites[i];

				// put neighbors in bag
				unsigned int thisSiteR = i / cols;
				unsigned int thisSiteC = i - (thisSiteR * cols);
				unsigned int neighborR;
				unsigned int neighborC;
				std::unordered_set<unsigned int> bag;
				// above
				neighborR = thisSiteR == internalSiteStartRow ? externalOutputSiteEndRow : thisSiteR - 1;
				neighborC = thisSiteC;
				bag.insert((neighborR * cols) + neighborC);
				// right
				neighborR = thisSiteR;
				neighborC = thisSiteC == cols - 1 ? 0 : thisSiteC + 1;
				bag.insert((neighborR * cols) + neighborC);
				// below
				neighborR = thisSiteR == externalOutputSiteEndRow ? internalSiteStartRow : thisSiteR + 1;
				neighborC = thisSiteC;
				bag.insert((neighborR * cols) + neighborC);
				// left
				neighborR = thisSiteR;
				neighborC = thisSiteC == 0 ? cols - 1 : thisSiteC - 1;
				bag.insert((neighborR * cols) + neighborC);

				// assign k inputs randomly by drawing from the bag of neighbors
				for (unsigned int k = 0; k < connectivity; k++) {
					uniform_int_distribution<unsigned int> randNeighborDist(0, bag.size() - 1);
					std::unordered_set<unsigned int>::iterator it = bag.begin();
					unsigned int inputSiteBagIndex = randNeighborDist(rnGen);
					for (unsigned int z = 0; z < inputSiteBagIndex; z++) {
						it++;
					}
					unsigned int inputSite = *it;

					// create synapse connection, store in the master list of synapse connections
					NeuralSynapse sc(inputSite, s->siteId, initialConnectionColor, 0.0f, neuronSites[inputSite].freshActivation);
					neuralSynapses[neuralSynapsesIndex] = sc;

					// store a reference to this synapse connection in this site's list of input synapse connections
					vector<NeuralSynapse>::iterator scIt = neuralSynapses.begin() + neuralSynapsesIndex;
					s->neuralSynapses.push_back((NeuralSynapse*) &(*scIt));
					neuralSynapsesIndex++;

					// remove from bag
					bag.erase(it);
				}
			}
		}
		else  // Moore neighborhood
		{
			for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {

				NeuronSite* s = &neuronSites[i];

				// put neighbors in bag
				unsigned int thisSiteR = i / cols;
				unsigned int thisSiteC = i - (thisSiteR * cols);
				unsigned int neighborR;
				unsigned int neighborC;
				std::unordered_set<unsigned int> bag;
				// above
				neighborR = thisSiteR == internalSiteStartRow ? externalOutputSiteEndRow : thisSiteR - 1;
				neighborC = thisSiteC;
				bag.insert((neighborR * cols) + neighborC);
				// above right
				neighborR = thisSiteR == internalSiteStartRow ? externalOutputSiteEndRow : thisSiteR - 1;
				neighborC = thisSiteC == cols - 1 ? 0 : thisSiteC + 1;
				bag.insert((neighborR * cols) + neighborC);
				// right
				neighborR = thisSiteR;
				neighborC = thisSiteC == cols - 1 ? 0 : thisSiteC + 1;
				bag.insert((neighborR * cols) + neighborC);
				// below right
				neighborR = thisSiteR == externalOutputSiteEndRow ? internalSiteStartRow : thisSiteR + 1;
				neighborC = thisSiteC == cols - 1 ? 0 : thisSiteC + 1;
				bag.insert((neighborR * cols) + neighborC);
				// below
				neighborR = thisSiteR == externalOutputSiteEndRow ? internalSiteStartRow : thisSiteR + 1;
				neighborC = thisSiteC;
				bag.insert((neighborR * cols) + neighborC);
				// below left
				neighborR = thisSiteR == externalOutputSiteEndRow ? internalSiteStartRow : thisSiteR + 1;
				neighborC = thisSiteC == 0 ? cols - 1 : thisSiteC - 1;
				bag.insert((neighborR * cols) + neighborC);
				// left
				neighborR = thisSiteR;
				neighborC = thisSiteC == 0 ? cols - 1 : thisSiteC - 1;
				bag.insert((neighborR * cols) + neighborC);
				// above left
				neighborR = thisSiteR == internalSiteStartRow ? externalOutputSiteEndRow : thisSiteR - 1;
				neighborC = thisSiteC == 0 ? cols - 1 : thisSiteC - 1;
				bag.insert((neighborR * cols) + neighborC);

				// assign k inputs randomly by drawing from the bag of neighbors
				for (unsigned int k = 0; k < connectivity; k++) {
					uniform_int_distribution<unsigned int> randNeighborDist(0, bag.size() - 1);
					std::unordered_set<unsigned int>::iterator it = bag.begin();
					unsigned int inputSiteBagIndex = randNeighborDist(rnGen);
					for (unsigned int z = 0; z < inputSiteBagIndex; z++) {
						it++;
					}
					unsigned int inputSite = *it;

					// create synapse connection, store in the master list of synapse connections
					NeuralSynapse sc(inputSite, s->siteId, initialConnectionColor, 0.0f, neuronSites[inputSite].freshActivation);
					neuralSynapses[neuralSynapsesIndex] = sc;

					// store a reference to this synapse connection in this site's list of input synapse connections
					vector<NeuralSynapse>::iterator scIt = neuralSynapses.begin() + neuralSynapsesIndex;
					s->neuralSynapses.push_back((NeuralSynapse*)&(*scIt));
					neuralSynapsesIndex++;

					// remove from bag
					bag.erase(it);
				}
			}
		}
	}
	else
	{
		// initialize site connectivity, arbitrary sites
		uniform_int_distribution<unsigned int> randInputDist(0, externalOutputStartCellIndex - 1);
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
				NeuralSynapse sc(inputSite, s->siteId, initialConnectionColor, 0.0f, neuronSites[inputSite].freshActivation);
				neuralSynapses[neuralSynapsesIndex] = sc;

				// store a reference to this synapse connection in this site's list of input synapse connections
				vector<NeuralSynapse>::iterator scIt = neuralSynapses.begin() + neuralSynapsesIndex;
				s->neuralSynapses.push_back((NeuralSynapse*)&(*scIt));
				neuralSynapsesIndex++;
			}
		}
	}
}

bool NeuralNetworkModule::setConnectivity(unsigned int connectivity) {
	if (connectivity <= 100) {
		if (neighborhoodConnections && connectivity > 8) {
			cout << "connectivity cannot be set > 8 when neighborhood connections is on, ignoring" << endl;
			return false;
		}

		this->connectivity = connectivity;
		resetCellStates();
		printConfigurationState();
	}
	else {
		cout << "connectivity out of bounds, ignoring" << endl;
		return false;
	}

	return true;
}

void NeuralNetworkModule::setNeighborhoodConnections(bool neighborhoodConnections) {
	if (this->neighborhoodConnections == neighborhoodConnections) {
		cout << "force neighborhood connections is already set to " << (neighborhoodConnections ? "true" : "false") << ", ignoring" << endl;
		return;
	}

	if (connectivity > 8) {
		cout << "connectivity must be <= 8 to turn on neighborhood connections, ignoring" << endl;
		return;
	}

	this->neighborhoodConnections = neighborhoodConnections;
	resetCellStates();
	printConfigurationState();
}

void NeuralNetworkModule::iterate()
{
	// process any requested config changes
	processConfigChangeQueue();

	// update logical state

	if (autoFeedForward)
		feedForward();

	if (autoNewInput)
		this->updateInputSites();

	double globalAverageFiringThresholdTotal = 0.0f;
	double globalAverageInputWeightTotal = 0.0f;

	// synchronous read for current state of sites
	unsigned int siteCount = neuronSites.size();
	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		NeuronSite* s = &neuronSites[i];

		// sum synapse weights that fed input to this neurons
		float totalInputWeight = 0.0f;
		vector<NeuralSynapse*>* inputNeuralSynapses = (vector<NeuralSynapse*>*) &s->neuralSynapses;
		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {
			NeuralSynapse* inputSynapse = inputNeuralSynapses->at(inputSiteIndex);
			if (neuronSites[inputSynapse->sourceSiteId].currentState)
				totalInputWeight += inputSynapse->connectionStrengthWeight;
		}

		// keep running total for global synapse weight average
		globalAverageInputWeightTotal += totalInputWeight;

		// fire neuron if total input weight exceeds firing threshold
		bool newState = totalInputWeight >= s->firingThreshold;
		float synapseWeightAdjustment = -synapseWeightAdjustmentDelta;
		if (newState){
			s->firingRateCalcActivations++;
			synapseWeightAdjustment = synapseWeightAdjustmentDelta;
			globalFiringRateCalcActivations++;
		}

		// Update synapse weights.  Synapses that contibuted to neuron firing are strengthend (weight increased),
		// those that did not are weakened (weight decresed).
		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {
			NeuralSynapse* inputSynapse = inputNeuralSynapses->at(inputSiteIndex);
			inputSynapse->connectionStrengthWeight += synapseWeightAdjustment;
			if (inputSynapse->connectionStrengthWeight > maxSynapseWeight)
				inputSynapse->connectionStrengthWeight = maxSynapseWeight;
			else if (inputSynapse->connectionStrengthWeight < minSynapseWeight)
				inputSynapse->connectionStrengthWeight = minSynapseWeight;
		}

		// refresh this neuron's firing rate
		if(iteration > 0 && iteration % firingRateSampleIterations == 0){
			s->firingRate = s->firingRateCalcActivations / (float) firingRateSampleIterations;
			s->firingRateCalcActivations = 0;
		}

		// Adjust this neuron's firing threshold according to it's current firing rate.  If it's firing rate is
		// above the global firing rate taget, increase the threshold it takes to fire the neuron.  Else if it's
		// firing rate is below the global firing rate target, decrease the threshold it takes to fire the neuron.
		if (s->firingRate > targetFiringRate)
			s->firingThreshold += firingRateThresholdAdjustmentDelta;
		else if (s->firingRate < targetFiringRate)
			s->firingThreshold -= firingRateThresholdAdjustmentDelta;

		// keep running total for global firing rate calculation
		globalAverageFiringThresholdTotal += s->firingThreshold;

		// update working state
		s->workingState = newState;
	}

	// calculate global average neuron firing threshold and synapse input weights from the collected values
	globalAverageFiringThreshold = globalAverageFiringThresholdTotal / (externalOutputEndCellIndex - internalStartCellIndex);
	globalAverageInputWeight = globalAverageInputWeightTotal / (externalOutputEndCellIndex - internalStartCellIndex);


	// refresh the current global firing rate
	if (iteration > 0 && iteration % firingRateSampleIterations == 0) {
		globalFiringRate = globalFiringRateCalcActivations / (float) firingRateSampleIterations;
		globalFiringRateCalcActivations = 0;
	}
	
	// site updates and calculate overall state checksum
	boost::crc_32_type crcResult;
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

			if (fadeInactiveSites) {
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

		// update site synapse connection render state
		vector<NeuralSynapse*>* inputNeuralSynapses = (vector<NeuralSynapse*>*) &s->neuralSynapses;
		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {
			NeuralSynapse* inputNeuralSynapse = inputNeuralSynapses->at(inputSiteIndex);
			inputNeuralSynapse->shouldRender = neuronSites[inputNeuralSynapse->sourceSiteId].freshActivation;
		}

		// apply the working state
		s->currentState = s->workingState;

		// add relevent state values to running checksum bytes
		//crcResult.process_bytes(&s->siteId, SIZE_SITE_ID);
		//crcResult.process_bytes(&s->currentState, SIZE_CURRENT_STATE);
		crcResult.process_byte(s->currentState);
		//crcResult.process_bytes(&s->workingState, SIZE_WORKING_STATE);
		//crcResult.process_bytes(&s->booleanFunctionId, SIZE_BOOLEAN_FUNCTION_ID);
		//crcResult.process_bytes(s->inputSiteIds.data(), s->inputSiteIds.size() * SIZE_SITE_ID);
		//crcResult.process_bytes(s->outputSiteIds.data(), s->outputSiteIds.size() * SIZE_SITE_ID);
	}
	unsigned int crcResultChecksum = crcResult.checksum();

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
		updateInputSites();
		renderComplete = false;
	}
	*/
}

void NeuralNetworkModule::incrementExternalInputRows() {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (internalRowCount == 0)
	{
		cout << "cannot add more external input rows, no room left.  Ignoring." << endl;
		return;
	}

	initialize(
		this->rows,
		this->cols,
		this->connectivity,
		this->externalInputRowCount + 1,
		this->feedbackInputRowCount,
		this->externalOutputRowCount,
		this->neighborhoodConnections,
		this->autoFeedForward
		);
}

void NeuralNetworkModule::decrementExternalInputRows() {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (externalInputRowCount == 1)
	{
		cout << "no more external input rows to remove, ignoring." << endl;
		return;
	}

	initialize(
		this->rows,
		this->cols,
		this->connectivity,
		this->externalInputRowCount - 1,
		this->feedbackInputRowCount,
		this->externalOutputRowCount,
		this->neighborhoodConnections,
		this->autoFeedForward
		);
}

void NeuralNetworkModule::incrementFeedbackInputRows() {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (internalRowCount == 1)
	{
		cout << "cannot add more feedback input rows, no room left.  Ignoring." << endl;
		return;
	}

	initialize(
		this->rows,
		this->cols,
		this->connectivity,
		this->externalInputRowCount,
		this->feedbackInputRowCount + 1,
		this->externalOutputRowCount,
		this->neighborhoodConnections,
		this->autoFeedForward
		);
}

void NeuralNetworkModule::decrementFeedbackInputRows() {
	if (feedbackInputRowCount == 0)
	{
		cout << "no more feedback input rows to remove, ignoring." << endl;
		return;
	}

	initialize(
		this->rows,
		this->cols,
		this->connectivity,
		this->externalInputRowCount,
		this->feedbackInputRowCount - 1,
		this->externalOutputRowCount,
		this->neighborhoodConnections,
		this->autoFeedForward
		);
}

void NeuralNetworkModule::incrementExternalOutputRows() {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (internalRowCount == 1)
	{
		cout << "cannot add more external output rows, no room left.  Ignoring." << endl;
		return;
	}

	initialize(
		this->rows,
		this->cols,
		this->connectivity,
		this->externalInputRowCount,
		this->feedbackInputRowCount,
		this->externalOutputRowCount + 1,
		this->neighborhoodConnections,
		this->autoFeedForward
		);
}

void NeuralNetworkModule::decrementExternalOutputRows() {
	unsigned int internalRowCount = (internalEndCellIndex - internalStartCellIndex) / cols;
	if (externalOutputRowCount == 0)
	{
		cout << "no more external output rows to remove, ignoring." << endl;
		return;
	}

	initialize(
		this->rows,
		this->cols,
		this->connectivity,
		this->externalInputRowCount,
		this->feedbackInputRowCount,
		this->externalOutputRowCount - 1,
		this->neighborhoodConnections,
		this->autoFeedForward
		);
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

void NeuralNetworkModule::toggleAutoFeedForward() {
	autoFeedForward = !autoFeedForward;
	printConfigurationState();
}

void NeuralNetworkModule::toggleAutoNewInput() {
	autoNewInput = !autoNewInput;
	printConfigurationState();
}

void NeuralNetworkModule::toggleFadeInactiveSites() {
	fadeInactiveSites = !fadeInactiveSites;
}

inline bool NeuralNetworkModule::getSiteActive(unsigned int siteId) {
	return neuronSites[siteId].currentState;
}

inline std::vector<float>* NeuralNetworkModule::getSiteColor(unsigned int siteId) {
	return &neuronSites[siteId].color;
}

inline unsigned int NeuralNetworkModule::getMaxSiteConnectionsCount() {
	return connectivity * rows * cols;
}

inline std::vector<SiteConnection*>* NeuralNetworkModule::getSiteConnections(unsigned int siteId) {
	return (std::vector<SiteConnection*>*) &neuronSites[siteId].neuralSynapses;
}

void NeuralNetworkModule::getConfigJson(Json::Value& configJson) {
	configJson.clear();
	configJson["rows"] = rows;
	configJson["columns"] = cols;
	configJson["connectivity"] = connectivity;
	configJson["externalInputRowCount"] = externalInputRowCount;
	configJson["feedbackInputRowCount"] = feedbackInputRowCount;
	configJson["externalOutputRowCount"] = externalOutputRowCount;
	configJson["neighborhoodConnections"] = neighborhoodConnections;
	configJson["autoFeedForward"] = autoFeedForward;
	configJson["autoNewInput"] = autoNewInput;
	configJson["targetFiringRate"] = targetFiringRate;
	configJson["initialNeuronFiringThreshold"] = initialNeuronFiringThreshold;
	configJson["firingRateSampleIterations"] = firingRateSampleIterations;
	configJson["firingRateThresholdAdjustmentDelta"] = firingRateThresholdAdjustmentDelta;
	configJson["synapseWeightAdjustmentDelta"] = synapseWeightAdjustmentDelta;
	configJson["minSynapseWeight"] = minSynapseWeight;
	configJson["maxSynapseWeight"] = maxSynapseWeight;
}

void NeuralNetworkModule::getStateJson(Json::Value& stateJson) {
	stateJson.clear();
	stateJson["iteration"] = iteration;
	stateJson["renderComplete"] = renderComplete;
	stateJson["globalFiringRate"] = globalFiringRate;
	stateJson["globalFiringRateCalcActivations"] = globalFiringRateCalcActivations;
	stateJson["globalAverageFiringThreshold"] = globalAverageFiringThreshold;
	stateJson["globalAverageInputWeight"] = globalAverageInputWeight;
}

bool NeuralNetworkModule::setConfigurationValue(const ConfigSetting& setting) {
	if (setting.key == "connectivity") {
		return setConnectivity(stoul(setting.value));
	}
	else if (setting.key == "externalInputRowCount") {
	}
	else if (setting.key == "feedbackInputRowCount") {
	}
	else if (setting.key == "externalOutputRowCount") {
	}
	else if (setting.key == "neighborhoodConnections") {
		setNeighborhoodConnections(setting.value == "true" ? true : false);
		return true;
	}
	else if (setting.key == "autoFeedForward") {
		toggleAutoFeedForward();
		return true;
	}
	else if (setting.key == "autoNewInput") {
	}
	else if (setting.key == "targetFiringRate") {
	}
	else if (setting.key == "initialNeuronFiringThreshold") {
	}
	else if (setting.key == "firingRateSampleIterations") {
	}
	else if (setting.key == "firingRateThresholdAdjustmentDelta") {
	}
	else if (setting.key == "synapseWeightAdjustmentDelta") {
	}
	else if (setting.key == "minSynapseWeight") {
	}
	else if (setting.key == "maxSynapseWeight") {
	}
	else if (setting.key == "specificInputPattern") {
		setSpecificInputPattern(stoul(setting.value));
		return true;
	}

	return false;
}

inline unsigned int NeuralNetworkModule::getConnectivity() {
	return connectivity;
}

void NeuralNetworkModule::printConfigurationState() {
	cout << "--------------------------------------------------------" << endl
		<< "Initializing Neural Network:" << endl
		<< "rows                               = " << std::to_string(rows) << endl
		<< "cols                               = " << std::to_string(cols) << endl
		<< "connectivity                       = " << std::to_string(connectivity) << endl
		<< "externalInputRowCount              = " << std::to_string(externalInputRowCount) << endl
		<< "feedbackInputRowCount              = " << std::to_string(feedbackInputRowCount) << endl
		<< "externalOutputRowCount             = " << std::to_string(externalOutputRowCount) << endl
		<< "neighborhoodConnections            = " << (neighborhoodConnections ? "true" : "false") << endl
		<< "autoFeedForward                    = " << (autoFeedForward ? "true" : "false") << endl
		<< "autoNewInput                       = " << (autoNewInput ? "true" : "false") << endl
		<< "targetFiringRate                   = " << std::to_string(targetFiringRate) << endl
		<< "initialNeuronFiringThreshold       = " << std::to_string(initialNeuronFiringThreshold) << endl
		<< "firingRateSampleIterations         = " << std::to_string(firingRateSampleIterations) << endl
		<< "firingRateThresholdAdjustmentDelta = " << std::to_string(firingRateThresholdAdjustmentDelta) << endl
		<< "synapseWeightAdjustmentDelta       = " << std::to_string(synapseWeightAdjustmentDelta) << endl
		<< "minSynapseWeight                   = " << std::to_string(minSynapseWeight) << endl
		<< "maxSynapseWeight                   = " << std::to_string(maxSynapseWeight) << endl
		<< endl;
}

void NeuralNetworkModule::printWorkingState() {
	cout << "--------------------------------------------------------" << endl
		<< "Neural Network Working State:" << endl
		<< "iteration                       = " << std::to_string(iteration) << endl
		<< "renderComplete                  = " << (renderComplete ? "true" : "false") << endl
		<< "globalFiringRate                = " << std::to_string(globalFiringRate) << endl
		<< "globalFiringRateCalcActivations = " << std::to_string(globalFiringRateCalcActivations) << endl
		<< "globalAverageFiringThreshold    = " << std::to_string(globalAverageFiringThreshold) << endl
		<< "globalAverageInputWeight        = " << std::to_string(globalAverageInputWeight) << endl
		<< "checkSums size                  = " << std::to_string(checkSums.size()) << endl
		<< endl;
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

void NeuralNetworkModule::cleanUp()
{
	neuronSites.clear();
	neuralSynapses.clear();
	checkSums.clear();
}

void NeuralNetworkModule::handleInputAction(int action, int key) {

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_SPACE:
			resetCellStates();
			break;
		case GLFW_KEY_R:
			updateInputSites();
			break;
		case GLFW_KEY_F:
			feedForward();
			break;
		case GLFW_KEY_T:
			toggleAutoFeedForward();
			break;
		case GLFW_KEY_A:
			toggleAutoNewInput();
			break;
		case GLFW_KEY_UP:
			setConnectivity(connectivity + 1);
			break;
		case GLFW_KEY_DOWN:
			setConnectivity(connectivity - 1);
			break;
		case GLFW_KEY_LEFT:
			setNeighborhoodConnections(false);
			break;
		case GLFW_KEY_RIGHT:
			setNeighborhoodConnections(true);
			break;
		case GLFW_KEY_KP_1:
			decrementExternalInputRows();
			break;
		case GLFW_KEY_KP_7:
			incrementExternalInputRows();
			break;
		case GLFW_KEY_KP_2:
			decrementFeedbackInputRows();
			break;
		case GLFW_KEY_KP_8:
			incrementFeedbackInputRows();
			break;
		case GLFW_KEY_KP_3:
			decrementExternalOutputRows();
			break;
		case GLFW_KEY_KP_9:
			incrementExternalOutputRows();
			break;
		case GLFW_KEY_INSERT:
			shiftInputData(0);
			break;
		case GLFW_KEY_I:
			toggleFadeInactiveSites();
			break;
		case GLFW_KEY_Z:
			printWorkingState();
			break;
		}
	}
}

NeuralNetworkModule::~NeuralNetworkModule()
{
	cleanUp();
}
