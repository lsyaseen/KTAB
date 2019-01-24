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

console.log(document.getElementById('statusQuoX').value);
var statusQuoX = document.getElementById('statusQuoX').value;
var statusQuoY = document.getElementById('statusQuoY').value;

var xRedDot= (statusQuoX != null) ?  statusQuoX : 40;
var yRedDot= (statusQuoY != null) ?  statusQuoY : 60;;
// document.getElementById('statusQoay').value;
var data=[
  {"actor":"XY","dim1pos":50,"dim2pos":90, "rx": 285, "ry": 40, "angle":0},
  {"actor":"LK","dim1pos":90,"dim2pos":20, "rx": 110, "ry": 150, "angle":0},
  {"actor":"HLJ","dim1pos":30,"dim2pos":50, "rx": 70, "ry": 93, "angle":0},
];

  data.forEach(function(d) { // convert strings to numbers
      d.dim1pos = +d.dim1pos;
      d.dim2pos = +d.dim2pos;
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
var colorScale = d3.scaleOrdinal(["#3366cc", "#109618", "#990099", "#0099c6", "#ff9900", "#dd4477", "#66aa00", "#b82e2e", "#316395", "#994499", "#22aa99", "#aaaa11", "#6633cc", "#e67300", "#8b0707", "#651067", "#329262", "#5574a6", "#3b3eac"]);

var xScale = d3.scaleLinear()
               .domain([0,100])
    // .domain([ d3.min(data, function(d) { return d.dim2pos; }) - 1,
    //           d3.max(data, function(d) { return d.dim2pos; }) + 1 ])
    .range([0, width]);

var yScale = d3.scaleLinear()
               .domain([0,100])
    // .domain([ d3.min(data, function(d) { return d.dim1pos; }) - 1,
    //           d3.max(data, function(d) { return d.dim1pos; }) + 1 ])
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
 .attr("transform", "translate(0,"+height/2+")")
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
    .text("dim2pos");

// Add y-axis to the canvas
canvas.append("g")
    .attr("class", "y axis") // .orient('left') took care of axis positioning for us
    .call(yAxis)
  .append("text")
    .attr("class", "label")
    .attr("transform", "rotate(-90)") // although axis is rotated, text is not
    .attr("y", 15) // y-offset from yAxis, moves text to the RIGHT because it's rotated, and positive y is DOWN
    .style("text-anchor", "end")
    .text("dim1pos");

// Add the tooltip container to the vis container
// it's invisible and its position/contents are defined during mouseover
var tooltip = d3.select("#twoDimPlot").append("div")
    .attr("class", "tooltip")
    .style("opacity", 0);

// tooltip mouseover event handler
var tipMouseover = function(d) {
    var color = colorScale(d.actor);
    var html  = "<span style='color:" + color + ";'>" + d.actor + "</span><br/>" +
                "<b>" + d.dim2pos + "</b> dim2pos, <b/>" + d.dim1pos + "</b> dim1pos";

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
  .attr("r", 4) // radius size, could map to another data dimension
  .attr("cx", function(d) { return xScale( d.dim2pos ); })     // x position
  .attr("cy", function(d) { return yScale( d.dim1pos ); })  // y position
  .style("fill", function(d) { return colorScale(d.actor); })
  .on("mouseover", tipMouseover)
  .on("mouseout", tipMouseout);


  canvas.append("circle")
  .attr("r", 2) // radius size, could map to another data dimension
  .attr("cx", xScale(xRedDot))     // x position
  .attr("cy", yScale(yRedDot))  // y position
  .attr("fill", "red");


  canvas.selectAll("ellipse")
  .data(data)
  .enter().append("ellipse")
  .attr("rx", function(d) { return d.rx ; }) // radius size, could map to another data dimension
  .attr("ry", function(d) { return d.ry ; }) // radius size, could map to another data dimension
  .attr("cx", function(d) { return xScale( d.dim2pos ); })     // x position
  .attr("cy", function(d) { return yScale( d.dim1pos ); })  // y position
  .style("fill","none" )
  .style("stroke", function(d) { return colorScale(d.actor); })
  .attr("transform", function(d) { return "rotate("+d.angle +","+xScale( d.dim2pos )+","+yScale( d.dim1pos )+")";})




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
      .text(function(d) { return d;});




}
