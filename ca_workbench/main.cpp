#include <iostream>

#include "CaWorkbench.hpp"

int main()
{
	try {
		cout << "Starting CA Workbench" << endl;

		CaWorkbench cawb;

		cout << "Exiting CA Workbench" << endl;
	}
	catch (string e) {
		cout << e << endl;
		system("pause");
	}

	return 0;
}
