#ifndef SITE_HPP
#define SITE_HPP

#include <string>
#include <vector>
#include "SiteConnection.hpp"

class Site
{
public:
	unsigned int siteId;
	unsigned char currentState;
	unsigned char workingState;
	std::vector<float> color;
	unsigned int stateChangeCount;
	std::vector<SiteConnection*> siteConnections;
};

const size_t SIZE_SITE_ID = sizeof(Site::siteId);
const size_t SIZE_CURRENT_STATE = sizeof(Site::currentState);
const size_t SIZE_WORKING_STATE = sizeof(Site::workingState);

#endif
