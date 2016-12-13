#include "RandomBooleanNetworkModule.hpp"

using namespace std;

RandomBooleanNetworkModule::RandomBooleanNetworkModule(
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

void RandomBooleanNetworkModule::initialize(
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

	externalInputStartCellIndex = 0;
	externalInputEndCellIndex = externalInputStartCellIndex + (externalInputRowCount * cols) - 1;
	feedbackInputStartCellIndex = externalInputEndCellIndex + 1;
	feedbackInputEndCellIndex = feedbackInputStartCellIndex + (feedbackInputRowCount * cols) - 1;
	internalStartCellIndex = feedbackInputEndCellIndex + 1;
	internalEndCellIndex = internalStartCellIndex + ((rows - (externalInputRowCount + feedbackInputRowCount + externalOutputRowCount)) * cols) - 1;
	externalOutputStartCellIndex = internalEndCellIndex + 1;
	externalOutputEndCellIndex = externalOutputStartCellIndex + (externalOutputRowCount * cols) - 1;

	resetCellStates();

	printConfigurationState();
}

void RandomBooleanNetworkModule::updateInputSites()
{
	bernoulli_distribution initialStateRandomDist(0.50);
	for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
		RandomBooleanNetworkSite* s = &sites[i];
		s->currentState = initialStateRandomDist(rnGen);
	}
	checkSums.clear();
}

void RandomBooleanNetworkModule::shiftInputData(int offset) {
	bool t1;
	bool t2 = sites[externalInputEndCellIndex].currentState == 1;

	for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
		RandomBooleanNetworkSite* s = &sites[i];
		t1 = s->currentState == 1;
		s->currentState = t2 == 1;
		t2 = t1;
	}

	checkSums.clear();
}

void RandomBooleanNetworkModule::resetCellStates()
{
	iteration = 0;
	cleanUp();

	// initialize sites
	unsigned int inputCombinations = (unsigned int)pow(2, connectivity);
	unsigned int booleanFunctionCount = (unsigned int)pow(2, inputCombinations);
	unsigned int siteCount = rows * cols;
	sites.resize(siteCount);
	siteConnections.resize(siteCount * connectivity);
	unsigned int siteConnectionIndex = 0;
	bernoulli_distribution initialStateRandomDist(0.50);
	uniform_int_distribution<unsigned int> randBooleanFunctionDist(0, booleanFunctionCount - 1);
	vector<float> initialConnectionColor(4);
	initialConnectionColor[0] = 0.0f;
	initialConnectionColor[1] = 1.0f;
	initialConnectionColor[2] = 0.0f;
	initialConnectionColor[3] = 0.75f;

	for (unsigned int i = 0; i < siteCount; i++) {

		RandomBooleanNetworkSite s;
		s.siteId = i;

		// choose random initial state
		s.currentState = initialStateRandomDist(rnGen);
		s.freshActivation = false;
		s.workingState = false;
		s.stateChangeCount = 0;

		s.color.resize(3);
		if (i >= externalInputStartCellIndex && i <= externalInputEndCellIndex) {  // external input site, blue, no boolean function
			s.color[0] = 0.0f;
			s.color[1] = 0.0f;
			s.color[2] = 1.0f;
		}
		else if (i >= feedbackInputStartCellIndex && i <= feedbackInputEndCellIndex) {  // feedback input site, green, no boolean function
			s.color[0] = 0.0f;
			s.color[1] = 1.0f;
			s.color[2] = 0.0f;
		}
		else if (i >= externalOutputStartCellIndex && i <= externalOutputEndCellIndex) { // external output site, red
																						 // choose random boolean function
			s.booleanFunctionId = randBooleanFunctionDist(rnGen);

			// color red
			s.color[0] = 1.0f;
			s.color[1] = 0.0f;
			s.color[2] = 0.0f;
		}
		else {  // internal site
				// choose random boolean function
			s.booleanFunctionId = randBooleanFunctionDist(rnGen);

			// color black
			s.color[0] = 0.0f;
			s.color[1] = 0.0f;
			s.color[2] = 0.0f;
			s.freshActivation = s.currentState == 1;
		}

		sites[i] = s;
	}

	if (neighborhoodConnections)
	{
		unsigned int internalSiteStartRow = internalStartCellIndex / cols;
		unsigned int externalOutputSiteEndRow = externalOutputEndCellIndex / cols;

		// neighbor connectivity
		if (connectivity <= 4) // von Neumann neighborhood (k <= 4)
		{
			for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {

				RandomBooleanNetworkSite* s = &sites[i];

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

					// create connection, store in the master list of connections
					SiteConnection sc(inputSite, s->siteId, initialConnectionColor, sites[inputSite].freshActivation);
					siteConnections[siteConnectionIndex] = sc;

					// store a reference to this connection in this site's list of input connections
					vector<SiteConnection>::iterator scIt = siteConnections.begin() + siteConnectionIndex;
					s->siteConnections.push_back((SiteConnection*)&(*scIt));
					siteConnectionIndex++;

					// remove from bag
					bag.erase(it);
				}
			}
		}
		else  // Moore neighborhood
		{
			for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {

				RandomBooleanNetworkSite* s = &sites[i];

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

					// create connection, store in the master list of connections
					SiteConnection sc(inputSite, s->siteId, initialConnectionColor, sites[inputSite].freshActivation);
					siteConnections[siteConnectionIndex] = sc;

					// store a reference to this connection in this site's list of input connections
					vector<SiteConnection>::iterator scIt = siteConnections.begin() + siteConnectionIndex;
					s->siteConnections.push_back((SiteConnection*)&(*scIt));
					siteConnectionIndex++;

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
			RandomBooleanNetworkSite* s = &sites[i];
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
					else{
						// make sure this input site has not already been chosen as an input for this site
						for (unsigned int l = 0; l < s->siteConnections.size(); l++) {
							if (s->siteConnections[l]->sourceSiteId == inputSite){
								validSelection = false;
								break;
							}
						}
					}
				} while (!validSelection);

				// create connection, store in the master list of connections
				SiteConnection sc(inputSite, s->siteId, initialConnectionColor, sites[inputSite].freshActivation);
				siteConnections[siteConnectionIndex] = sc;

				// store a reference to this connection in this site's list of input connections
				vector<SiteConnection>::iterator scIt = siteConnections.begin() + siteConnectionIndex;
				s->siteConnections.push_back((SiteConnection*)&(*scIt));
				siteConnectionIndex++;
			}
		}
	}
}

bool RandomBooleanNetworkModule::setConnectivity(unsigned int connectivity) {
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

void RandomBooleanNetworkModule::setNeighborhoodConnections(bool neighborhoodConnections) {
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

void RandomBooleanNetworkModule::iterate()
{
	// update logical state

	if (autoFeedForward)
		feedForward();

	if (autoNewInput)
		this->updateInputSites();

	unsigned int siteCount = sites.size();
	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		RandomBooleanNetworkSite* s = &sites[i];

		// build inputs up into a single byte
		char inputValue = 0;
		vector<SiteConnection*>* inputSiteConnections = (vector<SiteConnection*>*) &s->siteConnections;

		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {
			bool inputSiteValue = sites[inputSiteConnections->at(inputSiteIndex)->sourceSiteId].currentState == 1;
			if (inputSiteValue)
				inputValue = inputValue | ((char) 1 << inputSiteIndex);
		}

		// check input byte against function
		char booleanFunction = s->booleanFunctionId;
		char checkingBit = 1;
		if (inputValue)
			checkingBit = checkingBit << (inputValue - 1);
		bool newState = (booleanFunction & checkingBit) != 0;

		// update working state
		s->workingState = newState;
	}

	// push working state to new current state and calculate overall state checksum
	boost::crc_32_type crcResult;
	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		RandomBooleanNetworkSite* s = &sites[i];

		if (s->currentState != s->workingState) {

			// new state is different from the current state, increment the change counter and reset it to a solid color
			s->stateChangeCount++;
			s->freshActivation = s->workingState == 1;

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

		// update site connection render state
		vector<SiteConnection*>* inputSiteConnections = (vector<SiteConnection*>*) &s->siteConnections;
		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {
			SiteConnection* inputSiteConnection = inputSiteConnections->at(inputSiteIndex);
			inputSiteConnection->shouldRender = sites[inputSiteConnection->sourceSiteId].freshActivation;
		}

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
	std::set<unsigned int>::iterator csi = checkSums.find(crcResultChecksum);
	if (csi == checkSums.end()) {
		checkSums.insert(crcResultChecksum);
	}
	else {
		cout << "Cycle detected on iteration " << std::to_string(iteration) << endl;
		renderComplete = true;
	}

	if(renderComplete){
		updateInputSites();
		renderComplete = false;
	}
}

void RandomBooleanNetworkModule::incrementExternalInputRows() {
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

void RandomBooleanNetworkModule::decrementExternalInputRows() {
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

void RandomBooleanNetworkModule::incrementFeedbackInputRows() {
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

void RandomBooleanNetworkModule::decrementFeedbackInputRows() {
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

void RandomBooleanNetworkModule::incrementExternalOutputRows() {
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

void RandomBooleanNetworkModule::decrementExternalOutputRows() {
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

void RandomBooleanNetworkModule::feedForward() {
	// Copy feedbackInputRowCount rows from the external output into the feedback input sites.
	unsigned int outputSiteIndex = externalOutputStartCellIndex;
	for (unsigned int i = feedbackInputStartCellIndex; i <= feedbackInputEndCellIndex && outputSiteIndex <= externalOutputEndCellIndex; i++) {
		RandomBooleanNetworkSite* feedbackSite = &sites[i];
		RandomBooleanNetworkSite* outputSite = &sites[outputSiteIndex++];
		feedbackSite->currentState = outputSite->currentState;
	}
}

void RandomBooleanNetworkModule::toggleAutoFeedForward() {
	autoFeedForward = !autoFeedForward;
	printConfigurationState();
}

void RandomBooleanNetworkModule::toggleAutoNewInput() {
	autoNewInput = !autoNewInput;
	printConfigurationState();
}

void RandomBooleanNetworkModule::toggleFadeInactiveSites() {
	fadeInactiveSites = !fadeInactiveSites;
}

inline unsigned char RandomBooleanNetworkModule::getSiteState(unsigned int siteId) {
	return sites[siteId].currentState;
}

inline std::vector<float>* RandomBooleanNetworkModule::getSiteColor(unsigned int siteId) {
	return &sites[siteId].color;
}

inline unsigned int RandomBooleanNetworkModule::getMaxSiteConnectionsCount() {
	return connectivity * rows * cols;
}

inline std::vector<SiteConnection*>* RandomBooleanNetworkModule::getSiteConnections(unsigned int siteId) {
	return (std::vector<SiteConnection*>*) &sites[siteId].siteConnections;
}

inline unsigned int RandomBooleanNetworkModule::getConnectivity() {
	return connectivity;
}

void RandomBooleanNetworkModule::printConfigurationState() {
	cout << "--------------------------------------------------------" << endl
		<< "Initializing RBN:" << endl
		<< "rows                    = " << std::to_string(rows) << endl
		<< "cols                    = " << std::to_string(cols) << endl
		<< "connectivity            = " << std::to_string(connectivity) << endl
		<< "externalInputRowCount   = " << std::to_string(externalInputRowCount) << endl
		<< "feedbackInputRowCount   = " << std::to_string(feedbackInputRowCount) << endl
		<< "externalOutputRowCount  = " << std::to_string(externalOutputRowCount) << endl
		<< "neighborhoodConnections = " << (neighborhoodConnections ? "true" : "false") << endl
		<< "autoFeedForward         = " << (autoFeedForward ? "true" : "false") << endl
		<< "autoNewInput            = " << (autoNewInput ? "true" : "false") << endl
		<< endl;
}

void RandomBooleanNetworkModule::cleanUp()
{
	sites.clear();
	siteConnections.clear();
	checkSums.clear();
}

void RandomBooleanNetworkModule::handleInputAction(int action, int key) {

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
		}
	}
}

RandomBooleanNetworkModule::~RandomBooleanNetworkModule()
{
	cleanUp();
}
