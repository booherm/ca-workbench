Ext.ns("CaWorkbenchUi");
Ext.ns("TwoDimCaModule");
Ext.ns("NeuralNetModule");

CaWorkbenchUi.init = function()
{
    var twoDimCaModuleTab = TwoDimCaModule.init();
    var neuralNetModuleTab = NeuralNetModule.init();
	
	var mainTabPanel = Ext.create("Ext.tab.Panel", {
		items: [twoDimCaModuleTab, neuralNetModuleTab]
	});

	CaWorkbenchUi.viewport = Ext.create("Ext.container.Viewport", {
		items: [mainTabPanel]
	});
	
};

Ext.onReady(function(){
	CaWorkbenchUi.init();
});
