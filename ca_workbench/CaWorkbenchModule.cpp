#include "CaWorkbenchModule.hpp"

CaWorkbenchModule::CaWorkbenchModule(unsigned int rows, unsigned int cols) {
	// initialize random number generator
	rnGen.seed(std::random_device()());

	this->rows = rows;
	this->cols = cols;
	iteration = 0;
	renderComplete = false;
}

void CaWorkbenchModule::iterate() {
	iteration++;
}

void CaWorkbenchModule::handleInputAction(int action, int key) {
}

inline bool CaWorkbenchModule::getSiteActive(unsigned int siteId) {
	return sites[siteId].currentState;
}

inline std::vector<float>* CaWorkbenchModule::getSiteColor(unsigned int siteId) {
	return &sites[siteId].color;
}

inline unsigned int CaWorkbenchModule::getMaxSiteConnectionsCount() {
	return 0;
}

inline std::vector<SiteConnection*>* CaWorkbenchModule::getSiteConnections(unsigned int siteId) {
	return (std::vector<SiteConnection*>*) &sites[siteId].siteConnections;
}

bool CaWorkbenchModule::getRenderComplete() {
	return renderComplete;
}

unsigned int CaWorkbenchModule::getRowCount() {
	return rows;
}

unsigned int CaWorkbenchModule::getColumnCount() {
	return cols;
}

CaWorkbenchModule::~CaWorkbenchModule() {
}
