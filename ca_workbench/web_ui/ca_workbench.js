Ext.ns("CawbUi");

CawbUi.refreshModuleStateCallback = function (stateJson) {

    if (CawbUi.autoIterateField.getValue()) {
        // get state variables
        var iteration = stateJson.iteration;
        var globalFiringRate = stateJson.globalFiringRate;
        var globalAvgFiringThreshold = stateJson.globalAverageFiringThreshold;
        var globalAvgInputWeight = stateJson.globalAverageInputWeight;

        // set field values
        CawbUi.iterationField.setValue(iteration);
        CawbUi.renderCompleteField.setValue(stateJson.renderComplete);
        CawbUi.globalFiringRateField.setValue(globalFiringRate);
        CawbUi.globalAverageFiringThreshold.setValue(globalAvgFiringThreshold);
        CawbUi.globalAverageInputWeight.setValue(globalAvgInputWeight);

        // round for chart
        globalFiringRate = (globalFiringRate * 1000) / 1000;
        globalAvgFiringThreshold = (globalAvgFiringThreshold * 1000) / 1000;
        globalAvgInputWeight = (globalAvgInputWeight * 1000) / 1000;

        // add new point, rotate out the first
        CawbUi.globalFiringRateData.shift();
        CawbUi.globalFiringRateData.push({ x: iteration, y: globalFiringRate });
        CawbUi.globalAvgFiringThresholdData.shift();
        CawbUi.globalAvgFiringThresholdData.push({ x: iteration, y: globalAvgFiringThreshold });
        CawbUi.globalAvgInputWeightData.shift();
        CawbUi.globalAvgInputWeightData.push({ x: iteration, y: globalAvgInputWeight });

        // set on chart
        CawbUi.aggAnalysisChart.series[0].setData(CawbUi.globalFiringRateData, false, false, false);
        CawbUi.aggAnalysisChart.series[1].setData(CawbUi.globalAvgFiringThresholdData, false, false, false);
        CawbUi.aggAnalysisChart.series[2].setData(CawbUi.globalAvgInputWeightData, false, false, false);

        // redraw chart
        CawbUi.aggAnalysisChart.redraw();
    }
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
	CawbUi.initialExternalInputSynapseWeightField.setValue(configJson.initialExternalInputSynapseWeight);
	CawbUi.initialInternalSynapseWeightField.setValue(configJson.initialInternalSynapseWeight);
	CawbUi.firingRateSampleIterationsField.setValue(configJson.firingRateSampleIterations);
	CawbUi.firingRateThresholdAdjustmentDeltaField.setValue(configJson.firingRateThresholdAdjustmentDelta);
	CawbUi.synapseWeightStrengthenDeltaField.setValue(configJson.synapseWeightStrengthenDelta);
	CawbUi.synapseWeightWeakenDeltaField.setValue(configJson.synapseWeightWeakenDelta);
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
    /////////////////////////////////// module configuration /////////////////////////////////////////////
	CawbUi.rowsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Rows",
		labelWidth: 200
	});
	
	CawbUi.columnsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Columns",
		labelWidth: 200
	});
	
	CawbUi.connectivityField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Connectivity",
		itemId: "connectivity",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 200,
        minValue: 0,
        maxValue: 20,
		allowDecimals: false,
		listeners: {change: CawbUi.updateModuleConfigField}
	});

	CawbUi.externalInputRowCountField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "External Input Row Count",
		itemId: "externalInputRowCount",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 200,
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
		labelWidth: 200,
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
		labelWidth: 200,
        minValue: 0,
        maxValue: 1000,
		allowDecimals: false,
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.autoFeedForwardField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto Feed Forward",
		itemId: "autoFeedForward",
		labelWidth: 200,
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.autoNewInputField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto New Input",
		itemId: "autoNewInput",
		labelWidth: 200,
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.fadeStaleSitesField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Fade Stale Sites",
	    itemId: "fadeStaleSites",
	    labelWidth: 200,
	    listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.activeExternalInputSitePatternIdField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Active Ext. Input Site Pattern ID",
	    itemId: "activeExternalInputSitePatternId",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 200,
	    minValue: 0,
	    maxValue: 3,
	    step: 1,
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

	CawbUi.initialExternalInputSynapseWeightField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Initial Ext. Input Synapse Weight",
	    itemId: "initialExternalInputSynapseWeight",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: -5000,
	    maxValue: 5000,
	    step: 0.1,
	    decimalPrecision: 3,
	    listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.initialInternalSynapseWeightField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Initial Internal Synapse Weight",
	    itemId: "initialInternalSynapseWeight",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: -5000,
	    maxValue: 5000,
	    step: 0.1,
	    decimalPrecision: 3,
	    listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.synapseWeightStrengthenDeltaField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Synapse Weight Strengthen Delta",
		itemId: "synapseWeightStrengthenDelta",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -1000,
		maxValue: 1000,
		step: 0.1,
		decimalPrecision: 3,
		listeners: { change: CawbUi.updateModuleConfigField }
	});

	CawbUi.synapseWeightWeakenDeltaField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Synapse Weight Weaken Delta",
	    itemId: "synapseWeightWeakenDelta",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: -1000,
	    maxValue: 1000,
	    step: 0.1,
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

	CawbUi.configurationFieldSet = Ext.create("Ext.form.FieldSet", {
		title: "Configuration",
		width: 825,
		layout: { type: "table", columns: 3, tdAttrs: { style: { verticalAlign: "top" } } },
		items: [
			CawbUi.rowsField,
            {xtype: "component", width: 20},
            CawbUi.initialNeuronFiringThresholdField,
			CawbUi.columnsField,
            {xtype: "component", width: 20},
            CawbUi.targetFiringRateField,
			CawbUi.connectivityField,
            {xtype: "component", width: 20},
            CawbUi.firingRateSampleIterationsField,
			CawbUi.externalInputRowCountField,
            {xtype: "component", width: 20},
            CawbUi.firingRateThresholdAdjustmentDeltaField,
			CawbUi.feedbackInputRowCountField,
            {xtype: "component", width: 20},
            CawbUi.initialExternalInputSynapseWeightField,
			CawbUi.externalOutputRowCountField,
            {xtype: "component", width: 20},
            CawbUi.initialInternalSynapseWeightField,
			CawbUi.autoFeedForwardField,
            {xtype: "component", width: 20},
            CawbUi.synapseWeightStrengthenDeltaField,
			CawbUi.autoNewInputField,
            {xtype: "component", width: 20},
            CawbUi.synapseWeightWeakenDeltaField,
            CawbUi.fadeStaleSitesField,
            {xtype: "component", width: 20},
            CawbUi.minSynapseWeightField,
            CawbUi.activeExternalInputSitePatternIdField,
            {xtype: "component", width: 20},
            CawbUi.maxSynapseWeightField
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
    CawbUi.reloadButton = Ext.create("Ext.Button", {
        text: "Reload Page",
        width: 90,
        handler: function () {
            document.location.reload();
        }
    });

    CawbUi.resetButton = Ext.create("Ext.Button", {
        text: "Reset",
        width: 90,
        margin: "0 10 15 10",
        handler: function () {
            clearInterval(CawbUi.refreshInterval);
            CawbUi.resetInitialChartData();
            CaWorkbench.setModuleConfigValue("reset", null);
            CawbUi.refreshInterval = setInterval(function () { CaWorkbench.refreshModuleState(); }, 500);
        }
    });

    CawbUi.moduleControlButtonsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
        layout: { type: "hbox" },
        items: [
			CawbUi.resetButton,
            CawbUi.reloadButton
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
        width: 350,
        items: [
			CawbUi.iterationField,
			CawbUi.renderCompleteField,
			CawbUi.globalFiringRateField,
			CawbUi.globalAverageFiringThreshold,
			CawbUi.globalAverageInputWeight
        ]
    });

    ////////////////////////////////////////// layout /////////////////////////////////////////////
	CawbUi.moduleMiddlePanelsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
	    layout: {type: "vbox", align: "stretch"},
	    width: 325,
	    items: [
            CawbUi.renderWindowControlsFieldSet,
            CawbUi.moduleControlsFieldSet
        ]
	});    

	CawbUi.modulePanel = Ext.create("Sms.form.Panel", {
		region: "center",
		title: "Module",
		layout: {type: "table", columns: 6, tdAttrs: {style: {verticalAlign: "top"}}},
		items:  [
			{xtype: "component", width: 20},
			CawbUi.configurationFieldSet,
			{xtype: "component", width: 20},
			CawbUi.moduleMiddlePanelsContainer,
            { xtype: "component", width: 20 },
            CawbUi.stateFieldSet
		]
	});

	CawbUi.chartsPanel = Ext.create("Sms.form.Panel", {
	    region: "south",
	    title: "Analysis",
        height: 655,
	    html: "<div id='chart_container' style='height: 610px;'></div>"
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
	CawbUi.refreshInterval = setInterval(function () { CaWorkbench.refreshModuleState(); }, 500);
	CaWorkbench.refreshModuleConfig();
	CaWorkbench.refreshRenderWindowState();
};

CawbUi.resetInitialChartData = function () {
    var pointsToDisplay = 100;
    CawbUi.globalFiringRateData = [];
    CawbUi.globalAvgFiringThresholdData = [];
    CawbUi.globalAvgInputWeightData = [];

    for (var i = -pointsToDisplay; i < 0; i++) {
        CawbUi.globalFiringRateData.push({ x: i, y: null });
        CawbUi.globalAvgFiringThresholdData.push({ x: i, y: null });
        CawbUi.globalAvgInputWeightData.push({ x: i, y: null });
    }

    CawbUi.aggAnalysisChart.series[0].setData(CawbUi.globalFiringRateData, false, false, false);
    CawbUi.aggAnalysisChart.series[1].setData(CawbUi.globalAvgFiringThresholdData, false, false, false);
    CawbUi.aggAnalysisChart.series[1].setData(CawbUi.globalAvgInputWeightData, false, false, false);
    CawbUi.aggAnalysisChart.redraw();
};

CawbUi.initChart = function () {
    CawbUi.aggAnalysisChart = new Highcharts.Chart({
        credits: {enabled: false},
        chart: {
            animation: false,
            renderTo: "chart_container",
            style: {
                fontFamily: "Lucida Console",
                fontWeight: "bold"
            }
        },
        title: {
            text: "Aggregate Analysis"
        },
        xAxis: {
            title: { text: "Iteration" },
            type: "category"
        },
        yAxis: [{
            id: "globalFiringRateAxis",
            title: { text: "Global Firing Rate" },
            labels: { format: "{value:,.2f}" },
            min: 0, 
            max: 1
        },
        {
            id: "globalAvgFiringThresholdAxis",
            title: { text: "Global Avg. Firing Threshold" },
            labels: { format: "{value:,.2f}" },
            opposite: true
        },
        {
            id: "globalAvgInputWeightAxis",
            title: { text: "Global Avg. Input Weight" },
            labels: { format: "{value:,.2f}" },
            opposite: true
        }],
        series: [{
            type: "line",
            name: "Global Firing Rate",
            yAxis: "globalFiringRateAxis"
        },
        {
            type: "line",
            name: "Global Avg. Firing Threshold",
            yAxis: "globalAvgFiringThresholdAxis"
        },
        {
            type: "line",
            name: "Global Avg. Input Weight",
            yAxis: "globalAvgInputWeightAxis"
        }],
        tooltip: { enabled: false }
    });

    CawbUi.resetInitialChartData();
};

Ext.onReady(function(){
	CawbUi.init();
});
