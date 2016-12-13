#include "CaWorkbenchControlUi.hpp"

CaWorkbenchControlUi::CaWorkbenchControlUi(CaWorkbenchModule* module, CaWorkbenchRenderWindow* renderWindow)
	: AwesomiumUiWindow(1500, 550, "CA Workbench", "file:///c:/projects/vs_workspace/ca_workbench/ca_workbench/web_ui/ca_workbench.html")
{
	this->module = module;
	this->renderWindow = renderWindow;
}

void CaWorkbenchControlUi::onWindowDestroy() {
	renderWindow->handleInputCommand("closeWindow");
}

void CaWorkbenchControlUi::refreshModuleConfig(WebView* caller, const JSArray& args) {
	Json::Value configJson(Json::objectValue);
	module->getConfigJson(configJson);
	std::string callbackJsFunction = ToString(args.At(0).ToString());
	executeJs(callbackJsFunction + "(" + configJson.toStyledString() + ");");
}

void CaWorkbenchControlUi::refreshModuleState(WebView* caller, const JSArray& args) {
	Json::Value stateJson(Json::objectValue);
	module->getStateJson(stateJson);
	std::string callbackJsFunction = ToString(args.At(0).ToString());
	executeJs(callbackJsFunction + "(" + stateJson.toStyledString() + ");");
}

void CaWorkbenchControlUi::refreshRenderWindowState(WebView* caller, const JSArray& args) {
	Json::Value stateJson(Json::objectValue);
	renderWindow->getStateJson(stateJson);
	std::string callbackJsFunction = ToString(args.At(0).ToString());
	executeJs(callbackJsFunction + "(" + stateJson.toStyledString() + ");");
}

void CaWorkbenchControlUi::setModuleConfigValue(WebView* caller, const JSArray& args) {
	CaWorkbenchModule::ConfigSetting newSetting;
	newSetting.key = ToString(args.At(0).ToString());
	newSetting.value = ToString(args.At(1).ToString());

	module->enqueueConfigChange(newSetting);
}

void CaWorkbenchControlUi::sendRenderWindowCommand(WebView* caller, const JSArray& args) {
	std::string command = ToString(args.At(0).ToString());
	renderWindow->handleInputCommand(command);

	Json::Value stateJson(Json::objectValue);
	renderWindow->getStateJson(stateJson);
	std::string callbackJsFunction = ToString(args.At(1).ToString());
	executeJs(callbackJsFunction + "(" + stateJson.toStyledString() + ");");
}

void CaWorkbenchControlUi::bindJsFunctions() {
	JSObject scopeObj = createGlobalJsObject(std::string("CaWorkbench"));
	bindJsFunction(scopeObj, std::string("refreshModuleConfig"), JSDelegate(this, &CaWorkbenchControlUi::refreshModuleConfig));
	bindJsFunction(scopeObj, std::string("refreshModuleState"), JSDelegate(this, &CaWorkbenchControlUi::refreshModuleState));
	bindJsFunction(scopeObj, std::string("refreshRenderWindowState"), JSDelegate(this, &CaWorkbenchControlUi::refreshRenderWindowState));
	bindJsFunction(scopeObj, std::string("setModuleConfigValue"), JSDelegate(this, &CaWorkbenchControlUi::setModuleConfigValue));
	bindJsFunction(scopeObj, std::string("sendRenderWindowCommand"), JSDelegate(this, &CaWorkbenchControlUi::sendRenderWindowCommand));
}
