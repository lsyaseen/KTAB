// var margin = { top: 0, right: 20, bottom: 30, left: 50 },
// width2 = 850 - margin.left - margin.right,
// height = 370 - 30 - margin.bottom;

// data from load.js (session data)
var ActorsPositions = JSON.parse(sessionStorage.getItem("ActorsPositions"));
var NumOfTurns = sessionStorage.getItem("NumOfTurns");
var EffectivePowArray = JSON.parse(sessionStorage.getItem("EffectivePowArray"));

var margin = { top: 30, right: 20, bottom: 30, left: 50 },
    width2 = 460 - margin.left - margin.right,
    height = 270 - margin.top - margin.bottom;

var svg3 = d3.select("#Barlegend")
    .append("svg")
    .attr("width", "100%")
    .attr("height", "100%")
    .attr("preserveAspectRatio", "xMidYMid meet")
    .attr("viewBox", "0 0 250 500")
    .attr('transform', "translate(" + 15 + "," + 15 + ")")
    .append("g")
    .attr("class", "legend2")
function drawChart() {

    // Clear the exiting chart
    d3.select("#MainBarChart").html("");
    // d3.select("#Barlegend").html(""); //no selection so no need to reload

    var svg = d3.select("#MainBarChart")
        .append("svg")
        .attr("width", "100%")
        .attr("width", "100%")
        .attr("preserveAspectRatio", "xMidYMid meet")
        .attr("viewBox", "0 0 500 300")
        .append("g")
        .attr("transform", "translate(" + (margin.left + 40) + "," + margin.top + ")")

    var xScale = d3.scaleBand()
        .rangeRound([0, width2])
        .paddingInner(0.05)
        .align(0.1);

    var yScale = d3.scaleLinear()
        .range([height, 0]);

    var z = d3.scaleOrdinal()
        // Set of Distinct Colors (Set #1)
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


    var PositionsArray = [],
        PositionsArray2 = [],
        roundedPositions = [],
        effpowArray = [],
        effpowArray2 = [],
        namesArray = [],
        highestRange = 0,
        rows,
        selectedRect,
        selectedLegend;


    var xAxis = d3.axisBottom(xScale).scale(xScale);
    var yAxis = d3.axisLeft(yScale).scale(yScale);
    var range1, range2, range3, range4, range5, range6, range7, range8, range9, range10; // initializing an array for each range of positions 

    turn = currentTurn; //current turn from slider

    for (var i = 0; i < EffectivePowArray.length; i++) {
        namesArray.push(EffectivePowArray[i][0]);
    }

    for (var i = 0; i < EffectivePowArray.length; i++) {
        effpowArray.push(EffectivePowArray[i][1]);
    }

    effpowArray2 = effpowArray.slice();

    for (var i = 0; i < effpowArray.length; i++) {
        var index = namesArray.indexOf(namesArray[i])
        if (index !== -1) {
            effpowArray2[index] = effpowArray[i];
        }
    }

    findHighestEffpow();

    for (var i = 0; i < ActorsPositions.length; i++) {
        PositionsArray.push(ActorsPositions[i][turn + 1]); // it should be a var based on which turn is chosen
    }
    //round positions to group by position range
    for (i = 0; i < PositionsArray.length; i++) {
        roundedPositions.push(Math.round(PositionsArray[i] / 10) * 10);
    }

    groupActors(roundedPositions);

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

    xScale.domain(result.map(function (d) { return d.Actor; }));

    // if (y_range == "fixed") {
    // 	yScale.domain([0, highestRange]).nice();

    // }
    // else {
    yScale.domain([0, d3.max(idheights)]).nice();
    // }
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
        // .ticks(turns)
    }

    // gridlines in y axis function
    function make_y_gridlines() {
        return d3.axisLeft(yScale)
            .ticks(10)
    }

    // add the X gridlines
    svg.append("g")
        .attr("transform", "translate(0," + height + ")")
        .style("stroke-opacity", "0.2")
        .style("stroke-dasharray", "2")
        .style("shape-rendering", "crispEdges")
        .call(make_x_gridlines()
            .tickSize(-height)
            .tickFormat("")
        );

    // add the Y gridlines
    svg.append("g")
        .attr("class", "grid")
        .style("stroke-opacity", "0.2")
        .style("stroke-dasharray", "2")
        .style("shape-rendering", "crispEdges")
        .call(make_y_gridlines()
            .tickSize(-width2)
            .tickFormat("")
        );

    svg.append("g")
        .selectAll("g")
        .data(newData)
        .enter().append("g")
        .attr("fill", function (d) { return d.color })
        .attr("id", function (d, i) { return 'Actor_' + d.Name.replace(/\s+/g, '') }) // assign ID)  //  
        .on("mouseover", function (d, i) {
            selectedRect = "#Actor_" + d.Name;
            selectedLegend = "#Blegend_" + d.Name;
            onMouseover();;
        })
        .on("mouseout", onMouseout)
        .selectAll("rect")
        .data(function (d) { return d.values; })
        .enter().append("rect")
        .attr("x", function (d, i) { return xScale((barnames[i])); })
        .attr("y", function (d) { return yScale(d[1]); })
        .attr("height", function (d) { return yScale(d[0]) - yScale(d[1]); })
        .attr("width", xScale.bandwidth())
        .on("mouseover", function () { tooltip.style("display", null); })
        .on("mouseout", function () { tooltip.style("display", "none"); })
        .on("mousemove", function (d) {
            var xPosition = d3.mouse(this)[0];
            var yPosition = d3.mouse(this)[1];
            tooltip.attr("transform", "translate(" + (xPosition) + "," + (yPosition) + ")");
            tooltip.select("text").text(" Effective Power: " + (d[1] - d[0]));
        });

    //draw the axis
    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis);

    // text label for the x axis
    svg.append("text")
        .attr("transform", "translate(" + (width2 / 2) + " ," + (height + 30) + ")")
        .style("text-anchor", "middle")
        .text("Position");

    svg.append("g")
        .attr("class", "y axis")
        .call(yAxis);

    // text label for the y axis
    svg.append("text")
        .attr("transform", "rotate(-90)")
        .attr("y", -30 - margin.left)
        .attr("x", 0 - (height / 2))
        .attr("dy", "1em")
        .style("text-anchor", "middle")
        .text("Effective Power");

    svg.append("text")
        .attr("transform", "rotate(-90)")
        .attr("y", 0 - margin.left)
        .attr("x", 0 - (height / 2))
        .attr("dy", "1em")
        .style("text-anchor", "middle")
        .text("Influence x Salience");

    var legend = svg3.append("g")
        .selectAll("g")
        .data(newData)
        .enter().append("g")
        .attr("transform", function (d, i) {
            var xOff = (i % 3) * 65
            var yOff = Math.floor(i / 3) * 20
            return "translate(" + xOff + "," + (yOff + 80) + ")"
        });

    legend.append("rect")
        // .attr("x", 40)
        .attr("width", 10)
        .attr("height", 10)
        .attr("id", function (d) { return 'Blegend_' + d.Name })
        .attr("fill", function (d) { return d.color; })
        .on("mouseover", function (d) {
            selectedRect = "#Actor_" + d.Name;
            selectedLegend = "#Blegend_" + d.Name;
            onMouseover();
        })
        .on("mouseout", onMouseout);


    legend.append("text")
        .attr("x", 15)
        .attr("y", 5)
        .attr("dy", "0.32em")
        .text(function (d) { return d.Name; });

    // Prep the tooltip bits, initial display is hidden
    var tooltip = svg.append("g")
        .attr("class", "tooltip")
        .style("display", "none")
        .style("opacity", 1);


    tooltip.append("text")
        .attr("x", 50)
        .attr("dy", "1.2em")
        .style("text-anchor", "middle")
        .attr("font-size", "12px")
        .attr("font-weight", "bold");

    function onMouseover() {

        newData.forEach(function (d, i) {
            d3.selectAll("#Actor_" + d.Name.replace(/\s+/g, ''))
                .transition()
                .duration(50)
                .style("opacity", function () {
                    return ("#Actor_" + d.Name === selectedRect) ? 1.0 : 0.2;
                })

            d3.selectAll("#Blegend_" + d.Name)
                .attr("fill", function () {
                    return ("#Blegend_" + d.Name === selectedLegend) ? d.color : "#F1F1F2"
                })
        })
    }

    function onMouseout() {

        newData.forEach(function (d, i) {
            d3.selectAll("#Actor_" + d.Name.replace(/\s+/g, ''))
                .transition()
                .duration(50)
                .style("opacity", 1);

            d3.selectAll("#Blegend_" + d.Name)
                .attr("fill", d.color)
        })
    }

    function findHighestEffpow() {

        for (var turnNo = 0; turnNo < turns; turnNo++) {
            // repeat for all turns to find the highest range and set it as y-axes max value
            for (var i = 0; i < ActorsPositions.length; i++) {
                PositionsArray2.push(ActorsPositions[i][turnNo + 1]);
            }
            roundPositions(PositionsArray2);
        }
    }
    //round positions to group by position range
    function roundPositions(y) {
        //keep PositionsArray for the specified turn and PositionsArray2 for all other turns
        for (i = 0; i < PositionsArray2.length; i++) {
            roundedPositions.push(Math.round(PositionsArray2[i] / 10) * 10);
        }
        groupActors(roundedPositions);
        //make sure arrays are empty for rounding another turn's positions
        PositionsArray2.splice(0, PositionsArray2.length);
        roundedPositions.splice(0, roundedPositions.length);
    }
    function groupActors(e) {
        // initializing an array for each range of positions 
        //filling it with zeroes cuz d3 stack layout is expecting arrays of the same length. 
        range1 = Array(26).fill(0);
        range2 = Array(26).fill(0);
        range3 = Array(26).fill(0);
        range4 = Array(26).fill(0);
        range5 = Array(26).fill(0);
        range6 = Array(26).fill(0);
        range7 = Array(26).fill(0);
        range8 = Array(26).fill(0);
        range9 = Array(26).fill(0);
        range10 = Array(26).fill(0);

        for (i = 0; i < roundedPositions.length; i++) {

            if (roundedPositions[i] == 10) {
                range1[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 20) {
                range2[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 30) {
                range3[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 40) {
                range4[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 50) {
                range5[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 60) {
                range6[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 70) {
                range7[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 80) {
                range8[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 90) {
                range9[i] = +effpowArray2[i];
            }
            else if (roundedPositions[i] == 100) {
                range10[i] = +effpowArray2[i];
            }
        }
        rows = [range1, range2, range3, range4, range5, range6, range7, range8, range9, range10];
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