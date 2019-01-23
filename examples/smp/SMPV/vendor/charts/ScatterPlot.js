var ActorsPositions = JSON.parse(sessionStorage.getItem("ActorsPositions"));
var whyActorChanged = JSON.parse(sessionStorage.getItem("BargnsData"));
var NumOfTurns = sessionStorage.getItem("NumOfTurns");
var svgWidth2 = 550;
var svgheight2=300;
var effpow = JSON.parse(sessionStorage.getItem("effpow"));
var defaultColors = sessionStorage.getItem("defaultColors");
ActorsObj2 = JSON.parse(sessionStorage.getItem("ActorsObj"));

function drawScatterPlot() {

  d3.select("#ScatterPlot").html("");

  var margin = { top: 30, right: 20, bottom: 30, left: 50 },
      width = svgWidth2 - margin.left - margin.right,
      height = svgheight2 - margin.top - margin.bottom;
      

  var x = d3.scaleLinear()
      .range([0, width]);

  var y = d3.scaleLinear()
      .range([height, 0]);

   var r = d3.scaleSqrt()
  		.range([2,10]);

  var xAxis = d3.axisBottom()
  		.scale(x);

  var yAxis = d3.axisLeft()
  		.scale(y);

  var color = d3.scaleOrdinal(["#5C8598","#219DD8","#96C9E5","#3C3D3B","#ECCE6A","#f8ecba"]);

  var symbols = d3.scaleOrdinal(d3.symbols);

  // creates a generator for symbols
  var symbol = d3.symbol().size(100);

  var svg = d3.select('#ScatterPlot').append("svg")
      .attr("width", width + margin.left + margin.right)
      .attr("height", height + margin.top + margin.bottom)
    .append("g")
      .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

  var tooltip = d3.select("#ScatterPlot").append("div")
					.attr("class", "tooltipScatter")
					.style("opacity", 0);




var selectedScenNum = selectedScen;
var allPositions = arrPos;


var data = []
var dimensions = allPositions[selectedScenNum];
for (var d = 0; d < dimensions.length; d++) {
  
  var actors = dimensions[d];
  for (var a = 0; a < actors.length; a++) {

    var positions = actors[a]['positions'];
    var actorName = actors[a]['name'];
    for (var t = 0; t < positions.length ; t++) {
      data.push({"turn":t,"dim":d,"actor":actorName,"position":positions[t]})
    }
  }

}

  	data.forEach(function(d){

      d.position = +d.position;
      d.turn  = +d.turn;
      d.actor = d.actor;
      d.dim  = +d.dim;
    });

	x.domain(d3.extent(data, function(d){
    // console.log(d.position)
		return d.position;
	})).nice();

	y.domain(d3.extent(data, function(d){
		return d.turn;
	})).nice();

	r.domain(d3.extent(data, function(d){
		return d.dim;
	})).nice();

  svg.append('g')
    .attr('transform', 'translate(0,' + height + ')')
    .attr('class', 'x axis')
    .call(xAxis);

  svg.append('g')
    .attr('transform', 'translate(0,0)')
    .attr('class', 'y axis')
    .call(yAxis);

	svg.append('text')
		.attr('x', 10)
		.attr('y', 10)
		.attr('class', 'label')
		.text('Turn');

	svg.append('text')
		.attr('x', width)
		.attr('y', height - 10)
		.attr('text-anchor', 'end')
		.attr('class', 'label')
		.text('Position');

  // we use the ordinal scale symbols to generate symbols
  // such as d3.symbolCross, etc..
  // -> symbol.type(d3.symbolCross)()
  svg.selectAll("circle")
   .data(data)
 .enter().append("circle")
   .attr("class", "circle")
   .attr("r", 5)
   .attr("cx", function(d) { return x(d.position); })
   .attr("cy", function(d) { return y(d.turn); })
   .style("fill", function(d) { return color(d.actor); });

   var legend = svg.selectAll(".legend")
      .data(color.domain())
    .enter().append("g")
      .attr("class", "legend")
      .attr("transform", function(d, i) { return "translate(0," + i * 20 + ")"; });

  legend.append("rect")
      .attr("x", width - 18)
      .attr("width", 18)
      .attr("height", 18)
      .style("fill", color);

  legend.append("text")
      .attr("x", width - 24)
      .attr("y", 9)
      .attr("dy", ".35em")
      .style("text-anchor", "end")
      .text(function(d) { return d;
        });

    svg.selectAll(".symbol")
    .data(data)
    .enter().append("path")
    .attr("class", "symbol")
    .attr("d", function(d, i) { return symbol.type(symbols(d.dim))(); })
    .style("fill", function(d) { return color(d.actor); })
    .attr("transform", function(d) {
      return "translate(" + x(d.position) + "," + y(d.turn) +")";
     });
  var clicked = ""

  var legend1 = svg.selectAll(".legend")
    .data(color.domain())
  .enter().append("g")
    .attr("class", "legend")
    .attr("transform", function(d, i) { return "translate(0," + i * 20 + ")";
   });

   legend1.append("path")
  .style("fill", function(d) { return color(d); })
    	.attr("d", function(d, i) { return symbol.type(symbols(d))(); })
	    .attr("transform", function(d, i) {
    		return "translate(" + (width -10) + "," + 10 + ")";
  		})

}
