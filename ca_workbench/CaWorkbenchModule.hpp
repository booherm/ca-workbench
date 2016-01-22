#ifndef CAWORKBENCHMODULE_HPP
#define CAWORKBENCHMODULE_HPP

#include "Site.hpp"
#include <random>

class CaWorkbenchModule
{
public:
	// constructors
	CaWorkbenchModule(
		unsigned int rows,
		unsigned int cols
	);

	// public member functions
	virtual bool iterate();
	virtual void handleInputAction(int action, int key);
	virtual Site* getSite(unsigned int siteId);
	virtual bool getSiteActive(unsigned int siteId);
	virtual std::vector<float>* CaWorkbenchModule::getSiteColor(unsigned int siteId);

	std::vector<unsigned int>* getConnectionVectors();
	unsigned int getRowCount();
	unsigned int getColumnCount();

	// destructor
	virtual ~CaWorkbenchModule();

protected:
	unsigned int rows;
	unsigned int cols;
	std::default_random_engine rnGen;
	std::vector<Site> sites;
	std::vector<unsigned int> connectionVectors;
};

#endif
