//Based on C3.js (http://c3js.org). See Website for examples!

document.body.style.backgroundColor = 'rgb(240, 240, 240)';

var minHeight = 255;

var chart;
GenerateChart('bar', false, false)

var chartData;
var xValues=[];
var yValues=[];
var xs = {};
var dataColors = {};
var chartTypes = {};
var lineStyle = {};

//Is executed when js is loaded first.
//Extracts relevant information from chartData in variables
window.onload = function() 
{
  new QWebChannel(qt.webChannelTransport, function(channel) {
    chartData = channel.objects.chartData;

	var count = 0;
	for(var propertyName in channel.objects) {
		if (propertyName != 'chartData'){
			var xDataTemp = channel.objects[propertyName].m_XData
			var yDataTemp = channel.objects[propertyName].m_YData
			var dataLabelsTemp;
			//add label to x array
			xDataTemp.unshift('x'+count.toString())
			dataLabelsTemp = channel.objects[propertyName].m_Label

			xs[dataLabelsTemp] = 'x'+count.toString()
			
			xDataTemp.push(null); //append null value, to make sure the last tick on x-axis is displayed correctly
			yDataTemp.unshift(dataLabelsTemp)
			yDataTemp.push(null); //append null value, to make sure the last tick on y-axis is displayed correctly
			xValues[count] = xDataTemp
			yValues[count] = yDataTemp
			dataColors[dataLabelsTemp] = channel.objects[propertyName].m_Color
			chartTypes[dataLabelsTemp] = channel.objects[propertyName].m_ChartType

			if (channel.objects[propertyName].m_LineStyleName=="solid"){
				lineStyle[dataLabelsTemp]= ''
			}
			else {
				lineStyle[dataLabelsTemp]= [{'style':'dashed'}]
			}

			count++;
		}
	}
    setupChart(chartData)

  });
}

//This is necessary to resize the chart, after the size of the parent changed
window.onresize = function () {
  var size = window.innerHeight-(window.innerHeight/100*10); //subtract 5% of height to hide vertical scrool bar
  
  if (size < minHeight)
  {
    size = minHeight;
  }

  chart.resize({
    height: size,
  });
}

function ReloadChart(showSubchart)
{ 
    chartData.m_ShowSubchart = showSubchart;
    
    setupChart(chartData);
}

function setupChart(chartData)
{
  window.onresize();

  GenerateChart(chartData.m_DiagramTypeName, chartData.m_ShowSubchart, chartData.m_UsePercentageInPieChart, 
  chartData.m_xAxisLabel, chartData.m_yAxisLabel, chartData.m_diagramTitle, chartData.m_LegendPosition, chartData.m_DataPointSize, chartData.m_YAxisScale)
    
  chart.unload(); //unload data before loading new data
  
  //for multiple xy line chart, see http://c3js.org/samples/simple_xy_multiple.html
  var columns = [];
  for (var i in xValues){
	columns.push(xValues[i])
  }  
  for (var i in yValues){
	columns.push(yValues[i])
  }

  chart.load({
	  xs: xs,
		columns: columns
  });
}

//Transformation between different chart types
//takes the name of the chart type as a parameter
//called by QmitkC3jsWidget
function transformView(TransformTo) {
  chart.transform(TransformTo);
};

function changeTheme(color) {
link = document.getElementsByTagName("link")[0];
  if (color == 'dark') {
    link.href = "Chart_dark.css";
  }
  else
  {
    link.href = "Chart.css";
  }
};

//Here, the chart magic takes place. C3js is called
//chartType: either bar, line or pie
//showSubchart: see http://c3js.org/samples/options_subchart.html
//usePercentageInPieChart: percentage labels (only for pie chart)
function GenerateChart(chartType, showSubchart, usePercentageInPieChart, xAxisLabel, yAxisLabel, title, legendPosition, dataPointSize, YAxisScale)
{
	//adaption for bar ratio indepenend of amount of data points
	//otherwise, bars could be covered.
	var barRatio;
	try {
		barRatio = 0.8*Math.exp(-0.015*xValues[0].length);
	}
	catch (err){
		barRatio=0.42
	}
	var formatCharacter;
	if (usePercentageInPieChart==true){
		formatCharacter = '%'
	}
	else{
		formatCharacter = '.1f'
	}
  chart = c3.generate({
	title:{
		text: title,
		position: 'top-center'
	},
    data: {
        xs: {}, //use first "column" as x axis values
        columns: [],  //initialize empty. Data will be loaded in function setupChart(chartData)
        types: chartTypes,
        selection: {
          enabled: false,
          multiple: false,
        },
		colors: dataColors,
		regions: lineStyle,
    },
    legend: {
        position: legendPosition,
		show: true
    },
    grid: {
      y: {
        lines: [{value:0}] //Draws a horizontal line at y=0
      }
    },
    bar: {
        width: {
            ratio: barRatio
        }
    },
	pie:{
		label: {
		   format: function (value, ratio, id) {
				if (usePercentageInPieChart==true){
					return d3.format('%') (ratio);
				}
				else{
					return value;
				}
			}
		}
	},
    zoom: {
        enabled: true,
    },
    subchart: {
        show: showSubchart  //Shows a subchart that shows the region the primary chart is zoomed in to by overlay.
    }, 
    axis: {
        x: {
          tick: {
            multiline: false,
            fit: false, //to make more x labels appear on zoom
            centered: true,
			format: d3.format(".1f"),
          },
		  label: {
			text: xAxisLabel,
			position: 'outer-center'
		  }
        },
        y: {
          tick: {
            format: d3.format(formatCharacter)
          },
		  label: {
			text: yAxisLabel,
			position: 'outer-middle'
		  },
		  scale: {
            name: YAxisScale
          }
        }
    },
	tooltip: {
		format: {
			title: function (x) { return xAxisLabel +  ': ' +  d3.format(".2f")(x)}
		},
	},
    //Style data points in linegraph
    point: {
        r: dataPointSize,
        focus: 
        {
          expand: {
            r: dataPointSize + 2
          }
        }
    },

  });
}
