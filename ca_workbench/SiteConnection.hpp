#ifndef SITECONNECTION_HPP
#define SITECONNECTION_HPP

#include <vector>

class SiteConnection
{
public:
	SiteConnection();
	SiteConnection(unsigned int sourceSiteId, unsigned int destinationSiteId, const std::vector<float>& color, bool shouldRender);

	unsigned int sourceSiteId;
	unsigned int destinationSiteId;
	std::vector<float> color;
	bool shouldRender;
};

#endif
