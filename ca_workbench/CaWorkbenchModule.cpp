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

inline unsigned char CaWorkbenchModule::getSiteState(unsigned int siteId) {
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

void CaWorkbenchModule::getConfigJson(Json::Value& configJson) {
	configJson.clear();
	configJson["rows"] = rows;
	configJson["columns"] = cols;
}

void CaWorkbenchModule::getStateJson(Json::Value& stateJson) {
	stateJson.clear();
	stateJson["iteration"] = iteration;
	stateJson["renderComplete"] = renderComplete;
}

bool CaWorkbenchModule::setConfigurationValue(const std::string& setting, const std::string& value) {
	return false;
}

void CaWorkbenchModule::processConfigChangeQueue() {
}

bool CaWorkbenchModule::getRenderComplete() {
	return renderComplete;
}

unsigned int CaWorkbenchModule::getIteration() {
	return iteration;
}

unsigned int CaWorkbenchModule::getRowCount() {
	return rows;
}

unsigned int CaWorkbenchModule::getColumnCount() {
	return cols;
}

void CaWorkbenchModule::enqueueConfigChange(ConfigSetting setting) {
	configUpdateMutex.lock();
	configUpdateQueue.push(setting);
	configUpdateMutex.unlock();
}

CaWorkbenchModule::~CaWorkbenchModule() {
}
