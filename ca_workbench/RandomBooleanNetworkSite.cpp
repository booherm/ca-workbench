#include "RandomBooleanNetworkSite.hpp"

std::string siteToJson(const RandomBooleanNetworkSite& site) {
	std::string result =
		"{siteId:" + std::to_string(site.siteId)
		+ ",currentState:" + (site.currentState ? "true" : "false")
		+ ",workingState:" + (site.workingState ? "true" : "false")
		+ ",stateChangeCount:" + std::to_string(site.stateChangeCount)
		+ ",color:[" + std::to_string(site.color.at(0))
		+ "," + std::to_string(site.color.at(1))
		+ "," + std::to_string(site.color.at(2))
		+ "],booleanFunctionId:" + std::to_string(site.booleanFunctionId)
		+ ",inputSiteIds:[";

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
