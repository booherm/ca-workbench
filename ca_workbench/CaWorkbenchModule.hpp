#ifndef CAWORKBENCHMODULE_HPP
#define CAWORKBENCHMODULE_HPP

#include "Site.hpp"
#include "SiteConnection.hpp"
#include <glfw3.h>
#include <random>
#include <queue>
#include <boost/thread.hpp>
#include "json.hpp"

class CaWorkbenchModule
{
public:
	struct ConfigSetting {
		std::string key;
		std::string value;
	};

	// constructors
	CaWorkbenchModule(
		unsigned int rows,
		unsigned int cols
	);

	// public member functions
	virtual void iterate();
	virtual void handleInputAction(int action, int key);
	virtual bool getSiteActive(unsigned int siteId);
	virtual std::vector<float>* getSiteColor(unsigned int siteId);
	virtual unsigned int getMaxSiteConnectionsCount();
	virtual std::vector<SiteConnection*>* getSiteConnections(unsigned int siteId);
	virtual void getConfigJson(Json::Value& configJson);
	virtual void getStateJson(Json::Value& stateJson);
	virtual bool setConfigurationValue(const std::string& setting, const std::string& value);
	virtual void processConfigChangeQueue();

	unsigned int getRowCount();
	unsigned int getColumnCount();
	bool getRenderComplete();
	unsigned int getIteration();
	void enqueueConfigChange(ConfigSetting setting);

	// destructor
	virtual ~CaWorkbenchModule();


protected:
	unsigned int rows;
	unsigned int cols;
	unsigned int iteration;
	bool renderComplete;
	std::default_random_engine rnGen;
	std::vector<Site> sites;
	std::vector<SiteConnection> siteConnections;
	std::queue<ConfigSetting> configUpdateQueue;
	boost::mutex configUpdateMutex;
};

#endif
