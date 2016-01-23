#include "OneDimCaModule.hpp"

using namespace std;

OneDimCaModule::OneDimCaModule(unsigned int rows, unsigned int cols, unsigned int initialRuleSet) : CaWorkbenchModule(rows, cols)
{
	activeRuleSetNumber = initialRuleSet;
	if (activeRuleSetNumber > 3)
		throw string("OneDimCaModule - initial rule set number must be [0-3]");


	/*
	// R = 2:  2^(2^0)
	rules[0][0] = true;
	rules[1][0] = false;

	// R = 4:  2^(2^1)
	for (unsigned int rc = 0; rc < 4; rc++) {
		unsigned char r = (unsigned char)rc;

		for (unsigned int jc = 0; jc <= 1; jc++) {
			unsigned char j = (unsigned char)jc;
			unsigned char shifted = r >> j;
			rules[rc][1 - jc] = shifted & 1;
		}
	}

	// R = 16:  2^(2^2)
	for (unsigned int rc = 0; rc < 16; rc++) {
	unsigned char r = (unsigned char)rc;

	for (unsigned int jc = 0; jc <= 3; jc++) {
	unsigned char j = (unsigned char)jc;
	unsigned char shifted = r >> j;
	rules[rc][3 - jc] = shifted & 1;
	}
	}

	// R = 256:  2^(2^3)
	for (unsigned int rc = 0; rc < 256; rc++) {
	unsigned char r = (unsigned char)rc;

	for (unsigned int jc = 0; jc <= 7; jc++) {
	unsigned char j = (unsigned char)jc;
	unsigned char shifted = r >> j;
	rules[rc][7 - jc] = shifted & 1;
	}
	}
	*/

	resetCellStates();
}

void OneDimCaModule::resetCellStates()
{
	// initialize rule set

	// R: Rule Count
	// K: Number of States
	// N: Size of Neigborhood
	// R = K^(K^N)
	unsigned int neighborhoodStates = (unsigned int) pow(2, activeRuleSetNumber);
	ruleCount = (unsigned int) pow(2, neighborhoodStates);
	for (unsigned int rc = 0; rc < ruleCount; rc++) {
		unsigned char r = (unsigned char)rc;
		for (unsigned int jc = 0; jc <= neighborhoodStates - 1; jc++) {
			unsigned char j = (unsigned char)jc;
			unsigned char shifted = r >> j;
			rules[rc][neighborhoodStates - 1 - jc] = shifted & 1;
		}
	}

	// initialize cell states
	sites.resize(rows * cols);

	for (unsigned int i = 0; i < rows * cols; i++) {
		Site s;
		s.siteId = i;
		s.currentState = false;
		s.workingState = false;
		s.stateChangeCount = 0;
		s.color.resize(3);
		s.color[0] = 0.0f;
		s.color[1] = 0.0f;
		s.color[2] = 0.0f;

		sites[i] = s;
	}

	// one cell on initial condition
	sites[cols / 2].currentState = true;
	iteration = 1;
	renderComplete = false;

	printConfigurationState();
}

void OneDimCaModule::iterate()
{
	bool* activeRule = rules[activeRuleNumber];
	unsigned int prevIteration = iteration - 1;

	for (unsigned int i = 0; i < cols; i++)
	{

		if (activeRuleSetNumber == 0) {
			// R = 2
			if ((activeRule[0])) {
				Site* thisSite = &sites[(iteration * cols) + i];
				thisSite->currentState = true;
				thisSite->stateChangeCount++;
			}
		}
		else if (activeRuleSetNumber == 1) {
			// R = 4
			bool pir_n0;
			pir_n0 = sites[(prevIteration * cols) + i].currentState;
			if (
				(pir_n0  && activeRule[0])
				|| (!pir_n0 && activeRule[1])
				) {

				Site* thisSite = &sites[(iteration * cols) + i];
				thisSite->currentState = true;
				thisSite->stateChangeCount++;
			}
		}
		else if (activeRuleSetNumber == 2) {
			// R = 16
			bool pir_n0;
			bool pir_n1;

			// wrap-around array
			pir_n0 = sites[(prevIteration * cols) + i].currentState;
			pir_n1 = (i == cols - 1 ? sites[(prevIteration * cols)].currentState : sites[(prevIteration * cols) + i + 1].currentState);

			if (((pir_n0 &&  pir_n1) && activeRule[0])
				|| ((pir_n0 &&  !pir_n1) && activeRule[1])
				|| ((!pir_n0 && pir_n1) && activeRule[2])
				|| ((!pir_n0 && !pir_n1) && activeRule[3])
				) {
				Site* thisSite = &sites[(iteration * cols) + i];
				thisSite->currentState = true;
				thisSite->stateChangeCount++;
			}
		}
		else if (activeRuleSetNumber == 3) {

			// R = 256
			bool pir_n0;
			bool pir_n1;
			bool pir_n2;

			// wrap-around array
			pir_n0 = (i == 0 ? sites[(prevIteration * cols) + (cols - 1)].currentState : sites[(prevIteration * cols) + i - 1].currentState);
			pir_n1 = sites[(prevIteration * cols) + i].currentState;
			pir_n2 = (i == cols - 1 ? sites[(prevIteration * cols)].currentState : sites[(prevIteration * cols) + i + 1].currentState);

			if (((pir_n0 &&  pir_n1 &&  pir_n2) && activeRule[0])
				|| ((pir_n0 &&  pir_n1 && !pir_n2) && activeRule[1])
				|| ((pir_n0 && !pir_n1 &&  pir_n2) && activeRule[2])
				|| ((pir_n0 && !pir_n1 && !pir_n2) && activeRule[3])
				|| ((!pir_n0 &&  pir_n1 &&  pir_n2) && activeRule[4])
				|| ((!pir_n0 &&  pir_n1 && !pir_n2) && activeRule[5])
				|| ((!pir_n0 && !pir_n1 &&  pir_n2) && activeRule[6])
				|| ((!pir_n0 && !pir_n1 && !pir_n2) && activeRule[7])
				) {

				Site* thisSite = &sites[(iteration * cols) + i];
				thisSite->currentState = true;
				thisSite->stateChangeCount++;

			}
		}

	}

	if (iteration == rows - 1)
		renderComplete = true;
	iteration++;
}

void OneDimCaModule::incrementActiveRuleNumber() {

	if (activeRuleNumber == ruleCount - 1) {
		cout << "cannot exceed maximum rule number, ignoring" << endl;
		return;
	}

	activeRuleNumber++;
	resetCellStates();
}

void OneDimCaModule::decrementActiveRuleNumber() {
	if (activeRuleNumber == 0) {
		cout << "active rule number is 0, cannot decrement, ignoring" << endl;
		return;
	}

	activeRuleNumber--;
	resetCellStates();
}

void OneDimCaModule::incrementActiveRuleSetNumber() {

	if (activeRuleSetNumber == 3) {
		cout << "rule set cannot exceed 3, ignoring" << endl;
		return;
	}

	activeRuleNumber = 0;
	activeRuleSetNumber++;
	resetCellStates();
}

void OneDimCaModule::decrementActiveRuleSetNumber() {
	if (activeRuleSetNumber == 0) {
		cout << "active rule set number is 0, cannot decrement, ignoring" << endl;
		return;
	}

	activeRuleNumber = 0;
	activeRuleSetNumber--;
	resetCellStates();
}

void OneDimCaModule::handleInputAction(int action, int key) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_UP:
				incrementActiveRuleNumber();
				break;
			case GLFW_KEY_DOWN:
				decrementActiveRuleNumber();
				break;
			case GLFW_KEY_LEFT:
				decrementActiveRuleSetNumber();
				break;
			case GLFW_KEY_RIGHT:
				incrementActiveRuleSetNumber();
				break;
		}
	}
}

void OneDimCaModule::printConfigurationState() {
	cout << "--------------------------------------------------------" << endl
		<< "One Dimensional CA Configuration:" << endl
		<< "rows                    = " << std::to_string(rows) << endl
		<< "cols                    = " << std::to_string(cols) << endl
		<< "activeRuleSetNumber     = " << std::to_string(activeRuleSetNumber) << endl
		<< "ruleCount               = " << std::to_string(ruleCount) << endl
		<< "activeRuleNumber        = " << std::to_string(activeRuleNumber) << endl
		<< endl;
}

OneDimCaModule::~OneDimCaModule()
{
}
