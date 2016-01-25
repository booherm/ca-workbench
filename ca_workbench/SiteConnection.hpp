#ifndef SITECONNECTION_HPP
#define SITECONNECTION_HPP

#include <vector>

class SiteConnection
{
public:
	unsigned int sourceSiteId;
	unsigned int destinationSiteId;
	std::vector<float> color;
};

#endif
