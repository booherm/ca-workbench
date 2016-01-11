#include "RandomBooleanNetwork.hpp"

using namespace std;

RandomBooleanNetwork::RandomBooleanNetwork(
	unsigned int rows,
	unsigned int cols,
	unsigned int connectivity,
	unsigned int externalInputRowCount,
	unsigned int externalOutputRowCount,
	bool neighborhoodConnections
) {
	this->rows = rows;
	this->cols = cols;
	this->connectivity = connectivity;
	this->externalInputRowCount = externalInputRowCount;
	this->externalOutputRowCount = externalOutputRowCount;
	this->neighborhoodConnections = neighborhoodConnections;

	// initialize random number generator
	rnGen.seed(random_device()());

	initialized = false;
	resetCellStates();
	initialized = true;
}

void RandomBooleanNetwork::updateInputSites()
{
	bernoulli_distribution initialStateRandomDist(0.50);
	for (unsigned int i = 0; i < externalInputRowCount * cols; i++) {
		Site* s = &sites[i];
		s->currentState = initialStateRandomDist(rnGen);
	}
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
		if (i < externalInputRowCount * cols) {  // external input site, blue, no boolean function
			s.color[0] = 0.0f;
			s.color[1] = 0.0f;
			s.color[2] = 1.0f;
		}
		else if (i >= siteCount - (externalOutputRowCount * cols)) { // external output site, red
		   // choose random boolean function
			s.booleanFunctionId = randBooleanFunctionDist(rnGen);

			// color red
			s.color[0] = 1.0f;
			s.color[1] = 0.0f;
			s.color[2] = 0.0f;
		}
		else {
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
		// neighbor connectivity
		if (connectivity <= 4) // von Neumann (k <= 4)
		{
			for (unsigned int i = externalInputRowCount * cols; i < siteCount; i++) {
				//cout << "processing site " << std::to_string(i) << endl;
				Site* s = &sites[i];

				// put neighbors in bag
				unsigned int thisSiteR = i / cols;
				unsigned int thisSiteC = i - (thisSiteR * cols);
				unsigned int neighborR;
				unsigned int neighborC;
				std::unordered_set<unsigned int> bag;
				// above
				neighborR = thisSiteR == externalInputRowCount ? rows - 1 : thisSiteR - 1;
				neighborC = thisSiteC;
				bag.insert((neighborR * cols) + neighborC);
				// right
				neighborR = thisSiteR;
				neighborC = thisSiteC == cols - 1 ? 0 : thisSiteC + 1;
				bag.insert((neighborR * cols) + neighborC);
				// below
				neighborR = thisSiteR == rows - 1 ? externalInputRowCount : thisSiteR + 1;
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
	}
	else
	{
		// initialize site connectivity, arbitrary sites
		uniform_int_distribution<unsigned int> randInputDist(0, siteCount);
		for (unsigned int i = externalInputRowCount * cols; i < siteCount; i++) {
			Site* s = &sites[i];

			// assign k inputs randomly
			for (unsigned int k = 0; k < connectivity; k++) {
				// pick random site, not self, not one that was already chosen as an input site, not an external output site
				unsigned int inputSite;
				bool alreadyUsed;

				unsigned int attempt = 0;
				do {
					if (attempt > 0)
						cout << "extraneous effort" << endl;
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

bool RandomBooleanNetwork::iterate()
{
	// update logical state
	//cout << "begin iteration " << std::to_string(iteration) << endl;
	for (unsigned int i = externalInputRowCount * cols; i < sites.size(); i++) {
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

	// push working state to new current state
	for (unsigned int i = externalInputRowCount * cols; i < sites.size(); i++) {
		Site* s = &sites[i];
		s->currentState = s->workingState;
	}

//	cout << "Iteration: " << std::to_string(iteration) << ": CRC32 " << std::to_string(getSitesCrc32()) << endl;
	iteration++;


	return iteration == 1000 ? true : false;
}

std::vector<Site>* RandomBooleanNetwork::getSites()
{
	return &sites;
}

unsigned int RandomBooleanNetwork::getSitesCrc32() {

	boost::crc_32_type crcResult;
	for (unsigned int i = 0; i < sites.size(); i++)
	{
		Site s = sites[i];
		crcResult.process_bytes(&s.siteId, SIZE_SITE_ID);
		crcResult.process_bytes(&s.currentState, SIZE_CURRENT_STATE);
		crcResult.process_bytes(&s.workingState, SIZE_WORKING_STATE);
		crcResult.process_bytes(&s.booleanFunctionId, SIZE_BOOLEAN_FUNCTION_ID);
		crcResult.process_bytes(s.inputSiteIds.data(), s.inputSiteIds.size() * SIZE_SITE_ID);
		crcResult.process_bytes(s.outputSiteIds.data(), s.outputSiteIds.size() * SIZE_SITE_ID);
	}

	return crcResult.checksum();
}

void RandomBooleanNetwork::cleanUp()
{
	if (initialized) {
		sites.clear();
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
