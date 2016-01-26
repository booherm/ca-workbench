#include "SiteConnection.hpp"

SiteConnection::SiteConnection() {
}

SiteConnection::SiteConnection(unsigned int sourceSiteId, unsigned int destinationSiteId, const std::vector<float>& color, bool shouldRender) {
	this->sourceSiteId = sourceSiteId;
	this->destinationSiteId = destinationSiteId;
	this->color = color;
	this->shouldRender = shouldRender;
}
