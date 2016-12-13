#include "TwoDimCaModule.hpp"

TwoDimCaModule::TwoDimCaModule(unsigned int rows, unsigned int cols) : CaWorkbenchModule(rows, cols) {

	// von Neumann neighborhood (ME, N, E, S, W) requires state specifier string with length stateCount^5:
	//   2 state:   32 chars
	//   3 state:  243 chars
	//   4 state: 1024 chars
	//   5 state: 3125 chars
	// input will provide the state count and the representation
	// 
	// Moore neighborhood requires state specifier string stateCount^9 = 512 characters long: [0, some huge number]

	stateCount = 2;
	activeNeighborhoodType = NeighborhoodType::VON_NEUMANN;
	//stateSpecifierString = "01101001100101101001011001101001";  // Fredkin2
	stateSpecifierString = "01101101101101101111101011001000";  // Crystal2
	//stateSpecifierString = "00000000000000000000000000000000";  // state cycle detection test
	//stateSpecifierString = "00100111010011010101110001010001";  // random
	//stateSpecifierString = "01100011101100100011101110100110";  // random 2
	//stateSpecifierString = "00000000000000001111111111111111";  // random 2
	//stateSpecifierString = "11111111111111110000000000000000";  // random 2


	resetCellStates();
}

void TwoDimCaModule::resetCellStates() {

	bool initialRandom = false;
	bool initialCenterSiteOn = true;

	// initialize cell states
	sites.resize(rows * cols);
	
	std::bernoulli_distribution initialStateRandomDist(0.50);
	for (unsigned int i = 0; i < rows * cols; i++) {
		Site s;
		s.siteId = i;
		s.currentState = initialRandom ? initialStateRandomDist(rnGen) : false;
		s.workingState = false;
		s.stateChangeCount = 0;
		s.color.resize(3);

		if (s.currentState) {
			s.color[0] = 0.0f;
			s.color[1] = 0.0f;
			s.color[2] = 0.0f;
		}
		else {
			s.color[0] = 1.0f;
			s.color[1] = 1.0f;
			s.color[2] = 1.0f;
		}

		sites[i] = s;
	}

	if (initialCenterSiteOn) {
		Site* initSite = &sites[getSiteIndex((rows / 2) - 1, (cols / 2) - 1)];
		initSite->currentState = true;
		initSite->color.at(0) = 0.0f;
		initSite->color.at(1) = 0.0f;
		initSite->color.at(2) = 0.0f;
	}

	iteration = 1;
	renderComplete = false;
	checkSums.clear();

	printConfigurationState();
}

unsigned int TwoDimCaModule::getSiteIndex(unsigned int row, unsigned int col) {
	return (cols * row) + col;
}

void TwoDimCaModule::iterate()
{
	// process any requested config changes
	processConfigChangeQueue();

	// Assuming von Neumann neighborhood with 2 possible states, rule represented by 32 bit unsigned integer.
	// Ex:  2527982857 = 10010110101011011111010100001001
	// Fredkin2 = 1771476585 = 01101001100101101001011001101001
	// bit i   ME N E S W
	// ------------------
	//     0    0 0 0 0 0
	//     1    0 0 0 0 1
	//     2    0 0 0 1 0
	//     3    0 0 0 1 1
	//   ...
	//    31    1 1 1 1 1

	// For each site, match the site current state to exactly 1 of the 32 possible confiugrations.  Set the site's new value to bit i's value.

	for (unsigned int row = 0; row < rows; row++) {
		for (unsigned int col = 0; col < cols; col++) {

			// site indexes of neighborhood
			unsigned int meRow = row;
			unsigned int meCol = col;
			unsigned int meSiteIndex = getSiteIndex(meRow, meCol);
			unsigned int northRow = row == 0 ? rows - 1 : row - 1;
			unsigned int northCol = col;
			unsigned int northSiteIndex = getSiteIndex(northRow, northCol);
			unsigned int eastRow = row;
			unsigned int eastCol = col == cols - 1 ? 0 : col + 1;
			unsigned int eastSiteIndex = getSiteIndex(eastRow, eastCol);
			unsigned int southRow = row == rows - 1 ? 0 : row + 1;
			unsigned int southCol = col;
			unsigned int southSiteIndex = getSiteIndex(southRow, southCol);
			unsigned int westRow = row;
			unsigned int westCol = col == 0 ? cols - 1 : col - 1;
			unsigned int westSiteIndex = getSiteIndex(westRow, westCol);

			// build the current neighborhood state for this site into an unsigned int value [0, 31]
			unsigned int neighborhoodCurrentState = 0;
			neighborhoodCurrentState = neighborhoodCurrentState
				| (sites[meSiteIndex].currentState == 1    ? 1 << 4: 0)
				| (sites[northSiteIndex].currentState == 1 ? 1 << 3: 0)
				| (sites[eastSiteIndex].currentState == 1  ? 1 << 2: 0)
				| (sites[southSiteIndex].currentState == 1 ? 1 << 1: 0)
				| (sites[westSiteIndex].currentState == 1  ? 1 << 0: 0);

			// get the boolean state of the active rule number at the bit location indicated by the current neighborhood state
			unsigned int newStateDebug = (unsigned int) stateSpecifierString[neighborhoodCurrentState] - '0';
			
			bool newState = newStateDebug == 1;
			sites[meSiteIndex].workingState = newState;
		}
	}

	// replace the current state with the working state
	boost::crc_32_type crcResult;
	for (unsigned int i = 0; i < sites.size(); i++) {
		Site* site = &sites[i];
		site->currentState = site->workingState;
		std::vector<float>* color = &site->color;
		if (site->currentState) {
			color->at(0) = 0.0f;
			color->at(1) = 0.0f;
			color->at(2) = 0.0f;
		}
		else {
			color->at(0) = 1.0f;
			color->at(1) = 1.0f;
			color->at(2) = 1.0f;
		}

		crcResult.process_byte(site->currentState);
	}
	unsigned int crcResultChecksum = crcResult.checksum();

	// store checksum in set, mark render complete if a cycle has been detected yet
	iteration++;
	std::set<unsigned int>::iterator csi = checkSums.find(crcResultChecksum);
	if (csi == checkSums.end()) {
		checkSums.insert(crcResultChecksum);
	}
	else {
		std::cout << "Cycle detected on iteration " << std::to_string(iteration) << std::endl;
		renderComplete = true;
	}

}

void TwoDimCaModule::printConfigurationState() {
	std::cout << "--------------------------------------------------------" << std::endl
		<< "Two Dimensional CA Configuration:" << std::endl
		<< "rows                    = " << std::to_string(rows) << std::endl
		<< "cols                    = " << std::to_string(cols) << std::endl
		<< "stateCount              = " << std::to_string(stateCount) << std::endl
		<< "activeNeighborhoodType  = " << (activeNeighborhoodType == NeighborhoodType::MOORE ? "Moore" : "von Neumann") << std::endl
		<< "stateSpecifierString    = " << stateSpecifierString << std::endl
		<< std::endl;
}

void TwoDimCaModule::getConfigJson(Json::Value& configJson) {
	configJson.clear();
	configJson["rows"] = rows;
	configJson["columns"] = cols;
	configJson["state_count"] = stateCount;
	configJson["state_specifier_string"] = stateSpecifierString;
}

TwoDimCaModule::~TwoDimCaModule()
{
}

bool TwoDimCaModule::setStateCount(unsigned int stateCount) {

	if (stateCount < 2) {
		std::cout << "state count must be >= 2" << std::endl;
		return false;
	}
	if(stateCount > 10) {
		std::cout << "state count must be <= 10" << std::endl;
		return false;
	}

	if (!validateConfigurationCompatibility(activeNeighborhoodType, stateCount, stateSpecifierString))
		return false;

	this->stateCount = stateCount;
	resetCellStates();
	return true;
}

bool TwoDimCaModule::setActiveNeighborhoodType(NeighborhoodType neighborhoodType) {

	if (!validateConfigurationCompatibility(neighborhoodType, stateCount, stateSpecifierString))
		return false;

	activeNeighborhoodType = neighborhoodType;
	resetCellStates();
	return true;
}

bool TwoDimCaModule::setStateSpecifierString(const std::string& stateSpecifierString) {

	if (!validateConfigurationCompatibility(activeNeighborhoodType, stateCount, stateSpecifierString))
		return false;

	this->stateSpecifierString = stateSpecifierString;
	resetCellStates();
	return true;
}

bool TwoDimCaModule::setConfigurationValue(const ConfigSetting& setting) {
	if (setting.key == "stateCount") {
		return setStateCount(std::stol(setting.value));
	}
	else if (setting.key == "activeNeighborhoodType") {
		return setActiveNeighborhoodType((NeighborhoodType) stoul(setting.value));
	}
	else if (setting.key == "stateSpecifierString") {
		return setStateSpecifierString(setting.value);
	}
	else if (setting.key == "reset") {
		resetCellStates();
		return true;
	}

	return false;
}

void TwoDimCaModule::processConfigChangeQueue() {
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

bool TwoDimCaModule::validateConfigurationCompatibility(NeighborhoodType neighborhoodType, unsigned int stateCount, const std::string& stateSpecifierString) {

	// validate the state specifier string has exact number of required characters for the specified state count
	unsigned int reqStringLength;

	if (neighborhoodType == NeighborhoodType::VON_NEUMANN) {
		reqStringLength = pow(stateCount, 5);
	}
	else if (neighborhoodType == NeighborhoodType::MOORE) {
		reqStringLength = pow(stateCount, 9);
	}

	unsigned int stateSpecifierStringLength = stateSpecifierString.length();
	if (stateSpecifierStringLength != reqStringLength) {
		std::cout << "state specifier string length not equal to required string length of " << reqStringLength
			<< " (current length " << stateSpecifierStringLength << ")" << std::endl;
		return false;
	}

	// validate the state specifier string characters match expected values for the specified number of states allowed
	std::string validChars = "";
	for (unsigned int i = 0; i < stateCount; i++) {
		validChars += std::to_string(i);
	}

	for (unsigned int i = 0; i < stateSpecifierStringLength; i++) {
		if (validChars.find(stateSpecifierString[i]) == std::string::npos) {
			std::cout << "invalid character in state specifier string: " << stateSpecifierString[i] << std::endl;
			return false;
		}
	}

}
