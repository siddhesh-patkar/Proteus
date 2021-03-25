require('moment.js');
require('chart.js');

// Used by Chart.JS 2.6.0 but part of ECMA 6
Number.MAX_SAFE_INTEGER = 9007199254740991;
Number.MIN_SAFE_INTEGER = -9007199254740991;


function Histogram (root, config)  {
   var background = root.getElementsByTagName('rect')[0];
   var bounds = root.getElementsByTagName('rect')[1];
   var chartarea = root.getElementsByTagName('rect')[2];
   var obj = this;

   // Member variables:
   var chart = null;
   var ctx = null;
   var data = [];
   var loadDataPending = false;
   var loadDataPendingRows = [];

   // X axis control:
   var xAxisType = root.getAttribute('vfp:xtype'), xAxis;
   var xmin=null, xmax=null, binCount = 10, binSize = 10;
    
   // Triggered whenever the property panel is used to change the properties.
   this.reconfigure = function(newInstance) {
      if (newInstance) {
         // Get rid of the design graphics:
         root.removeChild(root.getElementsByTagName('g')[0]);

         // Remove unwanted x-axis groups from the config object:
         if (xAxisType != 'Numeric')
            delete config.xRange; 
         if (xAxisType != 'Time')  
            delete config.timeRange;              

         config.backgroundColour = background.style.fill;         
         
      }

      // Update the background colour:
      background.style.fill = config.backgroundColour;

      updateChart();
   };

   // Called on resize
   this.update = function () {
      // Fix the border width:
      var ctm = root.getCTM();
      var tbt = panel.createSVGTransform();
      tbt.setScale(1/ctm.a, 1/ctm.d);
      background.transform.baseVal.initialize(tbt);
      background.width.baseVal.value = bounds.width.baseVal.value*ctm.a;
      background.height.baseVal.value = bounds.height.baseVal.value*ctm.d;
   };


   // Set the labels according to minX/maxX and bar (bin) count
   this.setXRange = function (minX, maxX, bars) {
      chart.options.scales.xAxes[0].display = true;
      chart.options.scales.xAxes[1].display = false;
      xAxis = chart.options.scales.xAxes[0];
      xmin = parseFloat(minX);
      xmax = parseFloat(maxX);
      binCount = bars;
      binSize = (xmax-xmin)/bars;
      xAxis = chart.options.scales.xAxes[0];
      updateRange();
   }
   
   // Set the bin labels according to the time range and bin count
   // The logic here can support an actual time axis but full support for bar charts with time axis is not provided in Chart.JS 2.6.0.
   // We therefore set up the axis.time parameters in readiness for that but then use those values to create time based categories.
   this.setTimeRange = function (unit, range, bars, absolute) {
      chart.options.scales.xAxes[0].display = false;
      chart.options.scales.xAxes[1].display = true;
      xAxis = chart.options.scales.xAxes[1];

      // Choose a time unit and sensible step size. 
      var timeUnits = [ 
       { name: 'second', secs: 1 },
       { name: 'minute', secs: 60 },
       { name: 'hour',   secs: 3600 },
       { name: 'day',    secs: 86400 },
       { name: 'month',  secs: 0.001 }
      ];
      xAxis.time.unit = timeUnits[unit].name;
      
      // The step (bin) size is determined by a heuristic so that the graph shows a sensible number of bars that will encompass the requested range.
      //xAxis.time.stepSize = 1;
      //for (var i = 0; i<timeUnits[unit].steps.length; i++)
      //   if (range / timeUnits[unit].steps[i] >= 4)
      //      xAxis.time.stepSize = timeUnits[unit].steps[i];     
      if (xAxis.time.unit != 'month')
         xAxis.time.stepSize = range/bars;
      else   
         xAxis.time.stepSize = 1;
      xAxis.gridLines.offsetGridLines = (xAxis.time.stepSize == 1);
      
      // Choose a display format based on abs/rel.
      if (absolute)               
          xAxis.time.displayFormats  = { second: 'HH:mm:ss', minute: 'HH:mm', hour: 'HH:mm', day: 'MMM DD', month: 'MMM YY' };
      else    
          xAxis.time.displayFormats  = { second: 'mm:ss', minute: 'HH:mm',    hour: 'HH:mm', day: 'DDD HH:mm', month: 'M' };
      xAxis.time.tooltipFormat = xAxis.time.displayFormats[xAxis.time.unit];        
      
      // These values are preserved/used by updateRange, not by chart.js.
      xAxis.time.absolute = absolute;  
      xAxis.time.step =  xAxis.time.stepSize*timeUnits[unit].secs*1000; 
      xAxis.time.range = moment(0).add(range, xAxis.time.unit).valueOf();
      binCount = range/xAxis.time.stepSize;
      binSize = xAxis.time.step;
      updateRange();
   };
   

   // Set suggested y range for an axis. The autoscale will override
   // this if the data exceeds the specified range.
   this.setYRange = function (minY, maxY) {
      if (minY != undefined) {
         chart.options.scales.yAxes[0].ticks.suggestedMin = minY;
      }
      if (maxY != undefined) {
         chart.options.scales.yAxes[0].ticks.suggestedMax = maxY;
      }
      chart.update();
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
      if (loadDataPending)
         loadDataPendingRows.push(line)
      else {
         addDataRow(line);
         updateRange();   
         updateBins();
      }
   }
   
   // Clear all the data stored in the graph
   this.clearData = function () {
      data = [];
      if (xAxis.type == 't')
         xmin = xmax = null;
      updateRange();   
   }
   
   // Private: update/rebuild the chart from the config options.
   function rebuildChart () {
      // Destroy any existing chart:
      if (chart != null) {
         chart.destroy();
         obj.bins = [];
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
         type: 'bar',
         data: { datasets: [{}] },
         options: {
            legend: { display: false },
            responsive: true,
            maintainAspectRatio: false,
            scales: {
               xAxes: [{
                  id: 'x',
                  barPercentage: 1.0,
                  categoryPercentage: 0.8,
                  type: 'category',
                  gridLines: { offsetGridLines: false,
                               drawOnChartArea: false },                               
               },{
                  id: 't',
                  barPercentage: 1.0,
                  categoryPercentage: 0.8,
                  type: 'category',
                  gridLines: { offsetGridLines: false,
                               drawOnChartArea: false },                               
                  time: {  unit: 'minute' }
               }],
               yAxes: [{
                      id: '0',
                      type: 'linear',
                      position: 'left',
                      ticks: {  fontColor: 'black' }
               }]

            }
         }
      });

      // Store this in the root object so that we can retrieve it on reconstruction/rebinding.
      root.chart = chart;

      updateChart();
   }

     

   // Update the object from config.
   // Triggered after a property edit.
   function updateChart() {
      // Set up the title
      chart.options.title.text = config.titleText;
      chart.options.title.display = config.titleText != undefined && config.titleText != '';
      chart.options.title.fontSize = parseInt(config.titleFontSize);

      // Set font sizes
      chart.options.scales.xAxes[0].ticks.fontSize = parseInt(config.scaleFontSize);
      chart.options.scales.yAxes[0].ticks.fontSize = parseInt(config.scaleFontSize);
      
      // Update the axis settings.
      switch (xAxisType) {
         case 'Numeric' : obj.setXRange(parseFloat(config.xRange.min), parseFloat(config.xRange.max), parseInt(config.xRange.bars)); break;            
         case 'Time'    : obj.setTimeRange(parseInt(config.timeRange.unit), parseInt(config.timeRange.range), parseInt(config.timeRange.bars), parseInt(config.timeRange.absolute)); break;           
      }         
      obj.setYRange(config.yRange.min, config.yRange.max);
      
      if (chart.data.datasets[0].label == undefined) {
         chart.data.datasets[0] = {
            label : "histogram data",
            xAxisID : xAxis.id,
            backgroundColor : config.barColour,
            borderWidth : 2
         }
      }

      // Update the chart data and re-draw.
      chart.update();
   }

   
   // Update the range variables and thus compute the category labels
   // For the time axis, this involves establishing the time range of the data, and then
   // creating labels based on the display format chosen in setTimeRange.
   function updateRange () {   
      chart.data.labels = [];
      switch (xAxis.id) {
         case 'x' : 
            for (var i = 0; i <= binCount; ++i) {
               chart.data.labels[i] = xmin + (i * binSize); 
            }
            break;
            
         case 't' :   
            var chartRange = xAxis.time.range;
            var dataRange  = xmax-xmin;
            var tmin, tmax;
            if (xAxis.time.absolute) {
               tmax = moment(xmax);
               tmin = moment();
            } else {
               tmax = moment(xmax);
               tmin = moment(0);
            }
               
            if (tmin % xAxis.time.step != 0)
               tmin = tmin.subtract(tmin % xAxis.time.step);                  
            if (tmax % xAxis.time.step != 0)
               tmax = tmax.add(xAxis.time.step - tmax % xAxis.time.step);                  
            if (dataRange >= chartRange) {
               // Display latest data only
               xAxis.time.max = tmax;
               xAxis.time.min = moment(tmax).subtract(chartRange);
            } else {
               // Display data from the begining.
               xAxis.time.min = tmin;
               xAxis.time.max = moment(tmin).add(chartRange);
            }
            
            if (xAxis.time.unit == 'month') {
               var m = moment(xAxis.time.min);
               for (var i = 0; i<binCount; ++i) {
                  if (xAxis.time.absolute) 
                     chart.data.labels[i] = m.format(xAxis.time.tooltipFormat);
                  else   
                     chart.data.labels[i] = (m.year()-moment(0).year())*12 + m.month()+1;
                  m.add(1, 'month');
               }
            } else if (xAxis.gridLines.offsetGridLines) {
               for (var i = 0; i<binCount; ++i)
                  chart.data.labels[i] = moment(xAxis.time.min+i*xAxis.time.step).format(xAxis.time.tooltipFormat); 
            } else {
               for (var i = 0; i<=binCount; ++i)
                  chart.data.labels[i] = moment(xAxis.time.min+i*xAxis.time.step).format(xAxis.time.tooltipFormat); 
            }
            binSize = (xAxis.time.max-xAxis.time.min)/binCount;   
      }

      updateBins();
   }

   


   // Private: This function builds and repopulates the bins from the currently loaded data.
   function updateBins () {
      var bins = [], idx;
      for (var i=0; i<binCount; ++i)
         bins[i] = 0;
      for (var i=0; i<data.length; ++i) {
         switch (xAxis.id) {
            case 'x'  : idx = Math.floor((data[i].x-xmin)/binSize); break;
            case 't'  : 
               if (xAxis.time.unit != 'month') {
                  idx = Math.floor((data[i].x-moment(xAxis.time.min))/binSize); 
               } else {
                  var m = moment(data[i].x-moment(xAxis.time.min));
                  idx = (m.year()-moment(0).year())*12+m.month();
               }
               break;
         }
         if (idx >= 0 && idx < binCount)
            bins[idx] += data[i].y;   
      }  
      chart.data.datasets[0].data = bins;
      chart.update();
   }      
   
    
   // Private: add a row of data and update xmin/max
   function addDataRow (line) {
      var row = CSVtoArray (line), datum;
      if (row.length != 0) {
         if (xAxis.id == 'x')
            datum = { x: parseFloat(row[0]), y: parseFloat(row[1]) };
         else 
            datum = { x: moment(row[0]), y: parseFloat(row[1]) };
         if (xmin == null || datum.x < xmin) xmin = datum.x;
         if (xmax == null || datum.x > xmax) xmax = datum.x;
         data.push(datum);
      }
   }

   // Construct the chart. If we are binding to a root object that
   // already has an overlay then we can bypass the chart recreation.
   // This avoids fully reconstructing the chart after property edits.
   var div = createOverlay(root, chartarea, 'div');
   if (root.chart == undefined)
      rebuildChart();
   else {
      chart = root.chart;
      updateChart();
   }

   return this;

   // Test data:
   if (xAxisType == 'Numeric') {
      this.appendData("5,1")
      this.appendData("6,3")
      this.appendData("15,2")
      this.appendData("25,3")
      this.appendData("35,4")
      this.appendData("45,5")
      this.appendData("55,6")
      this.appendData("65,5")
      this.appendData("75,4")
      this.appendData("85,3")
      this.appendData("95,2")
   } else if (xAxisType == 'Time') {
      this.appendData("1970-01-01 12:25:40,1")
      this.appendData("1970-01-02 12:25:40,3")
      this.appendData("1970-01-03 12:25:40,1")
      this.appendData("2017-07-17 12:25:40,2")   
      this.appendData("2017-07-16 12:25:40,1")
      this.appendData("2017-07-17 12:25:40,2")   
      this.appendData("2017-07-18 12:25:50,3")   
      this.appendData("2017-07-19 12:25:40,4")   
      this.appendData("2017-07-20 12:25:50,10")   
      this.appendData("2017-07-21 12:25:40,4")   
      this.appendData("2017-07-22 12:25:40,3")   
      this.appendData("2017-07-23 12:25:40,2")   
      this.appendData("2017-07-24 12:25:40,1")   
      this.appendData("2017-07-25 12:25:40,2")   
   }

   return this;
}

