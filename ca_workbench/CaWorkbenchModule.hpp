#ifndef CAWORKBENCHMODULE_HPP
#define CAWORKBENCHMODULE_HPP

#include "Site.hpp"
#include "SiteConnection.hpp"
#include <glfw3.h>
#include <random>
#include "json.hpp"

class CaWorkbenchModule
{
public:
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

	unsigned int getRowCount();
	unsigned int getColumnCount();
	bool getRenderComplete();
	unsigned int getIteration();

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
};

#endif
