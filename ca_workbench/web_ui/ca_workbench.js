Ext.ns("CawbUi");

CawbUi.refreshStateCallback = function(stateJson){
	CawbUi.iterationField.setValue(stateJson.iteration);
	CawbUi.renderCompleteField.setValue(stateJson.renderComplete);
	CawbUi.globalFiringRateField.setValue(stateJson.globalFiringRate);
	CawbUi.globalFiringRateCalcField.setValue(stateJson.globalFiringRateCalcActivations);
	CawbUi.globalAverageFiringThreshold.setValue(stateJson.globalAverageFiringThreshold);
	CawbUi.globalAverageInputWeight.setValue(stateJson.globalAverageInputWeight);
};

CawbUi.refreshConfigCallback = function(configJson){
	CawbUi.refreshConfigInProgress = true;
	CawbUi.rowsField.setValue(configJson.rows);
	CawbUi.columnsField.setValue(configJson.columns);
	CawbUi.connectivityField.setValue(configJson.connectivity);
	CawbUi.externalInputRowCountField.setValue(configJson.externalInputRowCount);
	CawbUi.feedbackInputRowCountField.setValue(configJson.feedbackInputRowCount);
	CawbUi.externalOutputRowCountField.setValue(configJson.externalOutputRowCount);
	CawbUi.neighborhoodConnectionsField.setValue(configJson.neighborhoodConnections);
	CawbUi.autoFeedForwardField.setValue(configJson.autoFeedForward);
	CawbUi.autoNewInputField.setValue(configJson.autoNewInput);
	CawbUi.targetFiringRateField.setValue(configJson.targetFiringRate);
	CawbUi.initialNeuronFiringThresholdField.setValue(configJson.initialNeuronFiringThreshold);
	CawbUi.firingRateSampleIterationsField.setValue(configJson.firingRateSampleIterations);
	CawbUi.firingRateThresholdAdjustmentDeltaField.setValue(configJson.firingRateThresholdAdjustmentDelta);
	CawbUi.synapseWeightAdjustmentDeltaField.setValue(configJson.synapseWeightAdjustmentDelta);
	CawbUi.minSynapseWeightField.setValue(configJson.minSynapseWeight);
	CawbUi.maxSynapseWeightField.setValue(configJson.maxSynapseWeight);
	CawbUi.activeExternalInputSitePatternIdField.setValue(configJson.activeExternalInputSitePatternId);
	CawbUi.refreshConfigInProgress = false;
};

CawbUi.updateConfigField = function(field, newValue){
	if(!CawbUi.refreshConfigInProgress && field.isValid())
		CaWorkbench.setConfigValue(field.getItemId(), newValue);
};

CawbUi.sendRenderWindowCommand = function (command) {
    CaWorkbench.sendRenderWindowCommand(command);
};

CawbUi.init = function()
{
	CawbUi.reloadButton = Ext.create("Ext.Button", {
		text: "Reload Page",
		handler: function() {
			document.location.reload();
		}
	});

	CawbUi.refreshConfigButton = Ext.create("Ext.Button", {
		text: "Reload Config",
		handler: function() {
			CaWorkbench.refreshConfig();
		}
	});

	CawbUi.rowsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Rows",
		labelWidth: 250
	});
	
	CawbUi.columnsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Columns",
		labelWidth: 250
	});
	
	CawbUi.connectivityField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Connectivity",
		itemId: "connectivity",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
        minValue: 0,
        maxValue: 99,
		allowDecimals: false,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.externalInputRowCountField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "External Input Row Count",
		itemId: "externalInputRowCount",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
        minValue: 1,
        maxValue: 1000,
		allowDecimals: false,
		listeners: {change: CawbUi.updateConfigField}
	});
	
	CawbUi.feedbackInputRowCountField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Feedback Input Row Count",
		itemId: "feedbackInputRowCount",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
        minValue: 0,
        maxValue: 1000,
		allowDecimals: false,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.externalOutputRowCountField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "External Output Row Count",
		itemId: "externalOutputRowCount",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
        minValue: 0,
        maxValue: 1000,
		allowDecimals: false,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.neighborhoodConnectionsField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Neighborhood Connections",
		itemId: "neighborhoodConnections",
		labelWidth: 250,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.autoFeedForwardField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto Feed Forward",
		itemId: "autoFeedForward",
		labelWidth: 250,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.autoNewInputField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto New Input",
		itemId: "autoNewInput",
		labelWidth: 250,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.targetFiringRateField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Target Firing Rate",
		itemId: "targetFiringRate",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: 0,
		maxValue: 1,
		step: 0.1,
		decimalPrecision: 3,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.initialNeuronFiringThresholdField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Initial Neuron Firing Threshold",
		itemId: "initialNeuronFiringThreshold",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -1000,
		maxValue: 1000,
		step: 1,
		decimalPrecision: 3,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.firingRateSampleIterationsField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Firing Rate Sample Iterations",
		itemId: "firingRateSampleIterations",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
        minValue: 0,
        maxValue: 1000,
		allowDecimals: false,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.firingRateThresholdAdjustmentDeltaField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Firing Rate Threshold Adjustment Delta",
		itemId: "firingRateThresholdAdjustmentDelta",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: 0,
		maxValue: 1000,
		step: 1,
		decimalPrecision: 3,
		listeners: {change: CawbUi.updateConfigField}
	});
	
	CawbUi.synapseWeightAdjustmentDeltaField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Synapse Weight Adjustment Delta",
		itemId: "synapseWeightAdjustmentDelta",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: 0,
		maxValue: 1000,
		step: 1,
		decimalPrecision: 3,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.minSynapseWeightField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Minimum Synapse Weight",
		itemId: "minimumSynapseWeight",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -10000,
		maxValue: 10000,
		step: 1,
		decimalPrecision: 3,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.maxSynapseWeightField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Maximum Synapse Weight",
		itemId: "maxmumSynapseWeight",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -10000,
		maxValue: 10000,
		step: 1,
		decimalPrecision: 3,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.activeExternalInputSitePatternIdField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Active External Input Site Pattern ID",
		itemId: "activeExternalInputSitePatternId",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: 0,
		maxValue: 3,
		step: 1,
		listeners: {change: CawbUi.updateConfigField}
	});

	CawbUi.configurationFieldSet = Ext.create("Ext.form.FieldSet", {
		title: "Configuration",
		width: 500,
		padding: "0 0 0 20",
		items: [
			CawbUi.rowsField,
			CawbUi.columnsField,
			CawbUi.connectivityField,
			CawbUi.externalInputRowCountField,
			CawbUi.feedbackInputRowCountField,
			CawbUi.externalOutputRowCountField,
			CawbUi.neighborhoodConnectionsField,
			CawbUi.autoFeedForwardField,
			CawbUi.autoNewInputField,
			CawbUi.targetFiringRateField,
			CawbUi.initialNeuronFiringThresholdField,
			CawbUi.firingRateSampleIterationsField,
			CawbUi.firingRateThresholdAdjustmentDeltaField,
			CawbUi.synapseWeightAdjustmentDeltaField,
			CawbUi.minSynapseWeightField,
			CawbUi.maxSynapseWeightField,
			CawbUi.activeExternalInputSitePatternIdField,
			CawbUi.reloadButton,
			CawbUi.refreshConfigButton
		]
	});

	CawbUi.iterationField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Iteration",
		labelWidth: 200
	});

	CawbUi.renderCompleteField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Render Complete",
		labelWidth: 200
	});

	CawbUi.globalFiringRateField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Global Firing Rate",
		labelWidth: 200
	});
	
	CawbUi.globalFiringRateCalcField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Global Firing Rate Calc Activations",
		labelWidth: 200
	});

	CawbUi.globalAverageFiringThreshold = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Global Average Firing Threshold",
		labelWidth: 200
	});

	CawbUi.globalAverageInputWeight = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Global Average Input Weight",
		labelWidth: 200
	});

	CawbUi.stateFieldSet = Ext.create("Ext.form.FieldSet", {
	    title: "State",
		items: [
			CawbUi.iterationField,
			CawbUi.renderCompleteField,
			CawbUi.globalFiringRateField,
			CawbUi.globalFiringRateCalcField,
			CawbUi.globalAverageFiringThreshold,
			CawbUi.globalAverageInputWeight
		]
	});
	
	CawbUi.pauseButton = Ext.create("Ext.Button", {
	    text: "Pause",
	    handler: function () {
	        CawbUi.sendRenderWindowCommand("togglePaused");
	    }
	});

	CawbUi.controlsFieldSet = Ext.create("Ext.form.FieldSet", {
	    title: "Controls",
        height: 100,
	    items: [
			CawbUi.pauseButton
	    ]
	});

	CawbUi.moduleRightPanelsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
	    layout: {type: "vbox", align: "stretch"},
	    width: 450,
	    items: [
            CawbUi.stateFieldSet,
            CawbUi.controlsFieldSet
        ]
	});    

	CawbUi.modulePanel = Ext.create("Sms.form.Panel", {
		region: "center",
		title: "Module",
		layout: {type: "table", columns: 4, tdAttrs: {style: {verticalAlign: "top"}}},
		items:  [
			{xtype: "component", width: 20},
			CawbUi.configurationFieldSet,
			{xtype: "component", width: 20},
			CawbUi.moduleRightPanelsContainer
		]
	});

	CawbUi.viewport = Ext.create("Ext.container.Viewport", {
		layout: "border",
		items:  [CawbUi.modulePanel]
	});

	setInterval(function(){CaWorkbench.refreshState();}, 50);
	CawbUi.refreshConfigInProgress = false;
	CaWorkbench.refreshConfig();
};

Ext.onReady(function(){
	
	CawbUi.init();
});
