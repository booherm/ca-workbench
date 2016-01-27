#ifndef RANDOMBOOLEANNETWORKSITE_HPP
#define RANDOMBOOLEANNETWORKSITE_HPP

#include "Site.hpp"

class RandomBooleanNetworkSite : public Site
{
public:
	bool freshActivation;
	unsigned int booleanFunctionId;
};

#endif
