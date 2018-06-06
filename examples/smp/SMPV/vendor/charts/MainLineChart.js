// Define margins        
var margin = { top: 30, right: 20, bottom: 30, left: 50 },
  width = 500 - margin.left - margin.right,
  height = 270 - margin.top - margin.bottom;

// data from load.js (session data)
var ActorsNamesAllSce = JSON.parse(sessionStorage.getItem("ActorsNames"));
var ActorsPositions = JSON.parse(sessionStorage.getItem("ActorsPositions"));
var whyActorChanged = JSON.parse(sessionStorage.getItem("BargnsData"));
var NumOfTurns = sessionStorage.getItem("NumOfTurns");
var selectedDimNum = 0;
var selectedScenNum = sessionStorage.getItem("selectedScen");

function drawLine() {

  // Clear the exiting chart
  d3.select("#chart").html("");
  d3.select("#legend").html("");

  var svg = d3.select("#chart")
    .append("svg")
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("preserveAspectRatio", "xMidYMid meet")
    .attr("viewBox", "0 0 500 300")
    .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")")

  var svg2 = d3.select("#legend")
    .append("svg")
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("preserveAspectRatio", "xMidYMid meet")
    .attr("viewBox", "0 0 250 500")
    .attr('transform', "translate(" + 15 + "," + 15 + ")")
    .append("g")
    .attr("class", "legend1")

  var x_axix_data = [], // create an array for x-axix 
    XScale,
    YScale,
    xAxis,
    yAxis,
    turns,
    turn,
    line,
    positionsData = [],
    namesArray = [],
    bargnsData = [],
    selectedLine,
    selectedLegend;

  //names based on selected scenario
  namesArray = ActorsNamesAllSce[selectedScenNum];

  //positions based on selected Dim
  for (var i = 0; i < ActorsPositions[selectedScenNum][selectedDimNum].length; i += 1) {
    positionsData.push(ActorsPositions[selectedScenNum][selectedDimNum][i].positions);
  }

  //bargains based on selected Dim and scenario
  for (var i = 0; i < whyActorChanged[selectedScenNum][selectedDimNum][0].length; i += 1) {
    bargnsData.push(whyActorChanged[selectedScenNum][selectedDimNum][0][i]);
  }

  turn = currentTurn; //current turn from slider
  turns = NumOfTurns - 1; //cuz we're starting from 0

  //define the scales
  XScale = d3.scaleLinear().domain([0, turn]).range([0, width]);
  YScale = d3.scaleLinear().domain([0, 100]).range([height, 0]);

  // Define the axes
  xAxis = d3.axisBottom(XScale).scale(XScale).ticks(turn);
  yAxis = d3.axisLeft(YScale).scale(YScale);

  // Define the line
  line = d3.line()
    .x(function (d) { return XScale(d.Turn); })
    .y(function (d, i) { return YScale(d.val[i]); });


  for (var i = 0; i < turn + 1; i++) {
    x_axix_data.push(i);
  }

  // add the X gridlines
  svg.append("g")
    .attr("class", "grid")
    .attr("transform", "translate(0," + height + ")")
    .call(make_x_gridlines()
      .tickSize(-height)
      .tickFormat("")
    );

  // add the Y gridlines
  svg.append("g")
    .attr("class", "grid")
    .call(make_y_gridlines()
      .tickSize(-width)
      .tickFormat("")
    );

  // Add the X Axis
  svg.append("g")
    .attr("class", "x axis")
    .attr("transform", "translate(0," + height + ")")
    .call(xAxis);

  //text label for the x axis
  svg.append("text")
    .attr("transform",
    "translate(" + (width / 2) + " ," +
    (height + 30) + ")")
    .style("text-anchor", "middle")
    .text("Turn");

  // Add the Y Axis
  svg.append("g")
    .attr("class", "y axis")
    .call(yAxis);

  svg.selectAll(".grid line")
    .style("stroke", "lightgrey")
    .style("stroke-opacity", " 0.7")
    .style("stroke-dasharray", "2");

  // text label for the y axis
  svg.append("text")
    .attr("transform", "rotate(-90)")
    .attr("y", 0 - margin.left)
    .attr("x", 0 - (height / 2))
    .attr("dy", "1em")
    .style("text-anchor", "middle")
    .text("Position");

  var colors = d3.scaleOrdinal()
    .domain(namesArray)
    .range(["#5C8598", "#219DD8", "#96C9E5", "#3C3D3B",
      "#ECCE6A", "#f8ecba", "#60805D", "#8AC791",
      "#bebfc1", "#636664", "#a5f3cc", "#6acbec",
      "#6aecec", "#ff9966", "#1d80e2", "#6a8aec",
      "#a5a5f3", "#ffd857", "#a8bfa6", "#cb6aec",
      "#c88dc8", "#ec6a6a", "#ec6aab", "#e84a72",
      "#f1a7a7", "#e3994f", "#d87d22", "#d8ab22",
      "#d8d822", "#a6e765", "#4fd822", "#1cb01c",
      "#22d84f", "#22d87d", "#22d8ab", "#22d8d8",
      "#22abd8", "#156184", "#d3e6f8", "#224fd8",
      // Darker Shade for (Set #1)
      "#436170", "#18719a", "#5cabd6", "#1a1a19",
      "#e5bb34", "#f1d874", "#425940", "#75bd7e",
      "#97989b", "#3f4040", "#62eaa6", "#1db1e2",
      "#1de2e2", "#ff5500", "#124d87", "#1a47cb",
      "#4b4be7", "#ffc91a", "#6e946b", "#b11de2",
      "#ac53ac", "#e21d1d", "#e21d80", "#b5173f",
      "#e34f4f", "#b0661c", "#844d15", "#846815",
      "#848415", "#73c71f", "#318415", "#0e580e",
      "#158431", "#15844d", "#158468", "#158484",
      "#156884", "#07202c", "#7bb4ea", "#153184",
      // Lighter Shade for (Set #2)
      "#8faebc", "#7bc7ea", "#d6eaf5", "#737570",
      "#f9f0d2", "#fdf9e8", "#9ab497", "#ddeedf",
      "#e5e5e6", "#8b8d8c", "#e9fcf2", "#d2eff9",
      "#bbf6f6", "#ffddcc", "#8ebff0", "#d2dcf9",
      "#e9e9fc", "#fff3cc", "#e2eae1", "#efd2f9",
      "#e6cbe6", "#f9d2d2", "#f9d2e6", "#f6bbca",
      "#fce9e9", "#f5d9bd", "#eab37b", "#eace7b",
      "#eaea7b", "#d9f5bd", "#97ea7b", "#65e765",
      "#7bea97", "#7beab3", "#38e0b6", "#65e7e7",
      "#7bceea", "#38abe0", "#e9f3fc", "#91a8ee",
      // Another set of 18 (warm) Distinct Colors (Set #2)
      "#e6194b", "#3cb44b", "#ffe119", "#0082c8",
      "#f58231", "#911eb4", "#46f0f0", "#f032e6",
      "#d2f53c", "#fabebe", "#008080", "#e6beff",
      "#aa6e28", "#fffac8", "#800000", "#aaffc3",
      "#808000", "#ffd8b1", "#000080", "#808080",
      // Darker shade for set #2
      "#8a0f2e", "#267330", "#b39b00", "#004266",
      "#c35709", "#58126d", "#0fbdbd", "#be0eb5",
      "#a0c20a", "#f47171", "#003333", "#c466ff",
      "#674218", "#fff266", "#330000", "#66ff94",
      "#333300", "#ff9933", "#000033", "#4d4d4d"
    ]);


  //create an object for each actor, map all the properties.
  var actors = namesArray.map(function (row, i) {

    return {
      actor_name: row,
      visible: true,
      values: x_axix_data.map(function (x_value) {
        return {
          Turn: x_value,
          val: positionsData[i].map(function (y_values) {
            return y_values
          })
        }
      }),
      color: colors(row)
    }
  });

  // get initiator color for the point  
  for (i = 0; i < bargnsData.length; i++) {
    let obj = actors.findIndex(o => o.actor_name === bargnsData[i].Initiator);
    bargnsData[i]["color"] = actors[obj].color;
  }


  //initialize bargains points visibilaty status  
  bargnsData.forEach(function (obj) { obj["PointVisible"] = false; });

  //to show details about each bargain
  var tooltip = d3.select('#chart')
    .append('div')
    .attr('class', 'tooltip')
    .style("display", "none")
    .style('position', 'absolute')
    .style('background', '#fcfcfc')
    .style('z-index', 1);

  tooltip.append('div')
    .attr('class', 'label')
    .style('height', '100%')
    .style('padding', '7%')
    .style('line-height', '15px')
    .style('pointer-events', 'none')
    .style('color', 'black');


  actors.forEach(function (d, i) {

    //draw the lines
    drawLines(d, i);
    d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", '')).transition().duration(100)
      .style("stroke", function () { return d.visible ? d.color : "#F1F1F2"; })
      .style("opacity", function () { return d.visible ? 1 : 0; })

    //add the legend
    svg2.append("rect")
      .attr("width", 10)
      .attr("height", 10)
      .attr("id", 'legend_' + d.actor_name.replace(".", ''))
      .attr("transform", function () {
        xOff = (i % 3) * 65
        yOff = Math.floor(i / 3) * 20
        return "translate(" + xOff + "," + (yOff + 80) + ")"
      })
      .attr("fill", function () {
        return d.visible ? d.color : "#F1F1F2";
      })
      .on("click", function () {
        d3.select(this).attr("fill", function () {
          return d.visible ? d.color : "#F1F1F2";
        })

        // Hide or show the elements based on the ID
        d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
          .transition().duration(100)
        if (d.visible == true) {
          d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
            .transition().remove();
          actors[i].visible = false;
          d.visible = false;
        }
        else if (d.visible == false) {
          drawLines(d, i);
          actors[i].visible = true;
          d.visible = true;
        }
      })
      .on("mouseover", function () {
        selectedLine = "#Line_" + d.actor_name;
        selectedLegend = "#legend_" + d.actor_name;
        onMouseover(d, i);

      })
      // .on("mouseout", onMouseout);
      .on("mouseout", function () {
        MouseOutLegend(d, i);
        onMouseout();
      });

    //add bargns' points
    var dot = svg.selectAll(".dot")
      .data(bargnsData)
      .enter();

    svg2.append("text")
      .attr("transform", function () {
        xOff = (i % 3) * 65
        yOff = Math.floor(i / 3) * 20

        return "translate(" + (xOff + 15) + "," + (yOff + 89) + ")"
      })
      .text(function () { return d.actor_name })
      .attr("text-anchor", "start")
      .style("cursor", "pointer")
      .on("click", function () {
        //append points on selected line
        dot.append("circle")
          .attr("class", "dot")
          .attr("class", function (d) {
            return "dot" + d["Moved_Name"].replace(/\s+/, "").replace(".", '');
          })
        var visible = bargnsData[i]["PointVisible"] ? false : true;
        //show/hide Bargns' points
        if (bargnsData[i]["PointVisible"] == false) {

          //select all points on the selected line 
          d3.selectAll(".dot" + d.actor_name.replace(/\s+/, "").replace(".", ''))
            .attr("fill", function (d) {
              bargnsData[i]["PointVisible"] = d.PointVisible;
              return d.color;
            })
            .attr("r", 3.5)
            .attr("cx", function (d) { return XScale(d["turn"]); })
            .attr("cy", function (d) { return YScale(d["CurrPos"]); })
            .on("mouseover", function (d) {
              // tooltip.style("display", "inline");
              tooltip.style("display", null);

              d3.select(this).style("cursor", "pointer");
            })
            .on("mousemove", function (d) {
              var xPosition = d3.mouse(this)[0];
              var yPosition = d3.mouse(this)[1];
              d3.select('.label').html("From turn " + (d["turn"] - 1) + " to turn " + d["turn"] + " "
                + d["Moved_Name"] + "<br/> moved " + roundTo(d["Diff"], 2) + " from " + roundTo(d["PrevPos"], 2) + " to " + roundTo(d["CurrPos"], 2) + " as "
                + "<br/> a result of a bargain proposed " + "<br/>" + "by " + d["Initiator"] + " to " + d["Receiver"])

              tooltip
                // .style('top', (yPosition + 70) + 'px')
                // .style('left',(xPosition +90)  + 'px');
                .style('top', (yPosition) + 'px')
                .style('left', (xPosition) + 'px');
              // highlight the actor who initiated the bargain 
              d3.select("#legend_" + d["Initiator"].replace(".", ''))
                .style("stroke", "black")
                .style("stroke-width", "2.5");
            })
            .on("mouseout", function (d) {
              tooltip.style("display", "none");
              d3.select("#legend_" + d["Initiator"].replace(".", ''))
                .style("stroke", "none")
            });
          // Update whether or not points are visible
          bargnsData[i]["PointVisible"] = visible;
        }
        else {
          bargnsData[i]["PointVisible"] = visible;
          d3.selectAll(".dot" + d.actor_name.replace(/\s+/, "").replace(".", ''))
            .transition().remove();
        }
      });

  }) // end of forEach

  svg2.append("rect")
    .attr("width", 10)
    .attr("height", 10)
    .attr("y", 50)
    .attr("id", "select-All")
    .style("fill", "black")
    .on("click", function () {
      if (d3.select("#SelectLabel").text() == "Clear All") {
        bargnsData.forEach(function (d, i) {
          d.visible = false;
        })
        clearAll();
      }
      else {
        actors.forEach(function (d, i) {
          d.visible = true;
        })
        selectAll();
      }
    });

  svg2.append("text")
    .attr("x", 20)
    .attr("y", 58)
    .text("Clear All")
    .attr("id", "SelectLabel");


  function onMouseover(d, i) {
    MouseOverLegend(d, i);
    actors.forEach(function (d, i) {
      d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .transition()
        .duration(50)
        .style("opacity", function () {
          if ("#Line_" + d.actor_name === selectedLine)
            return 1.0
          else if (d.visible == true)
            return 0.2
          else
            MouseOutLegend(d, i); //remove line if visible = false
        })

      d3.select(selectedLine.replace(/\s+/g, '').replace(".", ''))
        .style("stroke-width", 4);

      d3.selectAll("#legend_" + d.actor_name.replace(".", ''))
        .attr("fill", function () {
          return ("#legend_" + d.actor_name === selectedLegend) ? d.color : "#F1F1F2"
        })
    })
  }

  function onMouseout() {
    actors.forEach(function (d, i) {
      d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .transition()
        .duration(50)
        .style("opacity", function () {
          if (d.visible == true)
            return 1;
          else
            d3.select("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", '')).remove(); //remove instead of chainging opacity
        })
      d3.select(selectedLine.replace(/\s+/g, '').replace(".", ''))
        .style("stroke-width", 1.2);
      d3.selectAll("#legend_" + d.actor_name.replace(".", ''))
        .attr("fill", function () {
          return d.visible ? d.color : "#F1F1F2";
        })
    })
  }

  function MouseOverLegend(d, i) {

    // if actor was not selected, just draw its line on hover then remove it 
    if (d.visible == false) {
      svg.append("path")
        .attr("class", "line")
        .style("fill", "none")
        .attr("id", 'Line_' + d.actor_name.replace(/\s+/g, '').replace(".", '')) // assign ID
        .style("stroke", d.color)
        .style("opacity", 1)
        .attr("d", line(d.values))
      // .transition()
    }
  }

  function MouseOutLegend(d, i) {
    if (d.visible == false) {
      d3.select("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .remove();
    }
  }

  function selectAll() {
    actors.forEach(function (d, i) {
      drawLines(d, i);
      actors[i].visible = true;
      d3.selectAll("#legend_" + d.actor_name.replace(".", ''))
        .attr("fill", d.color)
    })
    d3.select("#SelectLabel")
      .text("Clear All")
  }

  function clearAll() {
    actors.forEach(function (d, i) {
      d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .transition()
        .duration(50)
        .remove();
      d.visible = false;
      d3.selectAll("#legend_" + d.actor_name.replace(".", ''))
        .attr("fill", "#F1F1F2")
      bargnsData[i]["PointVisible"] = false;
      d3.selectAll(".dot" + d.actor_name.replace(/\s+/, "").replace(".", '')).remove();

    })
    d3.select("#SelectLabel")
      .text("Select All")
  }

  function drawLines(d, i) {

    //remove line if already exist (when hovering on a disabled actor's legend, his line is 
    // drwan but still d.visible = false so when legened is clicked and since d.visible = false another line will be drawn !) 
    d3.select("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
      .remove();

    svg.append("path")
      .attr("class", "line")
      .style("fill", "none")
      .attr("id", 'Line_' + d.actor_name.replace(/\s+/g, '').replace(".", '')) // assign ID
      .style("stroke", d.color)
      .style("opacity", 1)
      .attr("d", line(d.values))
      .attr("stroke-dasharray", function () {
        var totalLength = this.getTotalLength();
        return totalLength + " " + totalLength;
      })
      .attr("stroke-dashoffset", function () {
        var totalLength = this.getTotalLength();
        return totalLength;
      })
      .on("mouseover", function () {
        selectedLine = "#Line_" + d.actor_name;
        selectedLegend = "#legend_" + d.actor_name;
        onMouseover(d, i);
      })
      .on("mouseout", onMouseout)
      .transition("drawLines")
      .duration(2000)
      .ease(d3.easeLinear)
      .attr("stroke-dashoffset", 0);
  }
  // gridlines in x axis function
  function make_x_gridlines() {
    return d3.axisBottom(XScale)
      .ticks(turns)
  }

  // gridlines in y axis function
  function make_y_gridlines() {
    return d3.axisLeft(YScale)
      .ticks(10)
  }
  function roundTo(n, digits) {
    if (digits === undefined) {
      digits = 0;
    }

    var multiplicator = Math.pow(10, digits);
    n = parseFloat((n * multiplicator).toFixed(11));
    return Math.round(n) / multiplicator;
  }
}
