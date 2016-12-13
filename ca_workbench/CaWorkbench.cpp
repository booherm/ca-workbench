#include "CaWorkbench.hpp"

CaWorkbench::CaWorkbench()
{
	//module = new OneDimCaModule(800, 1600, 0);
	module = new TwoDimCaModule(400, 800);
	//module = new RandomBooleanNetworkModule(100, 200, 3, 23, 4, 4, false, true);
	//module = new NeuralNetworkModule(200, 400);
	//module = new NeuralNetworkModule(10, 20, 3, 1, 1, 1, false, true);

	renderWindow = new CaWorkbenchRenderWindow(module);
	uiWindow = new CaWorkbenchControlUi(module, renderWindow);

	uiWindow->threadStart();

	Sleep(5000);
	renderWindow->doRenderLoop();

	uiWindow->threadJoin();
}

CaWorkbench::~CaWorkbench()
{
	delete uiWindow;
	delete renderWindow;
	delete module;
}
