#include <iostream>
#include "AudioManager.hpp"
#include "CaWorkbench.hpp"
#include "RandomBooleanNetworkModule.hpp"

int main()
{
	try {
		cout << "Starting CA Workbench" << endl;

		CaWorkbenchModule* module = new RandomBooleanNetworkModule(200, 400, 3, 80, 10, 10, false, true);
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
