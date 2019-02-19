// data from load.js (session data)
// var ActorsNamesAllSce = JSON.parse(sessionStorage.getItem("ActorsNames"));
var ActorsPositions = JSON.parse(sessionStorage.getItem("ActorsPositions"));
var whyActorChanged = JSON.parse(sessionStorage.getItem("BargnsData"));
var NumOfTurns = sessionStorage.getItem("NumOfTurns");
var selectedDimNum = 0;
// var selectedScenNum = sessionStorage.getItem("selectedScen");
var defaultColors = sessionStorage.getItem("defaultColors");
var svgWidth2 = 550;
var svgheight2 = 300;

function getNewResoulution(w, h) {
  svgWidth2 = w;
  svgheight2 = h;
}

function drawLine() {
  var ActorsNamesAllSce = ActorsNames,
    selectedScenNum = selectedScen,
    ActorsPositions = arrPos,
    whyActorChanged = arrBargns;

  // Define margins        
  var margin = { top: 30, right: 20, bottom: 30, left: 50 },
    width = svgWidth2 - margin.left - margin.right,
    height = svgheight2 - margin.top - margin.bottom;

  // Clear the exiting chart
  d3.select("#MainLineChart").html("");
  d3.select("#legend").html("");

  var svg = d3.select("#MainLineChart")
    .append("svg")
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("viewBox", "0 0 " + " " + svgWidth2 + " " + svgheight2)
    .attr("preserveAspectRatio", "xMidYMid meet")
    .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")")

  var svg2 = d3.select("#legend")
    .append("svg")
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("preserveAspectRatio", "xMidYMid meet")
    .attr("viewBox", "0 0 250 650")
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

  var ActorsObj1 = JSON.parse(sessionStorage.getItem("ActorsObj"));

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

  //show points till chosen turn
  var bargnsDataByTurn = bargnsData.filter(function (obj) {
    return obj.turn <= turn;
  });

  //define the scales
  XScale = d3.scaleLinear().domain([0, turn]).range([0, width]);
  YScale = d3.scaleLinear().domain([0, 100]).range([height, 0]);

  // Define the axes
  xAxis = d3.axisBottom(XScale).scale(XScale).ticks(turn);
  yAxis = d3.axisLeft(YScale).scale(YScale);

  // Define the line
  line = d3.line()
    .x(function (d) { return XScale(d.Turn); })
    .y(function (d, i) { return YScale(d.val[i]); })
    .curve(d3.curveMonotoneX);


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
    .attr("class", "CharLabel")
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
    .attr("class", "CharLabel")
    .attr("transform", "rotate(-90)")
    .attr("y", 0 - margin.left)
    .attr("x", 0 - (height / 2))
    .attr("dy", "1em")
    .style("text-anchor", "middle")
    .text("Position");


  ActorsObj1.forEach(function (obj, index) {
    obj.values = x_axix_data.map(function (x_value) {
      return {
        Turn: x_value,
        val: positionsData[index].map(function (y_values) {
          return y_values
        })
      }
    })
  });
  // get initiator color for the point  
  for (i = 0; i < bargnsDataByTurn.length; i++) {
    var obj1 = ActorsObj1.findIndex(o => o.actor_name === bargnsDataByTurn[i].Initiator);
    bargnsDataByTurn[i]["color"] = ActorsObj1[obj1].color;
  }

  //initialize bargains points visibilaty status  
  bargnsDataByTurn.forEach(function (obj) { obj["PointVisible"] = false; });

  //to show details about each bargain
  var tooltip = d3.select('#MainLineChart')
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


  ActorsObj1.forEach(function (d, i) {
    //draw the lines
    drawLines(d, i);
    d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", '')).transition().duration(100)
      .style("stroke", function () { return d.visible ? d.color : "#F1F1F2"; })
      .style("opacity", function () { return d.visible ? 1 : 0; })

    //add the legend
    svg2.append("rect")
      .attr("width", 10)
      .attr("height", 10)
      .attr("id", 'legend_' + d.actor_name.replace(/\s+/g, '').replace(".", ''))
      .attr("transform", function () {
        xOff = (i % 3) * 85
        yOff = Math.floor(i / 3) * 20
        return "translate(" + xOff + "," + (yOff + 50) + ")"
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
          d3.selectAll("#Spoint_" + d.actor_name.replace(/\s+/, "").replace(".", ''))
            .style("display", 'none')
          ActorsObj1[i].visible = false;
          d.visible = false;
        }
        else if (d.visible == false) {
          drawLines(d, i);
          d3.selectAll("#Spoint_" + d.actor_name.replace(/\s+/, "").replace(".", ''))
            .style("display", 'inline')
          ActorsObj1[i].visible = true;
          d.visible = true;
        }
      })
      .on("mouseover", function () {
        selectedLine = "#Line_" + d.actor_name;
        selectedLegend = "#legend_" + d.actor_name;
        onMouseover(d, i);

      })
      .on("mouseout", function () {
        MouseOutLegend(d, i);
        onMouseout();
      });

    var startingPoints = svg.selectAll(".Spoint")
      .data(ActorsObj1)
      .enter()
    startingPoints.append("circle")
      .attr("class", "Spoint")
      .attr("id", function (d) {
        return "Spoint_" + d["actor_name"].replace(/\s+/, "").replace(".", '');
      })
      .attr("r", 3)
      .attr("cx", function (d) { return XScale(d['values'][0].Turn); })
      .attr("cy", function (d) { return YScale(d['values'][0]['val'][0]); })
      .style("fill", function (d) { return d['color'] })
      .style("display", 'inline')


    //add bargns' points
    var dot = svg.selectAll(".dot")
      .data(bargnsDataByTurn)
      .enter();
    svg2.append("text")
      .attr("transform", function () {
        xOff = (i % 3) * 85
        yOff = Math.floor(i / 3) * 20

        return "translate(" + (xOff + 15) + "," + (yOff + 59) + ")"
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
        var visible = bargnsDataByTurn[i]["PointVisible"] ? false : true;
        //show/hide Bargns' points
        if (bargnsDataByTurn[i]["PointVisible"] == false) {

          //select all points on the selected line 
          d3.selectAll(".dot" + d.actor_name.replace(/\s+/, "").replace(".", ''))
            .attr("fill", function (d) {
              bargnsDataByTurn[i]["PointVisible"] = d.PointVisible;
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
                .style('top', (yPosition) + 'px')
                .style('left', (xPosition) + 'px');

              // highlight the actor who initiated the bargain 
              d3.select("#legend_" + d["Initiator"].replace(/\s+/g, '').replace(".", ''))
                .style("stroke", "black")
                .style("stroke-width", "2.5");
            })
            .on("mouseout", function (d) {
              tooltip.style("display", "none");
              d3.select("#legend_" + d["Initiator"].replace(/\s+/g, '').replace(".", ''))
                .style("stroke", "none")
            });
          // Update whether or not points are visible
          bargnsDataByTurn[i]["PointVisible"] = visible;
        }
        else {
          bargnsDataByTurn[i]["PointVisible"] = visible;
          d3.selectAll(".dot" + d.actor_name.replace(/\s+/, "").replace(".", ''))
            .transition().remove();
        }
      });

  }) // end of forEach

  if (GroupsDetails.length > 0) {
    var Actorslegendsize = Math.floor((ActorsObj2.length - 1) / 3) * 20
    for (j = 0; j < GroupsDetails.length; j++) {
      //add the legend

      svg2.append("rect")
        .attr("width", 10)
        .attr("height", 10)
        .attr("class", 'GroupsLeg')
        .attr("transform", function () {
          xOff = (j % 3) * 85
          yOff = Math.floor((j) / 3) * 20
          return "translate(" + xOff + "," + (yOff + Actorslegendsize + 90) + ")"
        })
        .attr("id", 'legend_' + GroupsDetails[j].Gname.replace(/\s+/g, '').replace(".", ''))
        .attr("fill", GroupsDetails[j].Gcolor)

      svg2.append("text")
        .attr("transform", function () {
          //  xOff = ((ActorsObj1.length-1+i) % 3) * 85
          xOff = (j % 3) * 85
          yOff = Math.floor((j) / 3) * 20
          return "translate(" + (xOff + 15) + "," + (yOff + Actorslegendsize + 98) + ")"
        })
        .text(function () { return GroupsDetails[j].Gname })
    }
  }

  svg2.append("rect")
    .attr("width", 10)
    .attr("height", 10)
    .attr("y", 20)
    .attr("id", "select-All")
    .style("fill", "black")
    .on("click", function () {
      if (d3.select("#SelectLabel").text() == "Clear All") {
        bargnsDataByTurn.forEach(function (d, i) {
          d.visible = false;
          bargnsDataByTurn[i]["PointVisible"] = false;
        })
        clearAll();
      }
      else {
        ActorsObj1.forEach(function (d) {
          d.visible = true;
        })
        selectAll();
      }
    });

  svg2.append("text")
    .attr("x", 20)
    .attr("y", 28)
    .text("Clear All")
    .attr("id", "SelectLabel");

  function onMouseover(d, i) {
    MouseOverLegend(d, i);
    ActorsObj1.forEach(function (d, i) {
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

      d3.selectAll("#legend_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .attr("fill", function () {
          return ("#legend_" + d.actor_name === selectedLegend) ? d.color : "#F1F1F2"
        })
    })
  }

  function onMouseout() {
    ActorsObj1.forEach(function (d, i) {
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
      d3.selectAll("#legend_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
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
    ActorsObj1.forEach(function (d, i) {
      drawLines(d, i);
      ActorsObj1[i].visible = true;
      d3.selectAll("#legend_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .attr("fill", d.color)
      d3.selectAll("#Spoint_" + d.actor_name.replace(/\s+/, "").replace(".", ''))
        .style("display", 'inline')

    })
    d3.select("#SelectLabel")
      .text("Clear All")
  }

  function clearAll() {
    ActorsObj1.forEach(function (d, i) {
      d3.selectAll("#Line_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .transition()
        .duration(50)
        .remove();
      d.visible = false;
      d3.selectAll("#legend_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
        .attr("fill", "#F1F1F2")
      d3.selectAll(".dot" + d.actor_name.replace(/\s+/, "").replace(".", '')).remove();
      d3.selectAll("#Spoint_" + d.actor_name.replace(/\s+/, "").replace(".", ''))
        .style("display", 'none')

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
