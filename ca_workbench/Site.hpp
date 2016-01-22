#ifndef SITE_HPP
#define SITE_HPP

#include <string>
#include <vector>

class Site
{
public:
	unsigned int siteId;
	bool currentState;
	bool workingState;
	std::vector<float> color;
	unsigned int stateChangeCount;
};

const size_t SIZE_SITE_ID = sizeof(Site::siteId);
const size_t SIZE_CURRENT_STATE = sizeof(Site::currentState);
const size_t SIZE_WORKING_STATE = sizeof(Site::workingState);

std::string siteToJson(const Site& site);

#endif
