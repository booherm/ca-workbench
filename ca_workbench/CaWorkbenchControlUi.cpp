#include "CaWorkbenchControlUi.hpp"

CaWorkbenchControlUi::CaWorkbenchControlUi(CaWorkbenchModule* module) : AwesomiumUiWindow(1024, 768, "CA Workbench", "file:///c:/1/ca_workbench.html") {
	this->module = module;
}

void CaWorkbenchControlUi::bindJsFunctions() {

	JSObject scopeObj = createGlobalJsObject(std::string("CaWorkbench"));

	bindJsFunction(scopeObj, std::string("refreshConfig"), JSDelegate(this, &CaWorkbenchControlUi::refreshConfig));
	bindJsFunction(scopeObj, std::string("refreshState"), JSDelegate(this, &CaWorkbenchControlUi::refreshState));

}

void CaWorkbenchControlUi::refreshConfig(WebView* caller, const JSArray& args) {
	Json::Value configJson(Json::objectValue);
	module->getConfigJson(configJson);
	//std::string reply = executeJs("CawbUi.refreshConfigCallback(" + configJson.toStyledString() + ");");
	executeJs("CawbUi.refreshConfigCallback(" + configJson.toStyledString() + ");");
}

void CaWorkbenchControlUi::refreshState(WebView* caller, const JSArray& args) {
	Json::Value stateJson(Json::objectValue);
	module->getStateJson(stateJson);
	//std::string reply = executeJs("CawbUi.refreshStateCallback(" + stateJson.toStyledString() + ");");
	executeJs("CawbUi.refreshStateCallback(" + stateJson.toStyledString() + ");");
}
