#ifndef CAWORKBENCHCONTROLUI_HPP
#define CAWORKBENCHCONTROLUI_HPP

#include "AwesomiumUiWindow.hpp"
#include "CaWorkbenchModule.hpp"
#include "json.hpp"

class CaWorkbenchControlUi : public AwesomiumUiWindow
{

public:
	CaWorkbenchControlUi(CaWorkbenchModule* module);

private:
	void refreshConfig(WebView* caller, const JSArray& args);
	void refreshState(WebView* caller, const JSArray& args);
	void bindJsFunctions();

	CaWorkbenchModule* module;
};

#endif
