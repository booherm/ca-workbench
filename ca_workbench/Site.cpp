#include "Site.hpp"

std::string siteToJson(const Site& site) {
	std::string result =
		"{siteId:" + std::to_string(site.siteId)
		+ ",currentState:" + (site.currentState ? "true" : "false")
		+ ",workingState:" + (site.workingState ? "true" : "false")
		+ ",stateChangeCount:" + std::to_string(site.stateChangeCount)
		+ ",color:[" + std::to_string(site.color.at(0))
		+ "," + std::to_string(site.color.at(1))
		+ "," + std::to_string(site.color.at(2))
		+ "]}";

	return result;
}
