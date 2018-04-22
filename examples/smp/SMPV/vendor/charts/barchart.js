
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

var xScale = d3.scaleLinear()
  .range([0, width]);
var yScale = d3.scaleLinear()
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

function drawBarchart() {

  var xAxis = d3.axisBottom(xScale).scale(xScale);
  var yAxis = d3.axisLeft(yScale).scale(yScale);

  var PositionsPerTurn = [],
    effpowArray = [],
    namesArray = [],
    namesArray2 = [],
    effpowData,
    highestRange = 0,
    alleffpow = [],
    turns;

  var positionsData = positionsArray;
  turns = NumOfTurns;
  alleffpow = effpow;

  var range0, range1, range2, range3, range4, range5, range6, range7, range8, range9; // initializing an array for each range of positions 

  //show for 1st dim
  var namesArray = alleffpow[0].map(function (a) { return a.Name; });
  var effpowArray = alleffpow[0].map(function (a) { return a.fpower; });

  effpowArray2 = effpowArray.slice();

  for (var i = 0; i < effpowArray.length; i++) {
    var index = namesArray.indexOf(namesArray[i])
    if (index !== -1) {
      effpowArray2[index] = effpowArray[i];
    }
  }

  for (var i = 0; i < positionsData.length; i++) {
    PositionsPerTurn.push(positionsData[i][turns]); // it should be a var based on which turn is chosen
  }

  groupActors(PositionsPerTurn);

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
  range0.unshift(100);

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
    for (var j = 0; j < tempvalues.length; j++) { tempsum = tempsum + parseFloat(tempvalues[j]); }
    idheights.push(tempsum);
    barnames.push(result[i].Actor);
  };

  xScale.domain([0, 100]);
  yScale.domain([0, d3.max(idheights)]).nice();
  z.domain(keys);

  var stack = d3.stack().keys(keys)(result);
  var newData = namesArray.slice(1).map(function (name, i) {
    return {
      Name: name,
      values: stack[i],
      color: z(name)
    };
  })

  // gridlines in x axis function
  function make_x_gridlines() {
    return d3.axisBottom(xScale)
      .ticks(10)
  }

  // gridlines in y axis function
  function make_y_gridlines() {
    return d3.axisLeft(yScale)
      .ticks(10)
  }

  // add the X gridlines
  svg2.append("g")
    .attr("transform", "translate(0," + height + ")")
    .style("stroke-opacity", "0.2")
    .style("stroke-dasharray", "2")
    .style("shape-rendering", "crispEdges")
    .call(make_x_gridlines()
      .tickSize(-height)
      .tickFormat("")
    );

  // add the Y gridlines
  svg2.append("g")
    .attr("class", "grid")
    .style("stroke-opacity", "0.2")
    .style("stroke-dasharray", "2")
    .style("shape-rendering", "crispEdges")
    .call(make_y_gridlines()
      .tickSize(-width)
      .tickFormat("")
    );

  svg2.append("g")
    .selectAll("g")
    .data(newData)
    .enter().append("g")
    .attr("fill", function (d) { return d.color })
    .attr("id", function (d, i) { return 'Actor_' + d.Name.replace(/\s+/g, '').replace(".", '') }) // assign ID)  
    .selectAll("rect")
    .data(function (d) { return d.values; })
    .enter().append("rect")
    .attr("x", function (d, i) { return xScale((barnames[i])) - (width / newData[0].values.length) + 2; }) //to add space
    .attr("width", function (d) {
      var barWidth = width / (newData[0].values.length + 1);
      return barWidth
    })
    .attr("y", height)
    .attr("height", 0)
    .transition()
    .duration(3000)
        .attr("y", function (d) { return yScale(d[1]); })
    .attr("height", function (d) { return yScale(d[0]) - yScale(d[1]); });

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
    .attr("y", -30 - margin.left)
    .attr("x", 0 - (height / 2))
    .attr("dy", "1em")
    .style("text-anchor", "middle")
    .text("Effective Power");

  svg2.append("text")
    .attr("transform", "rotate(-90)")
    .attr("y", 0 - margin.left)
    .attr("x", 0 - (height / 2))
    .attr("dy", "1em")
    .style("text-anchor", "middle")
    .text("Influence x Salience");


  function groupActors(e) {
    // initializing an array for each range of positions 
    //filling it with zeroes cuz d3 stack layout is expecting arrays of the same length. 
    range0 = Array(26).fill(0);
    range1 = Array(26).fill(0);
    range2 = Array(26).fill(0);
    range3 = Array(26).fill(0);
    range4 = Array(26).fill(0);
    range5 = Array(26).fill(0);
    range6 = Array(26).fill(0);
    range7 = Array(26).fill(0);
    range8 = Array(26).fill(0);
    range9 = Array(26).fill(0);

    for (i = 0; i < e.length; i++) {

      if (e[i] >= 0 && e[i] < 10) {
        range0[i] = +effpowArray2[i];
      }
      else if (e[i] >= 10 && e[i] < 20) {
        range1[i] = +effpowArray2[i];
      }
      else if (e[i] >= 20 && e[i] < 30) {
        range2[i] = +effpowArray2[i];
      }
      else if (e[i] >= 30 && e[i] < 40) {
        range3[i] = +effpowArray2[i];
      }
      else if (e[i] >= 40 && e[i] < 50) {
        range4[i] = +effpowArray2[i];
      }
      else if (e[i] >= 50 && e[i] < 60) {
        range5[i] = +effpowArray2[i];
      }
      else if (e[i] >= 60 && e[i] < 70) {
        range6[i] = +effpowArray2[i];
      }
      else if (e[i] >= 70 && e[i] < 80) {
        range7[i] = +effpowArray2[i];
      }
      else if (e[i] >= 80 && e[i] < 90) {
        range8[i] = +effpowArray2[i];
      }
      else if (e[i] >= 90 && e[i] < 100) {
        range9[i] = +effpowArray2[i];
      }
    }
    rows = [range0, range1, range2, range3, range4, range5, range6, range7, range8, range9];

    // sum all ranges and find the highest		
    for (var i = 0; i < 10; i++) {
      var sum = (rows[i]).reduce(add, 0);
      var temp = sum;
      if (highestRange < temp)
        highestRange = temp;
    }
    function add(a, b) {
      return a + b;
    }
  }
}