#include <iostream>
#include "AudioManager.hpp"
#include "CaWorkbench.hpp"
#include "OneDimCaModule.hpp"
#include "RandomBooleanNetworkModule.hpp"
#include "NeuralNetworkModule.hpp"

int main()
{
	try {
		cout << "Starting CA Workbench" << endl;

		//CaWorkbenchModule* module = new OneDimCaModule(800, 1600, 0);
		CaWorkbenchModule* module = new RandomBooleanNetworkModule(10, 20, 3, 1, 1, 1, false, true);
		//CaWorkbenchModule* module = new NeuralNetworkModule(100, 200, 3, 1, 1, 1, false, true);
		CaWorkbench caWb = CaWorkbench(module);
		caWb.doRenderLoop();

		//AudioManager am = AudioManager();
		//am.printDeviceInformation();
		//am.startInputStream();
		//am.stopInputStream();
		//system("pause");
		//am.startOutputStream();
		//am.stopOutputStream();
		//am.shutdown();

		cout << "Exiting CA Workbench" << endl;
	}
	catch (string e) {
		cout << e << endl;
	}

	system("pause");

	return 0;
}
