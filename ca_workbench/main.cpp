#include <iostream>
#include "AudioManager.hpp"
#include "CaWorkbench.hpp"

int main()
{
	try {
		cout << "Starting CA Workbench" << endl;

		CaWorkbench caWb = CaWorkbench();
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
