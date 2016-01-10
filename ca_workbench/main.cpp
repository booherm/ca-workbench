#include <iostream>
#include "CaWorkbench.hpp"

using namespace std;

int main(void)
{
	try {
		cout << "Starting CA Workbench" << endl;

		CaWorkbench caWb = CaWorkbench();
		caWb.doRenderLoop();
		//RandomBooleanNetwork* rbn = new RandomBooleanNetwork(10, 10, 2);
		//delete rbn;

		cout << "Exiting CA Workbench" << endl;
		system("pause");
	}
	catch (string e) {
		cout << e << endl;
		system("pause");
	}

	return 0;
}
