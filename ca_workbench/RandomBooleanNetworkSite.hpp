#ifndef RANDOMBOOLEANNETWORKSITE_HPP
#define RANDOMBOOLEANNETWORKSITE_HPP

#include "Site.hpp"

class RandomBooleanNetworkSite : public Site
{
public:
	bool freshActivation;
	unsigned int booleanFunctionId;
	std::vector<unsigned int> inputSiteIds;
	std::vector<unsigned int> outputSiteIds;
};

std::string siteToJson(const RandomBooleanNetworkSite& site);

#endif
