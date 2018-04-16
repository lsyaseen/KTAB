// Define margins        

var margin = { top: 30, right: 20, bottom: 30, left: 50 },
  width = 500 - margin.left - margin.right,
  height = 270 - margin.top - margin.bottom;

var svg2 = d3.select("#Barchart")
  .append("svg")
  .attr("width", "100%")
  .attr("height", "100%")
  .attr("preserveAspectRatio", "xMidYMid meet")
  .attr("viewBox", "0 0 500 300")
  .append("g")
  .attr("transform", "translate(" + margin.left + "," + margin.top + ")");


var x = d3.scaleBand()
  .rangeRound([0, width])
  .paddingInner(0.05)
  .align(0.1);

var y = d3.scaleLinear()
  .range([height, 0]);

var z = d3.scaleOrdinal()
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

var xAxis = d3.axisBottom(x).scale(x);
var yAxis = d3.axisLeft(y).scale(y);

function drawBarchart() {
  var PositionsPerTurn = [],
    roundedPositions = [],
    effpowArray = [],
    namesArray = [],
    effpowData,
    turns;

  //from loadSQL.js
  var positionsData = positionsArray;
  turns = NumOfTurns;
  effpowData = EffectivePowArray;

  for (var i = 0; i < positionsData.length; i++) {
    PositionsPerTurn.push(positionsData[i][turns]); // it should be a var based on which turn is chosen
  }

  //round positions to group by position range
  for (i = 0; i < PositionsPerTurn.length; i++) {
    roundedPositions.push(Math.round(PositionsPerTurn[i] / 10) * 10);
  }

  for (var i = 0; i < effpowData.length; i++) {
    effpowArray.push(effpowData[i][1]);
  }

  for (var i = 0; i < effpowData.length; i++) {
    namesArray.push(effpowData[i][0]);
  }

  // initializing an array for each range of positions 
  var range1 = new Array(26).fill(0);
  var range2 = new Array(26).fill(0);
  var range3 = new Array(26).fill(0);
  var range4 = new Array(26).fill(0);
  var range5 = new Array(26).fill(0);
  var range6 = new Array(26).fill(0);
  var range7 = new Array(26).fill(0);
  var range8 = new Array(26).fill(0);
  var range9 = new Array(26).fill(0);
  var range10 = new Array(26).fill(0);

  //group positions
  for (i = 0; i < roundedPositions.length; i++) {

    if (roundedPositions[i] == 10) {
      range1[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 20) {
      range2[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 30) {
      range3[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 40) {
      range4[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 50) {
      range5[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 60) {
      range6[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 70) {
      range7[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 80) {
      range8[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 90) {
      range9[i] = effpowArray[i];
    }
    else if (roundedPositions[i] == 100) {
      range10[i] = effpowArray[i];
    }
  }

  //adding range position
  namesArray.unshift("Actor");
  range1.unshift(10);
  range2.unshift(20);
  range3.unshift(30);
  range4.unshift(40);
  range5.unshift(50);
  range6.unshift(60);
  range7.unshift(70);
  range8.unshift(80);
  range9.unshift(90);
  range10.unshift(100);


  var rows = [range1, range2, range3, range4, range5, range6, range7, range8, range9, range10];
  var result = rows.map(function (row) {
    return row.reduce(function (result, field, index) {
      result[namesArray[index]] = field;
      return result;
    }, {});
  });

  var keys = namesArray.slice(1);
  var idheights = [];
  var barnames = [];

  //to calculate bars' heights
  for (var i = 0; i < result.length; i++) {
    var tempvalues = d3.values(result[i]);
    tempvalues.shift();
    var tempsum = 0;
    for (var j = 0; j < tempvalues.length; j++) { tempsum = tempsum + tempvalues[j]; }
    idheights.push(tempsum);
    barnames.push(result[i].Actor);
  };

  x.domain(result.map(function (d) { return d.Actor; }));
  y.domain([0, d3.max(idheights)]).nice();
  z.domain(keys);

  // gridlines in x axis function
  function make_x_gridlines() {
    return d3.axisBottom(x)
    // .ticks(turns)
  }

  // gridlines in y axis function
  function make_y_gridlines() {
    return d3.axisLeft(y)
      .ticks(10)
  }

  // add the X gridlines
  svg2.append("g")
    .attr("class", "grid")
    .attr("transform", "translate(0," + height + ")")
    .call(make_x_gridlines()
      .tickSize(-height)
      .tickFormat("")
    )

  // add the Y gridlines
  svg2.append("g")
    .attr("class", "grid")
    .call(make_y_gridlines()
      .tickSize(-width)
      .tickFormat("")
    )

  svg2.append("g")
    .selectAll("g")
    .data(d3.stack().keys(keys)(result))
    .enter().append("g")
    .attr("fill", function (d) { return z(d.key); })
    .selectAll("rect")
    .data(function (d) { return d; })
    .enter().append("rect")
    .attr("x", function (d, i) { return x((barnames[i])); })
    .attr("y", height)
    .attr("width", x.bandwidth())
    .attr("height", 0)
    .transition()
    .duration(3000)
    .delay(function (d, i) { return i * 400 })
    .attr("y", function (d) { return y(d[1]); })
    .attr("height", function (d) { return y(d[0]) - y(d[1]); })

  //draw the axis
  svg2.append("g")
    .attr("class", "x axis")
    .attr("transform", "translate(0," + height + ")")
    .call(xAxis);

  // text label for the x axis
  svg2.append("text")
    .attr("transform", "translate(" + (width / 2) + " ," + (height + 30) + ")")
    .style("text-anchor", "middle")
    .text("Position");

  svg2.append("g")
    .attr("class", "y axis")
    .call(yAxis);

  // text label for the y axis
  svg2.append("text")
    .attr("transform", "rotate(-90)")
    .attr("y", 0 - margin.left)
    .attr("x", 0 - (height / 2))
    .attr("dy", "1em")
    .style("text-anchor", "middle")
    .text("Effective Power");

}