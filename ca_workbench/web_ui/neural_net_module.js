Ext.ns("NeuralNetModule");

NeuralNetModule.refreshModuleStateCallback = function (stateJson) {

    if (NeuralNetModule.autoIterateField.getValue()) {
        // get state variables
        var iteration = stateJson.iteration;
        var globalFiringRate = stateJson.globalFiringRate;
        var globalAvgFiringThreshold = stateJson.globalAverageFiringThreshold;
        var globalAvgInputWeight = stateJson.globalAverageInputWeight;

        // set field values
        NeuralNetModule.iterationField.setValue(iteration);
        NeuralNetModule.renderCompleteField.setValue(stateJson.renderComplete);
        NeuralNetModule.globalFiringRateField.setValue(globalFiringRate);
        NeuralNetModule.globalAverageFiringThreshold.setValue(globalAvgFiringThreshold);
        NeuralNetModule.globalAverageInputWeight.setValue(globalAvgInputWeight);

        // round for chart
        globalFiringRate = (globalFiringRate * 1000) / 1000;
        globalAvgFiringThreshold = (globalAvgFiringThreshold * 1000) / 1000;
        globalAvgInputWeight = (globalAvgInputWeight * 1000) / 1000;

        // add new point, rotate out the first
        NeuralNetModule.globalFiringRateData.shift();
        NeuralNetModule.globalFiringRateData.push({ x: iteration, y: globalFiringRate });
        NeuralNetModule.globalAvgFiringThresholdData.shift();
        NeuralNetModule.globalAvgFiringThresholdData.push({ x: iteration, y: globalAvgFiringThreshold });
        NeuralNetModule.globalAvgInputWeightData.shift();
        NeuralNetModule.globalAvgInputWeightData.push({ x: iteration, y: globalAvgInputWeight });

        // set on chart
        NeuralNetModule.aggAnalysisChart.series[0].setData(NeuralNetModule.globalFiringRateData, false, false, false);
        NeuralNetModule.aggAnalysisChart.series[1].setData(NeuralNetModule.globalAvgFiringThresholdData, false, false, false);
        NeuralNetModule.aggAnalysisChart.series[2].setData(NeuralNetModule.globalAvgInputWeightData, false, false, false);

        // redraw chart
        NeuralNetModule.aggAnalysisChart.redraw();
    }
};

NeuralNetModule.refreshModuleConfigCallback = function(configJson){
    NeuralNetModule.refreshModuleConfigInProgress = true;
	NeuralNetModule.rowsField.setValue(configJson.rows);
	NeuralNetModule.columnsField.setValue(configJson.columns);
	NeuralNetModule.connectivityField.setValue(configJson.connectivity);
	NeuralNetModule.externalInputRowCountField.setValue(configJson.externalInputRowCount);
	NeuralNetModule.feedbackInputRowCountField.setValue(configJson.feedbackInputRowCount);
	NeuralNetModule.externalOutputRowCountField.setValue(configJson.externalOutputRowCount);
	NeuralNetModule.autoFeedForwardField.setValue(configJson.autoFeedForward);
	NeuralNetModule.autoNewInputField.setValue(configJson.autoNewInput);
	NeuralNetModule.fadeStaleSitesField.setValue(configJson.fadeStaleSites);
	NeuralNetModule.targetFiringRateField.setValue(configJson.targetFiringRate);
	NeuralNetModule.initialNeuronFiringThresholdField.setValue(configJson.initialNeuronFiringThreshold);
	NeuralNetModule.initialExternalInputSynapseWeightField.setValue(configJson.initialExternalInputSynapseWeight);
	NeuralNetModule.initialInternalSynapseWeightField.setValue(configJson.initialInternalSynapseWeight);
	NeuralNetModule.firingRateSampleIterationsField.setValue(configJson.firingRateSampleIterations);
	NeuralNetModule.firingRateThresholdAdjustmentDeltaField.setValue(configJson.firingRateThresholdAdjustmentDelta);
	NeuralNetModule.synapseWeightStrengthenDeltaField.setValue(configJson.synapseWeightStrengthenDelta);
	NeuralNetModule.synapseWeightWeakenDeltaField.setValue(configJson.synapseWeightWeakenDelta);
	NeuralNetModule.minSynapseWeightField.setValue(configJson.minSynapseWeight);
	NeuralNetModule.maxSynapseWeightField.setValue(configJson.maxSynapseWeight);
	NeuralNetModule.activeExternalInputSitePatternIdField.setValue(configJson.activeExternalInputSitePatternId);
	NeuralNetModule.refreshModuleConfigInProgress = false;
};

NeuralNetModule.refreshRenderWindowStateCallback = function (stateJson) {

    NeuralNetModule.refreshRenderWindowStateInProgress = true;
    if (stateJson.autoIterate){
        NeuralNetModule.pauseButton.setText(stateJson.paused ? "Unpause" : "Pause");
        NeuralNetModule.pauseButton.setDisabled(false);
    }
    else {
        NeuralNetModule.pauseButton.setText("Pause");
        NeuralNetModule.pauseButton.setDisabled(true);
    }
    NeuralNetModule.stepButton.setDisabled(stateJson.autoIterate);
    NeuralNetModule.autoIterateField.setValue(stateJson.autoIterate);
    NeuralNetModule.gridLinesField.setValue(stateJson.gridLines);
    NeuralNetModule.siteConnectionsField.setValue(stateJson.siteConnections);
    NeuralNetModule.refreshRenderWindowStateInProgress = false;
};

NeuralNetModule.updateModuleConfigField = function(field, newValue){
    if (!NeuralNetModule.refreshModuleConfigInProgress && field.isValid())
		CaWorkbench.setModuleConfigValue(field.getItemId(), newValue);
};

NeuralNetModule.sendRenderWindowCommand = function (command) {
    if(!NeuralNetModule.refreshRenderWindowStateInProgress)
        CaWorkbench.sendRenderWindowCommand(command, "NeuralNetModule.refreshRenderWindowStateCallback");
};

NeuralNetModule.init = function()
{
    /////////////////////////////////// module configuration /////////////////////////////////////////////
	NeuralNetModule.rowsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Rows",
		labelWidth: 200
	});
	
	NeuralNetModule.columnsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Columns",
		labelWidth: 200
	});
	
	NeuralNetModule.connectivityField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Connectivity",
		itemId: "connectivity",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 200,
        minValue: 0,
        maxValue: 20,
		allowDecimals: false,
		listeners: {change: NeuralNetModule.updateModuleConfigField}
	});

	NeuralNetModule.externalInputRowCountField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "External Input Row Count",
		itemId: "externalInputRowCount",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 200,
        minValue: 1,
        maxValue: 1000,
		allowDecimals: false,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});
	
	NeuralNetModule.feedbackInputRowCountField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Feedback Input Row Count",
		itemId: "feedbackInputRowCount",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 200,
        minValue: 0,
        maxValue: 1000,
		allowDecimals: false,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.externalOutputRowCountField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "External Output Row Count",
		itemId: "externalOutputRowCount",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 200,
        minValue: 0,
        maxValue: 1000,
		allowDecimals: false,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.autoFeedForwardField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto Feed Forward",
		itemId: "autoFeedForward",
		labelWidth: 200,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.autoNewInputField = Ext.create("Ext.form.field.Checkbox", {
		fieldLabel: "Auto New Input",
		itemId: "autoNewInput",
		labelWidth: 200,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.fadeStaleSitesField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Fade Stale Sites",
	    itemId: "fadeStaleSites",
	    labelWidth: 200,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.activeExternalInputSitePatternIdField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Active Ext. Input Site Pattern ID",
	    itemId: "activeExternalInputSitePatternId",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 200,
	    minValue: 0,
	    maxValue: 3,
	    step: 1,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.initialNeuronFiringThresholdField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Initial Neuron Firing Threshold",
		itemId: "initialNeuronFiringThreshold",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -1000,
		maxValue: 1000,
		step: 1,
		decimalPrecision: 3,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.targetFiringRateField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Target Firing Rate",
	    itemId: "targetFiringRate",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: 0,
	    maxValue: 1,
	    step: 0.1,
	    decimalPrecision: 3,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.firingRateSampleIterationsField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Firing Rate Sample Iterations",
	    itemId: "firingRateSampleIterations",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: 0,
	    maxValue: 1000,
	    allowDecimals: false,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.firingRateThresholdAdjustmentDeltaField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Firing Rate Threshold Adjustment Delta",
	    itemId: "firingRateThresholdAdjustmentDelta",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: 0,
	    maxValue: 1000,
	    step: 1,
	    decimalPrecision: 3,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.initialExternalInputSynapseWeightField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Initial Ext. Input Synapse Weight",
	    itemId: "initialExternalInputSynapseWeight",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: -5000,
	    maxValue: 5000,
	    step: 0.1,
	    decimalPrecision: 3,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.initialInternalSynapseWeightField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Initial Internal Synapse Weight",
	    itemId: "initialInternalSynapseWeight",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: -5000,
	    maxValue: 5000,
	    step: 0.1,
	    decimalPrecision: 3,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.synapseWeightStrengthenDeltaField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Synapse Weight Strengthen Delta",
		itemId: "synapseWeightStrengthenDelta",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -1000,
		maxValue: 1000,
		step: 0.1,
		decimalPrecision: 3,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.synapseWeightWeakenDeltaField = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "Synapse Weight Weaken Delta",
	    itemId: "synapseWeightWeakenDelta",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 250,
	    minValue: -1000,
	    maxValue: 1000,
	    step: 0.1,
	    decimalPrecision: 3,
	    listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.minSynapseWeightField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Minimum Synapse Weight",
		itemId: "minSynapseWeight",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -10000,
		maxValue: 10000,
		step: 1,
		decimalPrecision: 3,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.maxSynapseWeightField = Ext.create("Ext.form.field.Number", {
		fieldLabel: "Maximum Synapse Weight",
		itemId: "maxSynapseWeight",
		allowBlank: false,
		repeatTriggerClick: false,
		labelWidth: 250,
		minValue: -10000,
		maxValue: 10000,
		step: 1,
		decimalPrecision: 3,
		listeners: { change: NeuralNetModule.updateModuleConfigField }
	});

	NeuralNetModule.configurationFieldSet = Ext.create("Ext.form.FieldSet", {
		title: "Configuration",
		width: 825,
		layout: { type: "table", columns: 3, tdAttrs: { style: { verticalAlign: "top" } } },
		items: [
			NeuralNetModule.rowsField,
            {xtype: "component", width: 20},
            NeuralNetModule.initialNeuronFiringThresholdField,
			NeuralNetModule.columnsField,
            {xtype: "component", width: 20},
            NeuralNetModule.targetFiringRateField,
			NeuralNetModule.connectivityField,
            {xtype: "component", width: 20},
            NeuralNetModule.firingRateSampleIterationsField,
			NeuralNetModule.externalInputRowCountField,
            {xtype: "component", width: 20},
            NeuralNetModule.firingRateThresholdAdjustmentDeltaField,
			NeuralNetModule.feedbackInputRowCountField,
            {xtype: "component", width: 20},
            NeuralNetModule.initialExternalInputSynapseWeightField,
			NeuralNetModule.externalOutputRowCountField,
            {xtype: "component", width: 20},
            NeuralNetModule.initialInternalSynapseWeightField,
			NeuralNetModule.autoFeedForwardField,
            {xtype: "component", width: 20},
            NeuralNetModule.synapseWeightStrengthenDeltaField,
			NeuralNetModule.autoNewInputField,
            {xtype: "component", width: 20},
            NeuralNetModule.synapseWeightWeakenDeltaField,
            NeuralNetModule.fadeStaleSitesField,
            {xtype: "component", width: 20},
            NeuralNetModule.minSynapseWeightField,
            NeuralNetModule.activeExternalInputSitePatternIdField,
            {xtype: "component", width: 20},
            NeuralNetModule.maxSynapseWeightField
		]
	});
    	
    ///////////////////////////////////// render window controls /////////////////////////////////////////////
	NeuralNetModule.pauseButton = Ext.create("Ext.Button", {
	    text: "Pause",
	    width: 90,
	    margin: "0 10 15 10",
	    handler: function () {
	        NeuralNetModule.sendRenderWindowCommand("togglePaused");
	    }
	});

	NeuralNetModule.stepButton = Ext.create("Ext.Button", {
	    text: "Step",
	    width: 90,
	    margin: "0 10 15 0",
	    handler: function () {
	        NeuralNetModule.sendRenderWindowCommand("iterateOneStep");
	    }
	});

	NeuralNetModule.screenShotButton = Ext.create("Ext.Button", {
	    text: "Screen Shot",
	    width: 90,
	    margin: "0 10 15 0",
	    handler: function () {
	        NeuralNetModule.sendRenderWindowCommand("screenShot");
	    }
	});

	NeuralNetModule.autoIterateField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Auto Iterate",
	    labelWidth: 120,
	    itemId: "autoIterate",
	    listeners: { change: function () { NeuralNetModule.sendRenderWindowCommand("toggleAutoIterate"); } }
	});

	NeuralNetModule.gridLinesField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Grid Lines",
	    labelWidth: 120,
	    itemId: "gridLines",
	    listeners: { change: function () { NeuralNetModule.sendRenderWindowCommand("toggleGridLines"); } }
	});

    NeuralNetModule.siteConnectionsField = Ext.create("Ext.form.field.Checkbox", {
        fieldLabel: "Site Connections",
        labelWidth: 120,
        itemId: "siteConnections",
        listeners: { change: function () { NeuralNetModule.sendRenderWindowCommand("toggleSiteConnections"); } }
    });

    NeuralNetModule.renderWindowControlButtonsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
        layout: { type: "hbox" },
        items: [
			NeuralNetModule.pauseButton,
            NeuralNetModule.stepButton,
            NeuralNetModule.screenShotButton
        ]
    });

    NeuralNetModule.renderWindowControlsFieldSet = Ext.create("Ext.form.FieldSet", {
	    title: "Render Window Controls",
	    height: 150,
        layout: {type: "vbox"},
        items: [
            NeuralNetModule.renderWindowControlButtonsContainer,
            NeuralNetModule.autoIterateField,
            NeuralNetModule.gridLinesField,
            NeuralNetModule.siteConnectionsField
	    ]
	});

    ///////////////////////////////////// module controls /////////////////////////////////////////////
    NeuralNetModule.reloadButton = Ext.create("Ext.Button", {
        text: "Reload Page",
        width: 90,
        handler: function () {
            document.location.reload();
        }
    });

    NeuralNetModule.resetButton = Ext.create("Ext.Button", {
        text: "Reset",
        width: 90,
        margin: "0 10 15 10",
        handler: function () {
            clearInterval(NeuralNetModule.refreshInterval);
            NeuralNetModule.resetInitialChartData();
            CaWorkbench.setModuleConfigValue("reset", null);
            NeuralNetModule.refreshInterval = setInterval(function () {
				CaWorkbench.refreshModuleState("NeuralNetModule.refreshModuleStateCallback");
			}, 500);
        }
    });

    NeuralNetModule.moduleControlButtonsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
        layout: { type: "hbox" },
        items: [
			NeuralNetModule.resetButton,
            NeuralNetModule.reloadButton
        ]
    });

    NeuralNetModule.moduleControlsFieldSet = Ext.create("Ext.form.FieldSet", {
        title: "Module Controls",
        height: 100,
        layout: { type: "vbox" },
        items: [
            NeuralNetModule.moduleControlButtonsContainer
        ]
    });

    ////////////////////////////////////// module state /////////////////////////////////////////////
    NeuralNetModule.iterationField = Ext.create("Ext.form.field.Display", {
        fieldLabel: "Iteration",
        labelWidth: 200
    });

    NeuralNetModule.renderCompleteField = Ext.create("Ext.form.field.Display", {
        fieldLabel: "Render Complete",
        labelWidth: 200
    });

    NeuralNetModule.globalFiringRateField = Ext.create("Ext.form.field.Display", {
        fieldLabel: "Global Firing Rate",
        labelWidth: 200
    });

    NeuralNetModule.globalAverageFiringThreshold = Ext.create("Ext.form.field.Display", {
        fieldLabel: "Global Average Firing Threshold",
        labelWidth: 200
    });

    NeuralNetModule.globalAverageInputWeight = Ext.create("Ext.form.field.Display", {
        fieldLabel: "Global Average Input Weight",
        labelWidth: 200
    });

    NeuralNetModule.stateFieldSet = Ext.create("Ext.form.FieldSet", {
        title: "State",
        width: 350,
        items: [
			NeuralNetModule.iterationField,
			NeuralNetModule.renderCompleteField,
			NeuralNetModule.globalFiringRateField,
			NeuralNetModule.globalAverageFiringThreshold,
			NeuralNetModule.globalAverageInputWeight
        ]
    });

    ////////////////////////////////////////// layout /////////////////////////////////////////////
	NeuralNetModule.moduleMiddlePanelsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
	    layout: {type: "vbox", align: "stretch"},
	    width: 325,
	    items: [
            NeuralNetModule.renderWindowControlsFieldSet,
            NeuralNetModule.moduleControlsFieldSet
        ]
	});    

	NeuralNetModule.modulePanel = Ext.create("Sms.form.Panel", {
		region: "center",
		title: "Module",
		layout: {type: "table", columns: 6, tdAttrs: {style: {verticalAlign: "top"}}},
		items:  [
			{xtype: "component", width: 20},
			NeuralNetModule.configurationFieldSet,
			{xtype: "component", width: 20},
			NeuralNetModule.moduleMiddlePanelsContainer,
            { xtype: "component", width: 20 },
            NeuralNetModule.stateFieldSet
		]
	});

	NeuralNetModule.chartsPanel = Ext.create("Sms.form.Panel", {
	    region: "south",
	    title: "Analysis",
        height: 655,
	    html: "<div id='chart_container' style='height: 610px;'></div>"
	});
	
	var neuralNetModuleTab = Ext.create("Sms.form.Panel", {
		layout: { type: "table", columns: 1, tdAttrs: { style: { verticalAlign: "top", padding: "0 10 0 0"} } },
		//bodyStyle: {"background-color": "lightgreen"},
		title: "Neural Network Module",
		height: 1000,
		items: [NeuralNetModule.modulePanel, NeuralNetModule.chartsPanel],
		listeners: {
			activate: NeuralNetModule.tabActivated,
			deactivate: NeuralNetModule.tabDeactivated
		}
    });
	
	return neuralNetModuleTab;
};

NeuralNetModule.resetInitialChartData = function () {
    var pointsToDisplay = 100;
    NeuralNetModule.globalFiringRateData = [];
    NeuralNetModule.globalAvgFiringThresholdData = [];
    NeuralNetModule.globalAvgInputWeightData = [];

    for (var i = -pointsToDisplay; i < 0; i++) {
        NeuralNetModule.globalFiringRateData.push({ x: i, y: null });
        NeuralNetModule.globalAvgFiringThresholdData.push({ x: i, y: null });
        NeuralNetModule.globalAvgInputWeightData.push({ x: i, y: null });
    }

    NeuralNetModule.aggAnalysisChart.series[0].setData(NeuralNetModule.globalFiringRateData, false, false, false);
    NeuralNetModule.aggAnalysisChart.series[1].setData(NeuralNetModule.globalAvgFiringThresholdData, false, false, false);
    NeuralNetModule.aggAnalysisChart.series[1].setData(NeuralNetModule.globalAvgInputWeightData, false, false, false);
    NeuralNetModule.aggAnalysisChart.redraw();
};

NeuralNetModule.initChart = function () {
    NeuralNetModule.aggAnalysisChart = new Highcharts.Chart({
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

    NeuralNetModule.resetInitialChartData();
};

NeuralNetModule.tabActivated = function(){
	NeuralNetModule.initChart();
	NeuralNetModule.refreshInterval = setInterval(function () {
		CaWorkbench.refreshModuleState("NeuralNetModule.refreshModuleStateCallback");
	}, 500);
	
	setTimeout(function(){
		CaWorkbench.refreshModuleConfig("NeuralNetModule.refreshModuleConfigCallback");
		CaWorkbench.refreshRenderWindowState("NeuralNetModule.refreshRenderWindowStateCallback");
	}, 3000);
};

NeuralNetModule.tabDeactivated = function(){
	clearInterval(NeuralNetModule.refreshInterval);
};
