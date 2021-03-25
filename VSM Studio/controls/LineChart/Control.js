require('moment.js');
require('chart.js');

// Used by Chart.JS 2.6.0 but part of ECMA 6
Number.MAX_SAFE_INTEGER = 9007199254740991;
Number.MIN_SAFE_INTEGER = -9007199254740991;


function LineChart (root, config)  {
   var xAxisType = root.getAttribute('vfp:xtype');
   var background = root.getElementsByTagName('rect')[0];
   var bounds = root.getElementsByTagName('rect')[1];
   var chartarea = root.getElementsByTagName('rect')[2];
   var obj = this;
            
   // Member variables:
   var chart = null;
   var ctx = null;
   var columns = [];
   var xAxis, xmin, xmax;
   var loadDataPending = false;
   var loadDataPendingRows = [];
      
   this.reconfigure = function(newInstance) {
      if (newInstance) {
         // Get rid of the design graphics:     
         root.removeChild(root.getElementsByTagName("path")[0]);

         // Create an empty columns array if not defined:
         if (config.columns ==  undefined)
            config.columns = {};
         
         // Remove unwanted x-axis groups from the config object:
         if (xAxisType != 'Numeric')
            delete config.xRange; 
         if (xAxisType != 'Time')  
            delete config.timeRange;              
            
         config.backGroundColour = background.style.fill; 
      }
            
      // Set up the correct number of column groups in the config object 
      var rebuild = false;
      var columns = config.columns;
      for (var i=0; i<10; ++i) {   
         var col = columns[i];
         if (i<config.numColumns) {
            if (col == undefined || col.name == undefined) {
               // Create default settings for a new column:
               col = {};
               col.name = "Column "+i;
               col.axis = "0";
               col.colour = "#000000";
               col.min = "0";
               col.max = "0";
               columns[i] = col;
               rebuild = true;
            }
         } else if (columns[i] != undefined) {
           // Remove the column data
           delete columns[i];
           rebuild = true;
         }            
      }
      
      // If the number of columns is changed, we need to do a full rebuild, otherwise we can just update the options:
      if (rebuild) {
         setControlConfig(root, config);
        rebuildChart();
      } else {
        updateChart();
      }
         
      
      // Update the background colour:
      background.style.fill = config.backGroundColour;
      
         
   };

   this.update = function () {
      // Fix the border width:
      var ctm = root.getCTM();
      var tbt = panel.createSVGTransform();
      tbt.setScale(1/ctm.a, 1/ctm.d);
      background.transform.baseVal.initialize(tbt);
      background.width.baseVal.value = bounds.  width.baseVal.value*ctm.a;
      background.height.baseVal.value = bounds.height.baseVal.value*ctm.d;      
      
      // Only needed for Chart.JS 2.2.2
      // chart.resize();
   };
   

   // Set up a numeric  x axis with the specified range (numeric data only)
   this.setXRange = function (xmin, xmax) {
      chart.options.scales.xAxes[0].display = true;
      chart.options.scales.xAxes[1].display = false;
      xAxis = chart.options.scales.xAxes[0];
      xAxis.ticks.min = xmin;
      xAxis.ticks.max = xmax;
      updateRange()
   };
   
   this.setTimeRange = function (unit, range, absolute) {
      chart.options.scales.xAxes[0].display = false;
      chart.options.scales.xAxes[1].display = true;
      xAxis = chart.options.scales.xAxes[1];
      
      // Choose a time unit and sensible step size            
      var timeUnits = [ 
       { name: 'second', secs: 1,      steps: [ 2, 5, 10, 30, 60, 120, 300, 600, 3600  ] }, 
       { name: 'minute', secs: 60,     steps: [ 2, 5, 10, 30, 60, 120, 300, 480, 1200 ] },
       { name: 'hour',   secs: 3600,   steps: [ 2, 4, 12, 24, 48,  120, 168, 336 ]  },
       { name: 'day',    secs: 86400,  steps: [ 2, 7, 14, 28, 35,  70,  140, 350, 700, 3500 ] },
       { name: 'week',   secs: 604800, steps: [ 2, 4,  8, 12, 26,  52,  104, 208, 408, 804 ] }
      ];
      xAxis.time.unit = timeUnits[unit].name;
      xAxis.time.unitStepSize = 1;
      for (var i = 0; i<timeUnits[unit].steps.length; i++)
         if (range / timeUnits[unit].steps[i] >= 4)
            xAxis.time.unitStepSize = timeUnits[unit].steps[i];     

      // this is what it's called in 2.6.0
      xAxis.time.stepSize =  xAxis.time.unitStepSize; 
      
      // Choose a display format based on abs/rel.
      if (absolute)               
          xAxis.time.displayFormats  = { second: 'HH:mm:ss', minute: 'HH:mm', hour: 'HH:mm', day: 'MMM DD', week: 'MMM YY' };
      else    
          xAxis.time.displayFormats  = { second: 'mm:ss', minute: 'HH:mm',    hour: 'HH:mm', day: 'DDD HH:mm', week: 'DDD' };
      xAxis.time.tooltipFormat = xAxis.time.displayFormats[xAxis.time.unit];        
      
      // These values are preserved/used by updateRange, not by chart.js.
      xAxis.time.absolute = absolute;  
      //xAxis.time.step =  xAxis.time.unitStepSize*timeUnits[unit].secs*1000; 
      xAxis.time.range = range*timeUnits[unit].secs*1000;      
      
      updateRange();
   };
            
   // Define a column  
   this.setColumn = function (column, _name, _axis, _colour) {
      var col = { name: _name, axis: _axis, colour: _colour, display: true  };
      col.dataset = {
          label: col.name,
          fill : false,
          borderColor : col.colour,
          lineTension : 0,
          xAxisID : xAxis.id,
          yAxisID : col.axis,
          data : []
      };  
      columns[column] = col;      
   };

   // Show or hide a column
   this.showColumn = function (column, _display) {
      columns[column].display = _display;
      if (_display)
         chart.data.datasets[column] = columns[column].dataset;
      else
         chart.data.datasets[column] = null;   
      updateScales();   
   };
 
   // Set suggest y range for an axis. The autoscale will override this if the data exceeds the specified range.  
   this.setYRange = function (axis, ymin, ymax) {
      if (ymin < ymax) { 
         chart.options.scales.yAxes[axis].ticks.suggestedMin = ymin;
         chart.options.scales.yAxes[axis].ticks.suggestedMax = ymax;
      } else {
         delete chart.options.scales.yAxes[axis].ticks.suggestedMin;
         delete chart.options.scales.yAxes[axis].ticks.suggestedMax;
      } 
      updateRange()      
   };

   
   // Load data from a file into the graph:
   // This will trigger an ajax request to fetch the data so operates asynchronously.
   // Note that because this is asynchronous it can result in the file data arriving after any recorded session data and this, in turn
   // can result in incorrect computation of the value of xmin for time relative mode. Therefore, any appendData() calls that arrive
   // whilst we are awaiting the file data are queued and then processed after the file data has arrived.
   this.setData = function (file) {                  
      // Start data queuing:
      loadDataPending = true;
      loadDataPendingRows = [];
      
      // Request the data file and load it into the graph:
      requestFile(file, function (text) {
         // Process log file:
         obj.clearData();
         if (text != null) {
            var lines = text.split('\n'); 
            for (var i in lines) 
                addDataRow(lines[i]);
         }

         // Process any queued data:
         for (var i in loadDataPendingRows)
            addDataRow(loadDataPendingRows[i]);
         loadDataPending = false;
         
         // Update the chart to display the new data:
         updateRange();   
      
      });

   };
   
   // Add new row of data and update the display
   this.appendData = function (line) {
      if (xAxis != null) {
         if (loadDataPending)
            loadDataPendingRows.push(line)
         else {
            addDataRow(line);
            updateRange();   
         }
      }
   }
   
   // Clear all the data stored in the graph
   this.clearData = function () {
      for (var i in columns)
         columns[i].dataset.data = [];
      xmin = null;
      xmax = null;
      updateRange();   
   }
   

   // Private: update/rebuild the chart from the config options.
   function rebuildChart () {
      // Destroy any existing chart:      
      if (chart != null) {
         chart.destroy();         
         columns = [];
      }
      div.innerHTML = '';         

      // Disable mouse interaction with the overlay:
      ctx = document.createElement('canvas');
      div.appendChild(ctx);
      div.style.pointerEvents='none';
      ctx.style.pointerEvents='none';
      ctx.style.width='100%';
      ctx.style.height='100%';
      ctx.style.background = 'none';
      ctx.style.borderStyle = "none";
   
      
      chart = new Chart(ctx, {
         type: 'line',
         data : { datasets: [] },
         options: {
            responsive: true,
            maintainAspectRatio: false,
             scales: {
                  xAxes: [{
                     id : 'x',
                     type: 'linear', 
                     display : false,
                     position: 'bottom',
                     ticks: {  fontColor: 'black' }
                  },{ id : 't',
                     type: 'time',
                     time: {  unit: 'minute' },
                     display : false,
                     position: 'bottom',
                     ticks: {  fontColor: 'black' }
                  }],
                  yAxes: [{
                      id: '0',
                      type: 'linear',
                      position: 'left',
                      ticks: {  fontColor: 'black' }
                  },{
                      id: '1',
                      type: 'linear',
                      position: 'right',
                      ticks: {  fontColor: 'black' }
                  }]
            }                                     
        }
      });
      
      // Store this in the root object so that we can retrieve it on reconstruction/rebinding.
      root.chart = chart;
      
      updateChart();            
   }   
      
      
   function updateChart() {
      // Set up the title
      chart.options.title.text = config.titleText;
      chart.options.title.display = config.titleText != undefined && config.titleText != '';
      chart.options.title.fontSize = parseInt(config.titleFontSize);
      
      // Set font sizes
      chart.options.legend.labels.fontSize = parseInt(config.labelFontSize);
      chart.options.legend.display = parseInt(config.showLegend);
      chart.options.scales.xAxes[0].ticks.fontSize = parseInt(config.scaleFontSize);
      chart.options.scales.xAxes[1].ticks.fontSize = parseInt(config.scaleFontSize);
      chart.options.scales.yAxes[0].ticks.fontSize = parseInt(config.scaleFontSize);
      chart.options.scales.yAxes[1].ticks.fontSize = parseInt(config.scaleFontSize);


      // Set the range for the x-asis
      switch (xAxisType) {
         case 'Numeric' : obj.setXRange(parseFloat(config.xRange.min), parseFloat(config.xRange.max)); break;            
         case 'Time'    : obj.setTimeRange(parseInt(config.timeRange.unit), parseInt(config.timeRange.range), parseInt(config.timeRange.absolute)); break;           
      }   
     
      // Rebuild the column settings:
      columns = [];
      for (var i=0; i<config.numColumns; ++i) { 
         if (i in config.columns) {
            var col = config.columns[i];
            obj.setColumn(i, col.name, col.axis, col.colour);      
         }
      } 
      
      // Update the Y axis settings. To do this, take the lowest/highest ymin/ymax from any column with valid settings:
      for (var axis = 0; axis < 2; ++axis) {
         var ymin=Number.MAX_VALUE, ymax=-Number.MAX_VALUE;
         for (var i=0; i<config.numColumns; ++i) { 
            if (i in config.columns) {
               var col = config.columns[i], cmin=parseFloat(col.min), cmax=parseFloat(col.max);
               if (col.axis == axis && cmin != cmax) {
                  if (cmin < ymin) ymin = cmin;
                  if (cmax > ymax) ymax = cmax;                                       
               }                              
            }
         }
         obj.setYRange(axis, ymin, ymax);
      }                    
      
  
      
      // Update the axis labels etc:
      updateScales();
      updateRange();
   }
           
   // Private: update the displayed datasets:
   function updateScales() {    
      // Choose colours for the axes - if there is only one data column for an
      // axis we can use that columns dataset colour, otherwise we use grey:
      var axes = chart.options.scales.yAxes;
      var leftColour, rightColour;
      var datasets = [];
      var i, j = 0;
      for (i in columns) {
          if (columns[i].display) { 
             if (columns[i].axis == 0) {
                if (leftColour == null)
                   leftColour = columns[i].colour;
                else
                   leftColour = 'black';
             } else {
                if (rightColour == null)
                   rightColour = columns[i].colour;
                else
                   rightColour = 'black';
             }
            
             if (columns[i].dataset != null) {
                datasets[j++] = columns[i].dataset;            
                columns[i].dataset.pointRadius = parseInt(config.pointRadius);
                columns[i].dataset.borderWidth = parseInt(config.lineWidth);
             }
          }
      }
      
      // Display only the axes that are in use:
      axes[0].display = leftColour != null;
      axes[1].display = rightColour != null;
      axes[0].ticks.fontColor = leftColour;
      axes[1].ticks.fontColor = rightColour;
                      
      // Display only the visible datasets:
      chart.data.datasets = datasets;
      chart.update();           
   }
   
   // Private: update the ranges on the x-axis.
   // The data is re-sorted in case it has arrived in the wrong order.
   function updateRange () {
      for (var i in columns) 
          if (columns[i].dataset != null)          
             columns[i].dataset.data.sort(function(a,b) { return a.x>b.x?1:a.x<b.x?-1:0; });             
      if (xAxis.id == 't') {
         var chartRange = xAxis.time.range;
         var dataRange  = moment(xmax)-moment(xmin);
         var tmin, tmax;
         if (xAxis.time.absolute) {
            tmax = moment(xmax);
            tmin = moment(xmin);
         } else {
            tmax = moment(moment(xmax)-moment(xmin));
            tmin = moment(0);
         }
            
         if (tmin % xAxis.time.step != 0)
            tmin = tmin.subtract(tmin % xAxis.time.step);                  
         if (tmax % xAxis.time.step != 0)
            tmax = tmax.add(xAxis.time.step - tmax % xAxis.time.step);                  
         if (dataRange > chartRange) {
            xAxis.time.max = tmax;
            xAxis.time.min = moment(tmax).subtract(chartRange);
         } else {
            xAxis.time.min = tmin;
            xAxis.time.max = moment(tmin).add(chartRange);
         }
      }
      chart.update();           
   }
   
   // Private: add a row of data and update xmin/max
   function addDataRow (line) {
      var row = CSVtoArray (line);
      if (row.length != 0) {
         if (xmin == null || row[0] < xmin) xmin = row[0];
         if (xmax == null || row[0] > xmax) xmax = row[0];
         for (var j=0; j<columns.length; j++) {
            if (xAxis.id == 'x')
               columns[j].dataset.data.push ({ x: parseFloat(row[0]), y: row[j+1]  });                                         
            else if (xAxis.time.absolute) 
               columns[j].dataset.data.push ({ x: moment(row[0]), y: row[j+1]  });                          
            else  // time, relative
               columns[j].dataset.data.push ({ x: moment(row[0])-moment(xmin), y: row[j+1] } );
         }
      }
   }

   // Construct the chart. If we are binding to a root object that already has an overlay then we can bypass the chart
   // recreation. This avoids fully reconstructing the chart after property edits.
   var div = createOverlay(root, chartarea, 'div');   
   if (root.chart == undefined)
      rebuildChart();
   else {
      chart = root.chart;
      updateChart();   
   }

   // Test data:
   //this.appendData("2017-07-16 12:23:40,1")
   //this.appendData("2017-07-16 12:25:40,2")   
   
   return this;
}


