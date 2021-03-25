require('moment.js');
require('chart.js');

// Used by Chart.JS 2.6.0 but part of ECMA 6
Number.MAX_SAFE_INTEGER = 9007199254740991;
Number.MIN_SAFE_INTEGER = -9007199254740991;


function BarChart (root, config)  {
   var background = root.getElementsByTagName('rect')[0];
   var bounds = root.getElementsByTagName('rect')[1];
   var chartarea = root.getElementsByTagName('rect')[2];
   var obj = this;

   // Member variables:
   var chart = null;
   var ctx = null;
   var loadDataPending = false;
   var loadDataPendingRows = [];

   // Triggered whenever the property panel is used to change the...
   // properties.
   this.reconfigure = function(newInstance) {
      if (newInstance) {
         // Get rid of the design graphics:
         root.removeChild(root.getElementsByTagName('g')[0]);

         // Create an empty categorySet array:
         config.categorySet = {};

         // Create an empty seriesSet array:
         config.seriesSet = {};

         config.backGroundColour = background.style.fill;
      }

      // Set up the correct number of categories in the config object
      var rebuild = setConfigCategories(config.numCategories, config.categorySet);

      // Set up the correct number of series in the config object
      rebuild = setConfigSeries(config.numSeries, config.seriesSet) || rebuild;

      // Update the background colour:
      background.style.fill = config.backGroundColour;

      // If the number of series or categories is changed, we need to do
      // a full propterty panel rebuild, otherwise we can just update
      // the propterty values:
      if (rebuild) {
         setControlConfig(root, config);
         rebuildChart();
      } else {
         updateChart();
      }
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

      // Only needed for Chart.JS 2.2.2
      // chart.resize();
   };

   // Define a data series in the chart
   this.setSeries = function (_idx, _name, _colour) {
      obj.seriesSet[_idx] = { name: _name, colour: _colour, display: true };
      obj.seriesSet[_idx].dataset = {
          label: _name,
          backgroundColor : _colour,
          data : []
      };
      for (var i = 0; i < config.numCategories; ++i) {
         obj.seriesSet[_idx].dataset.data[i] = 0;
      }
   };

   // Show or hide a dataset on the chart
   this.setSeriesVisible = function (_idx, _display) {
      obj.seriesSet[_idx].display = _display;
      updateData();
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
   // This will trigger an ajax request to fetch the data so operates
   // asynchronously.
   //
   // Note that because this is asynchronous it can result in the file
   // data arriving after any recorded session data and this, in turn
   // can result in incorrect computation of the value of xmin for time
   // relative mode. Therefore, any appendData() calls that arrive
   // whilst we are awaiting the file data are queued and then processed
   // after the file data has arrived.
   //
   // The expected file format is
   // <seriesIdx>,<catIdx>,<val>
   // per line.
   this.loadData = function (file) {
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
                readCSVData(lines[i]);
         }

         // Process any queued data:
         for (var i in loadDataPendingRows)
            readCSVData(loadDataPendingRows[i]);
         loadDataPending = false;

         // Update the chart to display the new data:
         chart.update();
      });
   };

   // Load a single series' data from a file.
   //
   // This differs from loadData in that a file containing data from
   // only one series takes a different format: line n holds the value
   // for the series' catagory n.
   this.setSeriesDataFile = function (_seriesIdx, file) {
      // Start data queuing:
      loadDataPending = true;
      loadDataPendingRows = [];

      // Request the data file and load it into the graph:
      requestFile(file, function (text) {
         // Process series file:
         obj.seriesSet[_seriesIdx].data = [];
         if (text != null) {
            var values = text.split('\n');
            for (var i in values)
               obj.setData(_seriesIdx, i, values[i]);
         }

         // Process any queued data:
         for (var i in loadDataPendingRows)
            readCSVData(loadDataPendingRows[i]);
         loadDataPending = false;

         // Update the chart to display the new data:
         chart.update();
      });
   }

   // Read a data point from a CSV line
   // <seriesIdx>,<catIndex>,<value>
   this.readCSVData = function (line) {
      if (loadDataPending) {
         loadDataPendingRows.push(line)
      } else {
         var data = CSVtoArray (line);
         if (data.length == 3) {
            obj.setData(data[0], data[1], data[2]);
         }
      }
   }

   // Read a series from a CSV line
   // Here the line should be
   // <SeriesIdx>,<SeriesCat0Val>,<SeriesCat1Val>,...
   this.readMultCSVData = function (line) {
      var data = CSVtoArray(line);

      if (loadDataPending) {
         for (var i = 0; i < (data.length - 1); ++i) {
            loadDataPendingRows.push(data[0] + "," + i + "," + data[i]);
         }
      } else {
         if (data.length > 1) {
            var series = obj.seriesSet[data[0]];

            // Put remaining elements of data into dataset
            var dataToCopy = (data.length - 1);
            // Prevent writing to categories that don't exist
            if ((chart.data.labels != undefined) &&
               (chart.data.labels.length < dataToCopy)) {
                  dataToCopy = chart.data.labels.length;
               }
            for (var j = 0; j < dataToCopy; ++j) {
               series.dataset.data[j] = data[j+1];
            }
         }

         chart.update();
      }
   }

   // Clear all the data stored in the graph
   this.clearData = function () {
      for (var i in obj.seriesSet) {
         obj.clearSeriesData(i);
      }
   }

   this.clearSeriesData = function (_seriesIdx) {
      if ((_seriesIdx < obj.seriesSet.length) &&
          (obj.seriesSet[_seriesIdx].dataset != undefined)) {
         obj.seriesSet[_seriesIdx].dataset.data = [];
      }
      updateData(); // Must be here in case this method is used outsidde
                    // clearData (otherwise could be called only once
                    // from clearData)
   }

   // Private: Update _categorySet with defaults according to
   // _numCategories, removing any extraneous ones.
   function setConfigCategories(_numCategories, _categorySet) {
      var rebuild = false;

      for (var i = 0; i < 30; ++i) {
         if (i < _numCategories) {
            // If the category doesn't already exist construct a
            // default.
            if (_categorySet[i] == undefined) {
               _categorySet[i] = "Category " + i;
               rebuild = true;
            }
         } else if (_categorySet[i] != undefined) {
            // Remove extraneous category.
            delete _categorySet[i];
            rebuild = true;
         }
      }

      return rebuild;
   }

   // Private: Update _seriesSet with defaults according to _numSeries,
   // removing any extraneous ones.
   function setConfigSeries(_numSeries, _seriesSet) {
      var rebuild = false;

      for (var i = 0; i < 10; ++i) {
         if (i < _numSeries) {
            // If the series doesn't already exist construct a default.
            if ((_seriesSet[i] == undefined) || (_seriesSet[i].name == undefined)) {
               _seriesSet[i] = { name : "Series " + i, colour : "#000000", display : true };
               rebuild = true;
            }
         } else if (_seriesSet[i] != undefined) {
            // Remove the data series
            delete _seriesSet[i];
            rebuild = true;
         }
      }

      return rebuild;
   }

   // Private: update/rebuild the chart from the config options.
   function rebuildChart () {
      // Destroy any existing chart:
      if (chart != null) {
         chart.destroy();
         obj.seriesSet = [];
         obj.categorySet = [];
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
         data : { datasets: [] },
         options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                  xAxes: [{
                     type: 'category'
                  }]
            }
         }
      });

      // Store this in the root object so that we can retrieve it on reconstruction/rebinding.
      root.chart = chart;

      updateChart();
   }

   this.getConfig = function () {
      return config;
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
      chart.options.scales.yAxes[0].ticks.fontSize = parseInt(config.scaleFontSize);

      // Copy the series from config:
      obj.seriesSet = [];
      for (var i = 0; i < config.numSeries; ++i) {
         var series = config.seriesSet[i];
         if (series != undefined) {
            obj.setSeries(i, series.name, series.colour);
         }
      }

      // Copy the categories from config:
      obj.categorySet = [];
      for (var i = 0; i < config.numCategories; ++i) {
         obj.categorySet[i] = config.categorySet[i];
      }

      // Update the Y axis settings.
      obj.setYRange(config.minY, config.maxY);

      // Update the chart categories and data and re-draw.
      updateCategories();
      updateData();
   }

   // Private: update the displayed datasets, and re-draw.
   function updateData() {
      var datasets = [];
      var i, j = 0;
      for (i in obj.seriesSet) {
          if (obj.seriesSet[i].display) {
             if (obj.seriesSet[i].dataset != null) {
                datasets[j++] = obj.seriesSet[i].dataset;
             }
          }
      }

      // Display only the visible datasets:
      chart.data.datasets = datasets;
      chart.update();
   }

   // Private: update the chart with obj's categories, and re-draw.
   function updateCategories() {
      chart.data.labels = obj.categorySet;
      chart.update();
   }

   // Set the given category value of the given series to catValue.
   this.setData = function (seriesIdx, catIdx, catValue) {
      obj.seriesSet[seriesIdx].dataset.data[catIdx] = catValue;
      chart.update();
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
}

