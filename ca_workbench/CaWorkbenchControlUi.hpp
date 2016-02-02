#ifndef CAWORKBENCHCONTROLUI_HPP
#define CAWORKBENCHCONTROLUI_HPP

#include "CaWorkbenchRenderWindow.hpp"
#include "AwesomiumUiWindow.hpp"
#include "CaWorkbenchModule.hpp"
#include "NeuralNetworkModule.hpp"
#include "json.hpp"

class CaWorkbenchControlUi : public AwesomiumUiWindow
{

public:
	CaWorkbenchControlUi(CaWorkbenchModule* module, CaWorkbenchRenderWindow* renderWindow);
	void onWindowDestroy();

private:
	void refreshConfig(WebView* caller, const JSArray& args);
	void refreshState(WebView* caller, const JSArray& args);
	void setConfigValue(WebView* caller, const JSArray& args);
	void sendRenderWindowCommand(WebView* caller, const JSArray& args);
	void bindJsFunctions();

	CaWorkbenchModule* module;
	CaWorkbenchRenderWindow* renderWindow;
};

#endif
