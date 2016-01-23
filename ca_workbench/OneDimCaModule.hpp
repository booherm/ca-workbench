#ifndef ONEDIMCAMODULE_HPP
#define ONEDIMCAMODULE_HPP

#include "CaWorkbenchModule.hpp"
#include <iostream>

class OneDimCaModule : public CaWorkbenchModule
{
public:
	OneDimCaModule(unsigned int rows, unsigned int cols, unsigned int initialRuleSet);

	// module interface
	void iterate();
	void handleInputAction(int action, int key);
	~OneDimCaModule();

private:
	unsigned int activeRuleNumber;
	unsigned int activeRuleSetNumber;
	unsigned int ruleCount;
	bool rules[256][8];

	void resetCellStates();
	void incrementActiveRuleNumber();
	void decrementActiveRuleNumber();
	void incrementActiveRuleSetNumber();
	void decrementActiveRuleSetNumber();
	void printConfigurationState();
};

#endif
