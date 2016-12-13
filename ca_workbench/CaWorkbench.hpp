#ifndef CAWORKBENCH_HPP
#define CAWORKBENCH_HPP

#include <iostream>
#include "CaWorkbenchRenderWindow.hpp"
#include "OneDimCaModule.hpp"
#include "TwoDimCaModule.hpp"
#include "RandomBooleanNetworkModule.hpp"
#include "NeuralNetworkModule.hpp"
#include "CaWorkbenchControlUi.hpp"

class CaWorkbench {

public:
	CaWorkbench();
	~CaWorkbench();

private:
	CaWorkbenchModule* module;
	CaWorkbenchControlUi* uiWindow;
	CaWorkbenchRenderWindow* renderWindow;
};

#endif
