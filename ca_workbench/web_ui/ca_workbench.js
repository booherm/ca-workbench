Ext.ns("CawbUi");

CawbUi.refreshModuleStateCallback = function(stateJson){
	CawbUi.iterationField.setValue(stateJson.iteration);
	CawbUi.renderCompleteField.setValue(stateJson.renderComplete);
	CawbUi.globalFiringRateField.setValue(stateJson.globalFiringRate);
	CawbUi.globalAverageFiringThreshold.setValue(stateJson.globalAverageFiringThreshold);
	CawbUi.globalAverageInputWeight.setValue(stateJson.globalAverageInputWeight);

    // update chart
	var y = (stateJson.globalFiringRate * 1000) / 1000;
	CawbUi.chart1.series[0].addPoint([stateJson.iterationx, y], true, true);
};

CawbUi.refreshModuleConfigCallback = function(configJson){
    CawbUi.refreshModuleConfigInProgress = true;
	CawbUi.rowsField.setValue(configJson.rows);
	CawbUi.columnsField.setValue(configJson.columns);
	CawbUi.connectivityField.setValue(configJson.connectivity);
	CawbUi.externalInputRowCountField.setValue(configJson.externalInputRowCount);
	CawbUi.feedbackInputRowCountField.setValue(configJson.feedbackInputRowCount);
	CawbUi.externalOutputRowCountField.setValue(configJson.externalOutputRowCount);
	CawbUi.autoFeedForwardField.setValue(configJson.autoFeedForward);
	CawbUi.autoNewInputField.setValue(configJson.autoNewInput);
	CawbUi.fadeStaleSitesField.setValue(configJson.fadeStaleSites);
	CawbUi.targetFiringRateField.setValue(configJson.targetFiringRate);
	CawbUi.initialNeuronFiringThresholdField.setValue(configJson.initialNeuronFiringThreshold);
	CawbUi.initialSynapseWeightField.setValue(configJson.initialSynapseWeight);
	CawbUi.firingRateSampleIterationsField.setValue(configJson.firingRateSampleIterations);
	CawbUi.firingRateThresholdAdjustmentDeltaField.setValue(configJson.firingRateThresholdAdjustmentDelta);
	CawbUi.synapseWeightAdjustmentDeltaField.setValue(configJson.synapseWeightAdjustmentDelta);
	CawbUi.minSynapseWeightField.setValue(configJson.minSynapseWeight);
	CawbUi.maxSynapseWeightField.setValue(configJson.maxSynapseWeight);
	CawbUi.activeExternalInputSitePatternIdField.setValue(configJson.activeExternalInputSitePatternId);
	CawbUi.refreshModuleConfigInProgress = false;
};

CawbUi.refreshRenderWindowStateCallback = function (stateJson) {

    CawbUi.refresRenderWindowStateInProgress = true;
    if (stateJson.autoIterate){
        CawbUi.pauseButton.setText(stateJson.paused ? "Unpause" : "Pause");
        CawbUi.pauseButton.setDisabled(false);
    }
    else {
        CawbUi.pauseButton.setText("Pause");
        CawbUi.pauseButton.setDisabled(true);
    }
    CawbUi.stepButton.setDisabled(stateJson.autoIterate);
    CawbUi.autoIterateField.setValue(stateJson.autoIterate);
    CawbUi.gridLinesField.setValue(stateJson.gridLines);
    CawbUi.siteConnectionsField.setValue(stateJson.siteConnections);
    CawbUi.refresRenderWindowStateInProgress = false;
};

CawbUi.updateModuleConfigField = function(field, newValue){
    if (!CawbUi.refreshModuleConfigInProgress && field.isValid())
		CaWorkbench.setModuleConfigValue(field.getItemId(), newValue);
};

CawbUi.sendRenderWindowCommand = function (command) {
    if(!CawbUi.refresRenderWindowStateInProgress)
        CaWorkbench.sendRenderWindowCommand(command);
};

CawbUi.init = function()
{
    ////////////////////////////////////////// debugging buttons /////////////////////////////////////////////
	CawbUi.reloadButton = Ext.create("Ext.Button", {
		text: "Reload Page",
		handler: function() {
			document.location.reload();
		}
	});

	CawbUi.updateChartButton = Ext.create("Ext.Button", {
	    text: "Update Chart",
	    handler: function () {
	        CawbUi.updateChart();
	    }
	});

    /////////////////////////////////// module configuration /////////////////////////////////////////////
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
		listeners: {change: CawbUi.updateModuleConfigField}
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
		listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.autoFeedForwardField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto Feed Forward",
		itemId: "autoFeedForward",
		labelWidth: 250,
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.autoNewInputField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto New Input",
		itemId: "autoNewInput",
		labelWidth: 250,
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.fadeStaleSitesField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Fade Stale Sites",
	    itemId: "fadeStaleSites",
	    labelWidth: 250,
	    listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.initialSynapseWeightField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Initial Synapse Weight",
	    itemId: "initialSynapseWeight",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: -5000,
	    maxValue: 5000,
	    step: 0.1,
	    decimalPrecision: 3,
	    listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.minSynapseWeightField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Minimum Synapse Weight",
		itemId: "minSynapseWeight",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -10000,
		maxValue: 10000,
		step: 1,
		decimalPrecision: 3,
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.maxSynapseWeightField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Maximum Synapse Weight",
		itemId: "maxSynapseWeight",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -10000,
		maxValue: 10000,
		step: 1,
		decimalPrecision: 3,
		listeners: { change: CawbUi.updateModuleConfigField }
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
		listeners: { change: CawbUi.updateModuleConfigField }
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
			CawbUi.autoFeedForwardField,
			CawbUi.autoNewInputField,
            CawbUi.fadeStaleSitesField,
			CawbUi.targetFiringRateField,
			CawbUi.initialNeuronFiringThresholdField,
            CawbUi.initialSynapseWeightField,
			CawbUi.firingRateSampleIterationsField,
			CawbUi.firingRateThresholdAdjustmentDeltaField,
			CawbUi.synapseWeightAdjustmentDeltaField,
			CawbUi.minSynapseWeightField,
			CawbUi.maxSynapseWeightField,
			CawbUi.activeExternalInputSitePatternIdField,
			CawbUi.reloadButton,
            CawbUi.updateChartButton
		]
	});

    ////////////////////////////////////// module state /////////////////////////////////////////////
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
			CawbUi.globalAverageFiringThreshold,
			CawbUi.globalAverageInputWeight
		]
	});
	
    ///////////////////////////////////// render window controls /////////////////////////////////////////////
	CawbUi.pauseButton = Ext.create("Ext.Button", {
	    text: "Pause",
	    width: 90,
	    margin: "0 10 15 10",
	    handler: function () {
	        CawbUi.sendRenderWindowCommand("togglePaused");
	    }
	});

	CawbUi.stepButton = Ext.create("Ext.Button", {
	    text: "Step",
	    width: 90,
	    margin: "0 10 15 0",
	    handler: function () {
	        CawbUi.sendRenderWindowCommand("iterateOneStep");
	    }
	});

	CawbUi.screenShotButton = Ext.create("Ext.Button", {
	    text: "Screen Shot",
	    width: 90,
	    margin: "0 10 15 0",
	    handler: function () {
	        CawbUi.sendRenderWindowCommand("screenShot");
	    }
	});

	CawbUi.autoIterateField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Auto Iterate",
	    labelWidth: 120,
	    itemId: "autoIterate",
	    listeners: { change: function () { CawbUi.sendRenderWindowCommand("toggleAutoIterate"); } }
	});

	CawbUi.gridLinesField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Grid Lines",
	    labelWidth: 120,
	    itemId: "gridLines",
	    listeners: { change: function () { CawbUi.sendRenderWindowCommand("toggleGridLines"); } }
	});

    CawbUi.siteConnectionsField = Ext.create("Ext.form.field.Checkbox", {
        fieldLabel: "Site Connections",
        labelWidth: 120,
        itemId: "siteConnections",
        listeners: { change: function () { CawbUi.sendRenderWindowCommand("toggleSiteConnections"); } }
    });

    CawbUi.renderWindowControlButtonsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
        layout: { type: "hbox" },
        items: [
			CawbUi.pauseButton,
            CawbUi.stepButton,
            CawbUi.screenShotButton
        ]
    });

    CawbUi.renderWindowControlsFieldSet = Ext.create("Ext.form.FieldSet", {
	    title: "Render Window Controls",
	    height: 150,
        layout: {type: "vbox"},
        items: [
            CawbUi.renderWindowControlButtonsContainer,
            CawbUi.autoIterateField,
            CawbUi.gridLinesField,
            CawbUi.siteConnectionsField
	    ]
	});

    ///////////////////////////////////// module controls /////////////////////////////////////////////
    CawbUi.resetButton = Ext.create("Ext.Button", {
        text: "Reset",
        width: 90,
        margin: "0 10 15 10",
        handler: function () {
            CaWorkbench.setModuleConfigValue("reset", null);
        }
    });

    CawbUi.moduleControlButtonsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
        layout: { type: "hbox" },
        items: [
			CawbUi.resetButton
        ]
    });

    CawbUi.moduleControlsFieldSet = Ext.create("Ext.form.FieldSet", {
        title: "Module Controls",
        height: 100,
        layout: { type: "vbox" },
        items: [
            CawbUi.moduleControlButtonsContainer
        ]
    });

    ////////////////////////////////////////// layout /////////////////////////////////////////////
	CawbUi.moduleRightPanelsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
	    layout: {type: "vbox", align: "stretch"},
	    width: 450,
	    items: [
            CawbUi.stateFieldSet,
            CawbUi.renderWindowControlsFieldSet,
            CawbUi.moduleControlsFieldSet
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

	CawbUi.chartsPanel = Ext.create("Sms.form.Panel", {
	    region: "south",
	    title: "Analysis",
        height: 400,
	    html: "<div id='chart_container'></div>"
	});

	CawbUi.viewport = Ext.create("Ext.container.Viewport", {
		layout: "border",
		items: [
           CawbUi.modulePanel,
           CawbUi.chartsPanel
		]
	});

	CawbUi.initChart();
	CawbUi.refreshModuleConfigInProgress = false;
	CawbUi.refresRenderWindowStateInProgress = false;
	setInterval(function () { CaWorkbench.refreshModuleState(); }, 50);
	CaWorkbench.refreshModuleConfig();
	CaWorkbench.refreshRenderWindowState();
};

CawbUi.initChart = function () {

    var pointsToDisplay = 100;
    var initialData = [];
    for(var i = 0; i < pointsToDisplay; i++)
        initialData.push(0.0);

    CawbUi.chart1 = new Highcharts.Chart({
        credits: {enabled: false},
        chart: {
            animation: false,
            renderTo: "chart_container"
        },
        title: {
            text: "Global Firing Rate"
        },
        xAxis: {
            title: { text: "Iteration" },
            type: "category"
        },
        yAxis: {
            title: {text: "Global Firing Rate"},
            min: 0,
            max: 1
        },
        series: [{
            type: "line",
            name: "Global Firing Rate",
            data: initialData
        }],
        tooltip: {enabled: false}
    });

};

Ext.onReady(function(){
	
	CawbUi.init();
});
