var ActorsPositions = JSON.parse(sessionStorage.getItem("ActorsPositions"));
var whyActorChanged = JSON.parse(sessionStorage.getItem("BargnsData"));
var NumOfTurns = sessionStorage.getItem("NumOfTurns");
var svgWidth = 550;
var svgheight=300;
var effpow = JSON.parse(sessionStorage.getItem("effpow"));
var defaultColors = sessionStorage.getItem("defaultColors");
ActorsObj2 = JSON.parse(sessionStorage.getItem("ActorsObj"));

function draw2DimPlot() {

  d3.select("#twoDimPlot").html("");

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

var data=[
  {"cereal":"Apple_Cinnamon_Cheerios","manufacturer":"General Mills","calories":110,"sugar":10},
  {"cereal":"Apple_Jacks","manufacturer":"Kelloggs","calories":112,"sugar":14},
  {"cereal":"Cap'n'Crunch","manufacturer":"Quaker Oats","calories":120,"sugar":12},
  {"cereal":"Cheerios","manufacturer":"General Mills","calories":110,"sugar":1},
  {"cereal":"Cinnamon_Toast_Crunch","manufacturer":"General Mills","calories":120,"sugar":9},
  {"cereal":"Cocoa_Puffs","manufacturer":"General Mills","calories":110,"sugar":13},

];
  data.forEach(function(d) { // convert strings to numbers
      d.calories = +d.calories;
      d.sugar = +d.sugar;
  });

// Common pattern for defining vis size and margins
var margin = { top: 30, right: 20, bottom: 30, left: 50 },
width = svgWidth - margin.left - margin.right,
height = svgheight - margin.top - margin.bottom;

// Add the visualization svg canvas to the vis-container <div>
var canvas = d3.select("#twoDimPlot").append("svg")
    .attr("width",  width  + margin.left + margin.right)
    .attr("height", height + margin.top  + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

// Define our scales
var colorScale = d3.scaleOrdinal(["#5C8598","#219DD8","#96C9E5","#3C3D3B","#ECCE6A","#f8ecba"]);

var xScale = d3.scaleLinear()
    .domain([ d3.min(data, function(d) { return d.sugar; }) - 1,
              d3.max(data, function(d) { return d.sugar; }) + 1 ])
    .range([0, width]);

var yScale = d3.scaleLinear()
    .domain([ d3.min(data, function(d) { return d.calories; }) - 1,
              d3.max(data, function(d) { return d.calories; }) + 1 ])
    .range([height, 0]); // flip order because y-axis origin is upper LEFT


var xAxis = d3.axisBottom()
.scale(xScale);

var yAxis = d3.axisLeft()
    .scale(yScale);



// gridlines in x axis function
function make_x_gridlines() {		
  return d3.axisBottom(xScale)
      .ticks(1)
}

// gridlines in y axis function
function make_y_gridlines() {		
  return d3.axisLeft(yScale)
      .ticks(1)
}
// add the X gridlines
canvas.append("g")
.attr("transform", "translate("+width/2+"," + height + ")")
.attr("class", "grid")
.style("stroke-opacity", "0.2")
.style("stroke-dasharray", "2")
.style("shape-rendering", "crispEdges")
.call(make_x_gridlines()
    .tickSize(-height)
    .tickFormat("")
);

 // add the Y gridlines
 canvas.append("g")
 .attr("transform", "translate(0,90)")
.attr("class", "grid")
 .style("stroke-opacity", "0.2")
 .style("stroke-dasharray", "2")
 .style("shape-rendering", "crispEdges")
 .call(make_y_gridlines()
     .tickSize(-width)
     .tickFormat("")
 );

// Add x-axis to the canvas
canvas.append("g")
    .attr("class", "x axis")
    .attr("transform", "translate(0," + height + ")") // move axis to the bottom of the canvas
    .call(xAxis)
  .append("text")
    .attr("class", "label")
    .attr("x", width) // x-offset from the xAxis, move label all the way to the right
    .attr("y", -6)    // y-offset from the xAxis, moves text UPWARD!
    .style("text-anchor", "end") // right-justify text
    .text("Sugar");

// Add y-axis to the canvas
canvas.append("g")
    .attr("class", "y axis") // .orient('left') took care of axis positioning for us
    .call(yAxis)
  .append("text")
    .attr("class", "label")
    .attr("transform", "rotate(-90)") // although axis is rotated, text is not
    .attr("y", 15) // y-offset from yAxis, moves text to the RIGHT because it's rotated, and positive y is DOWN
    .style("text-anchor", "end")
    .text("Calories");

// Add the tooltip container to the vis container
// it's invisible and its position/contents are defined during mouseover
var tooltip = d3.select("#twoDimPlot").append("div")
    .attr("class", "tooltip")
    .style("opacity", 0);

// tooltip mouseover event handler
var tipMouseover = function(d) {
    var color = colorScale(d.manufacturer);
    var html  = d.cereal + "<br/>" +
                "<span style='color:" + color + ";'>" + d.manufacturer + "</span><br/>" +
                "<b>" + d.sugar + "</b> sugar, <b/>" + d.calories + "</b> calories";

    tooltip.html(html)
        .style("left", (d3.event.pageX + 15) + "px")
        .style("top", (d3.event.pageY - 28) + "px")
      .transition()
        .duration(200) // ms
        .style("opacity", .9) // started as 0!

};
// tooltip mouseout event handler
var tipMouseout = function(d) {
    tooltip.transition()
        .duration(300) // ms
        .style("opacity", 0); // don't care about position!
};

// Add data points!
canvas.selectAll("circle")
  .data(data)
.enter().append("circle")
  .attr("class", "circle")
  .attr("r", 5.5) // radius size, could map to another data dimension
  .attr("cx", function(d) { return xScale( d.sugar ); })     // x position
  .attr("cy", function(d) { return yScale( d.calories ); })  // y position
  .style("fill", function(d) { return colorScale(d.manufacturer); })
  .on("mouseover", tipMouseover)
  .on("mouseout", tipMouseout);

  var legend = canvas.selectAll(".legend")
      .data(colorScale.domain())
    .enter().append("g")
      .attr("class", "legend")
      .attr("transform", function(d, i) { return "translate(0," + i * 20 + ")"; });

  legend.append("rect")
      .attr("x", width - 18)
      .attr("width", 18)
      .attr("height", 18)
      .style("fill", colorScale);

  legend.append("text")
      .attr("x", width - 24)
      .attr("y", 9)
      .attr("dy", ".35em")
      .style("text-anchor", "end")
      .text(function(d) { return d;
        });




}
