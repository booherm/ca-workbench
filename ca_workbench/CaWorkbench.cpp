#include "CaWorkbench.hpp"

CaWorkbench::CaWorkbench()
{

	CaWorkbenchControlUi uiWindow;
	uiWindow.threadStart();

	//module = new OneDimCaModule(800, 1600, 0);
	//module = new RandomBooleanNetworkModule(100, 200, 3, 23, 4, 4, false, true);
	module = new NeuralNetworkModule(200, 400, 3, 20, 20, 20, false, true);
	renderWindow = new CaWorkbenchRenderWindow(module);
	renderWindow->doRenderLoop();

	uiWindow.threadJoin();

	/*
	//AudioManager am = AudioManager();
	//am.printDeviceInformation();
	//am.startInputStream();
	//am.stopInputStream();
	//system("pause");
	//am.startOutputStream();
	//am.stopOutputStream();
	//am.shutdown();
	*/

}

CaWorkbench::~CaWorkbench()
{
	delete renderWindow;
	delete module;
}
