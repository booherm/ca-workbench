#include "RandomBooleanNetwork.hpp"

using namespace std;

RandomBooleanNetwork::RandomBooleanNetwork(
	unsigned int rows,
	unsigned int cols,
	unsigned int connectivity,
	unsigned int externalInputRowCount,
	unsigned int feedbackInputRowCount,
	unsigned int externalOutputRowCount,
	bool neighborhoodConnections
) {
	// initialize random number generator
	rnGen.seed(random_device()());
	initialize(
		rows,
		cols,
		connectivity,
		externalInputRowCount,
		feedbackInputRowCount,
		externalOutputRowCount,
		neighborhoodConnections
	);
}

void RandomBooleanNetwork::initialize(
	unsigned int rows,
	unsigned int cols,
	unsigned int connectivity,
	unsigned int externalInputRowCount,
	unsigned int feedbackInputRowCount,
	unsigned int externalOutputRowCount,
	bool neighborhoodConnections
) {
	this->rows = rows;
	this->cols = cols;
	this->connectivity = connectivity;
	this->externalInputRowCount = externalInputRowCount;
	this->feedbackInputRowCount = feedbackInputRowCount;
	this->externalOutputRowCount = externalOutputRowCount;
	this->neighborhoodConnections = neighborhoodConnections;

	externalInputStartCellIndex = 0;
	externalInputEndCellIndex = externalInputStartCellIndex + (externalInputRowCount * cols) - 1;
	feedbackInputStartCellIndex = externalInputEndCellIndex + 1;
	feedbackInputEndCellIndex = feedbackInputStartCellIndex + (feedbackInputRowCount * cols) - 1;
	internalStartCellIndex = feedbackInputEndCellIndex + 1;
	internalEndCellIndex = internalStartCellIndex + ((rows - (externalInputRowCount + feedbackInputRowCount + externalOutputRowCount)) * cols) - 1;
	externalOutputStartCellIndex = internalEndCellIndex + 1;
	externalOutputEndCellIndex = externalOutputStartCellIndex + (externalOutputRowCount * cols) - 1;

	initialized = false;
	resetCellStates();
	initialized = true;

	printConfigurationState();
}

void RandomBooleanNetwork::updateInputSites()
{
	bernoulli_distribution initialStateRandomDist(0.50);
	for (unsigned int i = externalInputStartCellIndex; i <= externalInputEndCellIndex; i++) {
		Site* s = &sites[i];
		s->currentState = initialStateRandomDist(rnGen);
	}
	checkSums.clear();
}

void RandomBooleanNetwork::resetCellStates()
{
	iteration = 0;
	cleanUp();

	// initialize sites
	unsigned int inputCombinations = (unsigned int)pow(2, connectivity);
	unsigned int booleanFunctionCount = (unsigned int)pow(2, inputCombinations);
	unsigned int siteCount = rows * cols;
	sites.resize(siteCount);
	bernoulli_distribution initialStateRandomDist(0.50);
	uniform_int_distribution<unsigned int> randBooleanFunctionDist(0, booleanFunctionCount - 1);

	for (unsigned int i = 0; i < siteCount; i++) {

		Site s;
		s.siteId = i;

		// choose random initial state
		s.currentState = initialStateRandomDist(rnGen);
		s.workingState = false;

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
				//cout << "processing site " << std::to_string(i) << endl;
				Site* s = &sites[i];

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
					s->inputSiteIds.push_back(inputSite);

					// add this site to the input site's output
					sites[inputSite].outputSiteIds.push_back(s->siteId);

					// remove from bag
					bag.erase(it);
				}
			}
		}
		else  // Moore neighborhood
		{
			for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
				//cout << "processing site " << std::to_string(i) << endl;
				Site* s = &sites[i];

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
					s->inputSiteIds.push_back(inputSite);

					// add this site to the input site's output
					sites[inputSite].outputSiteIds.push_back(s->siteId);

					// remove from bag
					bag.erase(it);
				}
			}
		}
	}
	else
	{
		// initialize site connectivity, arbitrary sites
		uniform_int_distribution<unsigned int> randInputDist(0, siteCount - 1);
		for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
			Site* s = &sites[i];

			// assign k inputs randomly
			for (unsigned int k = 0; k < connectivity; k++) {
				// pick random site, not self, not one that was already chosen as an input site, not an external output site
				unsigned int inputSite;
				bool alreadyUsed;

				unsigned int attempt = 0;
				do {
					//if (attempt > 0)
//						cout << "extraneous effort" << endl;
					inputSite = randInputDist(rnGen);
					alreadyUsed = find(s->inputSiteIds.begin(), s->inputSiteIds.end(), inputSite) != s->inputSiteIds.end();
					attempt++;
				} while (inputSite == s->siteId || alreadyUsed);
				s->inputSiteIds.push_back(inputSite);

				// add this site to the input site's output
				sites[inputSite].outputSiteIds.push_back(s->siteId);
			}
		}
	}


	//cout << "connectivity init complete" << endl;
	
	/*
	// print state
	for (unsigned int i = 0; i < sites.size(); i++)
	{
		Site s = sites[i];
		//cout << siteToJson(s) << endl;
		cout << std::to_string(siteCrc32(s)) << endl;
	}
	*/
}

void RandomBooleanNetwork::setConnectivity(unsigned int connectivity) {

	if(connectivity <= 100){
		if (neighborhoodConnections && connectivity > 8) {
			cout << "connectivity cannot be set > 8 when neighborhood connections is on, ignoring" << endl;
			return;
		}

		this->connectivity = connectivity;
		resetCellStates();
		printConfigurationState();
	}
	else {
		cout << "connectivity out of bounds, ignoring" << endl;
	}
}

void RandomBooleanNetwork::setNeighborhoodConnections(bool neighborhoodConnections) {
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

bool RandomBooleanNetwork::iterate()
{
	// update logical state
	//cout << "begin iteration " << std::to_string(iteration) << endl;
	
	unsigned int siteCount = sites.size();

	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		Site* s = &sites[i];

		// build inputs up into a single byte
		char inputValue = 0;
		for (unsigned int inputSiteIndex = 0; inputSiteIndex < connectivity; inputSiteIndex++) {
			bool inputSiteValue = sites[s->inputSiteIds[inputSiteIndex]].currentState;
			if (inputSiteValue)
				inputValue = inputValue | ((char) 1 << inputSiteIndex);
		}

		// check input byte against function
		char booleanFunction = s->booleanFunctionId;
		char checkingBit = 1;
		if(inputValue)
			checkingBit = checkingBit << (inputValue - 1);
		bool newState = (booleanFunction & checkingBit) != 0;

		// update working state
		s->workingState = newState;
	}

	// push working state to new current state and calculate overall state checksum
	boost::crc_32_type crcResult;
	for (unsigned int i = internalStartCellIndex; i <= externalOutputEndCellIndex; i++) {
		Site* s = &sites[i];
		s->currentState = s->workingState;

		//crcResult.process_bytes(&s->siteId, SIZE_SITE_ID);
		//crcResult.process_bytes(&s->currentState, SIZE_CURRENT_STATE);
		crcResult.process_byte(s->currentState);
		//crcResult.process_bytes(&s->workingState, SIZE_WORKING_STATE);
		//crcResult.process_bytes(&s->booleanFunctionId, SIZE_BOOLEAN_FUNCTION_ID);
		//crcResult.process_bytes(s->inputSiteIds.data(), s->inputSiteIds.size() * SIZE_SITE_ID);
		//crcResult.process_bytes(s->outputSiteIds.data(), s->outputSiteIds.size() * SIZE_SITE_ID);
	}
	unsigned int crcResultChecksum = crcResult.checksum();

	iteration++;
	std::set<unsigned int>::iterator csi = checkSums.find(crcResultChecksum);
	if (csi == checkSums.end()) {
		checkSums.insert(crcResultChecksum);
		return false;
	}
	else{
		cout << "Cycle detected on iteration " << std::to_string(iteration) << endl;
		return true;
	}

	//cout << "Iteration: " << std::to_string(iteration) << ": CRC32 " << std::to_string(crcResultChecksum) << endl;
	//cout << "Iteration: " << std::to_string(iteration) << ": CRC32 " << std::to_string(getSitesCrc32()) << endl;
	
//	return iteration == 100 ? true : false;
}

void RandomBooleanNetwork::incrementExternalInputRows() {
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
		this->neighborhoodConnections
	);
}

void RandomBooleanNetwork::decrementExternalInputRows() {
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
		this->neighborhoodConnections
	);
}

void RandomBooleanNetwork::feedForward() {
	// Copy feedbackInputRowCount rows from the external output into the feedback input sites.
	unsigned int outputSiteIndex = externalOutputStartCellIndex;
	for (unsigned int i = feedbackInputStartCellIndex; i <= feedbackInputEndCellIndex && outputSiteIndex <= externalOutputEndCellIndex; i++) {
		Site* feedbackSite = &sites[i];
		Site* outputSite = &sites[outputSiteIndex++];
		feedbackSite->currentState = outputSite->currentState;
	}
}

unsigned int RandomBooleanNetwork::getConnectivity() {
	return connectivity;
}

std::vector<Site>* RandomBooleanNetwork::getSites()
{
	return &sites;
}

unsigned int RandomBooleanNetwork::getSitesCrc32()
{
	boost::crc_32_type crcResult;
	unsigned int size = sites.size();
	for (unsigned int i = 0; i < size; i++)
	{
		Site* s = &sites[i];
//		crcResult.process_bytes(&s.siteId, SIZE_SITE_ID);
		crcResult.process_bytes(&s->currentState, SIZE_CURRENT_STATE);
//		crcResult.process_bytes(&s.workingState, SIZE_WORKING_STATE);
//		crcResult.process_bytes(&s.booleanFunctionId, SIZE_BOOLEAN_FUNCTION_ID);
	//	crcResult.process_bytes(s.inputSiteIds.data(), s.inputSiteIds.size() * SIZE_SITE_ID);
//		crcResult.process_bytes(s.outputSiteIds.data(), s.outputSiteIds.size() * SIZE_SITE_ID);
	}

	return crcResult.checksum();
}

void RandomBooleanNetwork::printConfigurationState() {

	cout << "--------------------------------------------------------" << endl 
		<< "Initializing RBN:" << endl
		<< "rows                    = " << std::to_string(this->rows) << endl
		<< "cols                    = " << std::to_string(this->cols) << endl
		<< "connectivity            = " << std::to_string(this->connectivity) << endl
		<< "externalInputRowCount   = " << std::to_string(this->externalInputRowCount) << endl
		<< "feedbackInputRowCount   = " << std::to_string(this->feedbackInputRowCount) << endl
		<< "externalOutputRowCount  = " << std::to_string(this->externalOutputRowCount) << endl
		<< "neighborhoodConnections = " << (this->neighborhoodConnections ? "true" : "false") << endl
		<< endl;
}

void RandomBooleanNetwork::cleanUp()
{
	if (initialized) {
		sites.clear();
		checkSums.clear();
	}
}

RandomBooleanNetwork::~RandomBooleanNetwork()
{
	cleanUp();
}

std::string siteToJson(const Site& site) {
	std::string result =
		"{siteId:" + std::to_string(site.siteId)
		+ ",currentState:" + (site.currentState ? "true" : "false")
		+ ",workingState:" + (site.workingState ? "true" : "false")
		+ ",booleanFunctionId:" + std::to_string(site.booleanFunctionId)
		+",inputSiteIds:[";

	std::vector<unsigned int> inputSiteIds = site.inputSiteIds;
	unsigned int inputSiteIdsSize = inputSiteIds.size();
	for (unsigned int i = 0; i < inputSiteIdsSize; i++) {
		if (i > 0)
			result += ",";
		result += std::to_string(inputSiteIds[i]);
	}

	result += "],outputSiteIds:[";
	std::vector<unsigned int> outputSiteIds = site.outputSiteIds;
	unsigned int outputSiteIdsSize = outputSiteIds.size();
	for (unsigned int i = 0; i < outputSiteIdsSize; i++) {
		if (i > 0)
			result += ",";
		result += std::to_string(outputSiteIds[i]);
	}
	result += "]}";

	return result;
}

unsigned int siteCrc32(const Site& site) {

	std::string result =
		std::to_string(site.siteId)
		+ "|" + std::to_string(site.currentState)
		+ "|" + std::to_string(site.workingState)
		+ "|" + std::to_string(site.booleanFunctionId)
		+ "|";

	std::vector<unsigned int> inputSiteIds = site.inputSiteIds;
	unsigned int inputSiteIdsSize = inputSiteIds.size();
	for (unsigned int i = 0; i < inputSiteIdsSize; i++) {
		if (i > 0)
			result += ",";
		result += std::to_string(inputSiteIds[i]);
	}

	result += "|";
	std::vector<unsigned int> outputSiteIds = site.outputSiteIds;
	unsigned int outputSiteIdsSize = outputSiteIds.size();
	for (unsigned int i = 0; i < outputSiteIdsSize; i++) {
		if (i > 0)
			result += ",";
		result += std::to_string(outputSiteIds[i]);
	}

	boost::crc_32_type crcResult;
	crcResult.process_bytes(result.data(), result.length());
	
	return crcResult.checksum();
}
