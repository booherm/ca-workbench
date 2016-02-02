#include "CaWorkbenchControlUi.hpp"

CaWorkbenchControlUi::CaWorkbenchControlUi(CaWorkbenchModule* module, CaWorkbenchRenderWindow* renderWindow)
	: AwesomiumUiWindow(1024, 768, "CA Workbench", "file:///c:/projects/vs_workspace/ca_workbench/ca_workbench/web_ui/ca_workbench.html")
{
	this->module = module;
	this->renderWindow = renderWindow;
}

void CaWorkbenchControlUi::onWindowDestroy() {
	renderWindow->handleInputCommand("closeWindow");
}

void CaWorkbenchControlUi::refreshConfig(WebView* caller, const JSArray& args) {
	Json::Value configJson(Json::objectValue);
	module->getConfigJson(configJson);
	executeJs("CawbUi.refreshConfigCallback(" + configJson.toStyledString() + ");");
}

void CaWorkbenchControlUi::refreshState(WebView* caller, const JSArray& args) {
	Json::Value stateJson(Json::objectValue);
	module->getStateJson(stateJson);
	executeJs("CawbUi.refreshStateCallback(" + stateJson.toStyledString() + ");");
}

void CaWorkbenchControlUi::setConfigValue(WebView* caller, const JSArray& args) {
	CaWorkbenchModule::ConfigSetting newSetting;
	newSetting.key = ToString(args.At(0).ToString());
	newSetting.value = ToString(args.At(1).ToString());

	module->enqueueConfigChange(newSetting);

	std::cout << "received call to set " << newSetting.key << " to value " << newSetting.value << std::endl;
}

void CaWorkbenchControlUi::sendRenderWindowCommand(WebView* caller, const JSArray& args) {

	std::string command = ToString(args.At(0).ToString());
	std::cout << "received call to send render window command " << command << std::endl;

	renderWindow->handleInputCommand(command);
}

void CaWorkbenchControlUi::bindJsFunctions() {

	JSObject scopeObj = createGlobalJsObject(std::string("CaWorkbench"));

	bindJsFunction(scopeObj, std::string("refreshConfig"), JSDelegate(this, &CaWorkbenchControlUi::refreshConfig));
	bindJsFunction(scopeObj, std::string("refreshState"), JSDelegate(this, &CaWorkbenchControlUi::refreshState));
	bindJsFunction(scopeObj, std::string("setConfigValue"), JSDelegate(this, &CaWorkbenchControlUi::setConfigValue));
	bindJsFunction(scopeObj, std::string("sendRenderWindowCommand"), JSDelegate(this, &CaWorkbenchControlUi::sendRenderWindowCommand));
}
