#include <iostream>
#include "AudioManager.hpp"
#include "CaWorkbench.hpp"
#include "OneDimCaModule.hpp"
#include "RandomBooleanNetworkModule.hpp"
#include "NeuralNetworkModule.hpp"
#include "AwesomiumUiWindow.hpp"

int main()
{
	try {
		cout << "Starting CA Workbench" << endl;

		//CaWorkbenchModule* module = new OneDimCaModule(800, 1600, 0);
		//CaWorkbenchModule* module = new RandomBooleanNetworkModule(100, 200, 3, 23, 4, 4, false, true);
		//CaWorkbenchModule* module = new NeuralNetworkModule(200, 400, 3, 20, 20, 20, false, true);
		//CaWorkbench caWb = CaWorkbench(module);
		//caWb.doRenderLoop();

		AwesomiumUiWindow uiWindow(800, 600, "CA Workbench");
		uiWindow.threadStart();
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

		cout << "Exiting CA Workbench" << endl;
	}
	catch (string e) {
		cout << e << endl;
	}

	system("pause");

	return 0;
}
