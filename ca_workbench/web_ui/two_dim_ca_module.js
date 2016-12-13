Ext.ns("TwoDimCaModule");

TwoDimCaModule.init = function()
{
    /////////////////////////////////// module configuration /////////////////////////////////////////////
	TwoDimCaModule.rowsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Rows",
		labelWidth: 200
	});
	
	TwoDimCaModule.columnsField = Ext.create("Ext.form.field.Display", {
		fieldLabel: "Columns",
		labelWidth: 200
	});
	
	TwoDimCaModule.stateCount = Ext.create("Ext.form.field.Number", {
	    fieldLabel: "State Count",
		itemId: "stateCount",
	    allowBlank: false,
	    repeatTriggerClick: false,
	    labelWidth: 200,
	    minValue: 1,
	    maxValue: 255,
	    step: 1,
		width: 275,
	    listeners: { change: TwoDimCaModule.updateModuleConfigField }
	});

	// neighborhood select box here
	
	TwoDimCaModule.stateSpecifierString = Ext.create("Ext.form.field.Text", {
		labelWidth: 200,
		fieldLabel: "State Specifier String",
		itemId: "stateSpecifierString",
		width: 500,
		maxLength: 50,
		listeners: { change: TwoDimCaModule.updateModuleConfigField }
	});

	TwoDimCaModule.configurationFieldSet = Ext.create("Ext.form.FieldSet", {
		title: "Configuration",
		width: 825,
		layout: { type: "table", columns: 1, tdAttrs: { style: { verticalAlign: "top" } } },
		items: [
			TwoDimCaModule.rowsField,
			TwoDimCaModule.columnsField,
			TwoDimCaModule.stateCount,
			TwoDimCaModule.stateSpecifierString
		]
	});
    
	///////////////////////////////////// render window controls /////////////////////////////////////////////
	TwoDimCaModule.pauseButton = Ext.create("Ext.Button", {
	    text: "Pause",
	    width: 90,
	    margin: "0 10 15 10",
	    handler: function () {
	        TwoDimCaModule.sendRenderWindowCommand("togglePaused");
	    }
	});

	TwoDimCaModule.stepButton = Ext.create("Ext.Button", {
	    text: "Step",
	    width: 90,
	    margin: "0 10 15 0",
	    handler: function () {
	        TwoDimCaModule.sendRenderWindowCommand("iterateOneStep");
	    }
	});

	TwoDimCaModule.screenShotButton = Ext.create("Ext.Button", {
	    text: "Screen Shot",
	    width: 90,
	    margin: "0 10 15 0",
	    handler: function () {
	        TwoDimCaModule.sendRenderWindowCommand("screenShot");
	    }
	});

	TwoDimCaModule.autoIterateField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Auto Iterate",
	    labelWidth: 120,
	    itemId: "autoIterate",
		checked: true,
	    listeners: { change: function () { TwoDimCaModule.sendRenderWindowCommand("toggleAutoIterate"); } }
	});

	TwoDimCaModule.gridLinesField = Ext.create("Ext.form.field.Checkbox", {
	    fieldLabel: "Grid Lines",
	    labelWidth: 120,
	    itemId: "gridLines",
	    listeners: { change: function () { TwoDimCaModule.sendRenderWindowCommand("toggleGridLines"); } }
	});

    TwoDimCaModule.siteConnectionsField = Ext.create("Ext.form.field.Checkbox", {
        fieldLabel: "Site Connections",
        labelWidth: 120,
        itemId: "siteConnections",
        listeners: { change: function () { TwoDimCaModule.sendRenderWindowCommand("toggleSiteConnections"); } }
    });

    TwoDimCaModule.renderWindowControlButtonsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
        layout: { type: "hbox" },
        items: [
			TwoDimCaModule.pauseButton,
            TwoDimCaModule.stepButton,
            TwoDimCaModule.screenShotButton
        ]
    });

    TwoDimCaModule.renderWindowControlsFieldSet = Ext.create("Ext.form.FieldSet", {
	    title: "Render Window Controls",
	    height: 150,
        layout: {type: "vbox"},
        items: [
            TwoDimCaModule.renderWindowControlButtonsContainer,
            TwoDimCaModule.autoIterateField,
            TwoDimCaModule.gridLinesField,
            TwoDimCaModule.siteConnectionsField
	    ]
	});

    ///////////////////////////////////// module controls /////////////////////////////////////////////
    TwoDimCaModule.reloadButton = Ext.create("Ext.Button", {
        text: "Reload Page",
        width: 90,
        handler: function () {
            document.location.reload();
        }
    });

    TwoDimCaModule.resetButton = Ext.create("Ext.Button", {
        text: "Reset",
        width: 90,
        margin: "0 10 15 10",
        handler: function () {
            clearInterval(TwoDimCaModule.refreshInterval);
            //TwoDimCaModule.resetInitialChartData();
            CaWorkbench.setModuleConfigValue("reset", null);
            TwoDimCaModule.refreshInterval = setInterval(function () { CaWorkbench.refreshModuleState(
				"TwoDimCaModule.refreshModuleStateCallback");
			}, 500);
        }
    });

    TwoDimCaModule.moduleControlButtonsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
        layout: { type: "hbox" },
        items: [
			TwoDimCaModule.resetButton,
            TwoDimCaModule.reloadButton
        ]
    });

    TwoDimCaModule.moduleControlsFieldSet = Ext.create("Ext.form.FieldSet", {
        title: "Module Controls",
        height: 100,
        layout: { type: "vbox" },
        items: [
            TwoDimCaModule.moduleControlButtonsContainer
        ]
    });

    ////////////////////////////////////// module state /////////////////////////////////////////////
    TwoDimCaModule.iterationField = Ext.create("Ext.form.field.Display", {
        fieldLabel: "Iteration",
        labelWidth: 200
    });

    TwoDimCaModule.renderCompleteField = Ext.create("Ext.form.field.Display", {
        fieldLabel: "Render Complete",
        labelWidth: 200
    });

    TwoDimCaModule.stateFieldSet = Ext.create("Ext.form.FieldSet", {
        title: "State",
        width: 350,
        items: [
			TwoDimCaModule.iterationField,
			TwoDimCaModule.renderCompleteField
        ]
    });
	
    ////////////////////////////////////////// layout /////////////////////////////////////////////
	TwoDimCaModule.moduleMiddlePanelsContainer = Ext.create("Ext.panel.Panel", {
        border: false,
	    layout: {type: "vbox", align: "stretch"},
	    width: 325,
	    items: [
            TwoDimCaModule.renderWindowControlsFieldSet,
            TwoDimCaModule.moduleControlsFieldSet
        ]
	});    
	
	TwoDimCaModule.modulePanel = Ext.create("Sms.form.Panel", {
		region: "center",
		title: "Module",
		layout: {type: "table", columns: 6, tdAttrs: {style: {verticalAlign: "top"}}},
		items:  [
			{xtype: "component", width: 20},
			TwoDimCaModule.configurationFieldSet,
			{xtype: "component", width: 20},
			TwoDimCaModule.moduleMiddlePanelsContainer,
            { xtype: "component", width: 20 },
            TwoDimCaModule.stateFieldSet
		]
	});

	TwoDimCaModule.chartsPanel = Ext.create("Sms.form.Panel", {
	    region: "south",
	    title: "Analysis",
        height: 655,
	    html: "<div id='TwoDimCaModuleChart' style='height: 610px;'></div>"
	});
	
	var twoDimCaModuleTab = Ext.create("Sms.form.Panel", {
		layout: { type: "table", columns: 1, tdAttrs: { style: { verticalAlign: "top", padding: "0 10 0 0"} } },
		title: "2D CA Module",
		height: 1000,
		items: [TwoDimCaModule.modulePanel, TwoDimCaModule.chartsPanel],
		listeners: {
			activate: TwoDimCaModule.tabActivated,
			deactivate: TwoDimCaModule.tabDeactivated
		}
    });
	

	TwoDimCaModule.refreshModuleConfigInProgress = false;
	TwoDimCaModule.refreshRenderWindowStateInProgress = false;
	
	return twoDimCaModuleTab;
};

TwoDimCaModule.refreshModuleStateCallback = function(stateJson){
	if (TwoDimCaModule.autoIterateField.getValue()) {
		// get state variables
		var iteration = stateJson.iteration;

		// set field values
		TwoDimCaModule.iterationField.setValue(iteration);
		TwoDimCaModule.renderCompleteField.setValue(stateJson.renderComplete);
	}
};

TwoDimCaModule.refreshModuleConfigCallback = function(configJson){
	TwoDimCaModule.rowsField.setValue(configJson.rows);
	TwoDimCaModule.columnsField.setValue(configJson.columns);
	TwoDimCaModule.stateCount.setValue(configJson.state_count);
	TwoDimCaModule.stateSpecifierString.setValue(configJson.state_specifier_string);
};

TwoDimCaModule.refreshRenderWindowStateCallback = function(){
	//console.log("TwoDimCaModule.refreshRenderWindowStateCallback");
};

/*
TwoDimCaModule.resetInitialChartData = function () {
    var pointsToDisplay = 100;
    TwoDimCaModule.globalFiringRateData = [];
    TwoDimCaModule.globalAvgFiringThresholdData = [];
    TwoDimCaModule.globalAvgInputWeightData = [];

    for (var i = -pointsToDisplay; i < 0; i++) {
        TwoDimCaModule.globalFiringRateData.push({ x: i, y: null });
        TwoDimCaModule.globalAvgFiringThresholdData.push({ x: i, y: null });
        TwoDimCaModule.globalAvgInputWeightData.push({ x: i, y: null });
    }

    TwoDimCaModule.aggAnalysisChart.series[0].setData(TwoDimCaModule.globalFiringRateData, false, false, false);
    TwoDimCaModule.aggAnalysisChart.series[1].setData(TwoDimCaModule.globalAvgFiringThresholdData, false, false, false);
    TwoDimCaModule.aggAnalysisChart.series[1].setData(TwoDimCaModule.globalAvgInputWeightData, false, false, false);
    TwoDimCaModule.aggAnalysisChart.redraw();
};

TwoDimCaModule.initChart = function () {
    TwoDimCaModule.aggAnalysisChart = new Highcharts.Chart({
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

    TwoDimCaModule.resetInitialChartData();
};
*/

TwoDimCaModule.updateModuleConfigField = function(field, newValue){
    if (!TwoDimCaModule.refreshModuleConfigInProgress && field.isValid())
		CaWorkbench.setModuleConfigValue(field.getItemId(), newValue);
};

TwoDimCaModule.tabActivated = function(){
	TwoDimCaModule.refreshInterval = setInterval(function () {
		CaWorkbench.refreshModuleState("TwoDimCaModule.refreshModuleStateCallback");
	}, 500);
	
	setTimeout(function(){
		CaWorkbench.refreshModuleConfig("TwoDimCaModule.refreshModuleConfigCallback");
		CaWorkbench.refreshRenderWindowState("TwoDimCaModule.refreshRenderWindowStateCallback");
	}, 3000);
};

TwoDimCaModule.tabDeactivated = function(){
	clearInterval(TwoDimCaModule.refreshInterval);
};

TwoDimCaModule.sendRenderWindowCommand = function(command){
	if(!TwoDimCaModule.refreshRenderWindowStateInProgress)
		CaWorkbench.sendRenderWindowCommand(command, "TwoDimCaModule.refreshRenderWindowStateCallback");
};

