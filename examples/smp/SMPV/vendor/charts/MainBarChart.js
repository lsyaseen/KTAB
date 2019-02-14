
// data from load.js (session data)
// var allpos = JSON.parse(sessionStorage.getItem("ActorsPositions"));
// var AllEffcPow = JSON.parse(sessionStorage.getItem("AllEffcPow"));
var NumOfTurns = sessionStorage.getItem("NumOfTurns");
var effpow = JSON.parse(sessionStorage.getItem("effpow"));
var ActorsObj2 = JSON.parse(sessionStorage.getItem("ActorsObj"));
var selectedDimNum = 0;
var svgWidth2 = 550;
var svgheight2 = 300;
var NoOfBars = 25; //default
var axisLables = true;
var currentTurn;

function getNewResoulution(w, h, Bn,lables) {
    svgWidth2 = w;
    svgheight2 = h;
    NoOfBars = Bn;
    axisLables= lables;
}
// function getTurnData(turn){

//     currentTurn= turn;
// } 

function drawChart() {
    
    allpos = arrPos;
    AllEffcPow =arreff;
 
    var margin = { top: 30, right: 20, bottom: 30, left: 50 },
        width2 = svgWidth2 - margin.left - margin.right,
        height = svgheight2 - margin.top - margin.bottom;

    // Clear the exiting chart
    d3.select("#MainBarChart").html("");
    d3.select("#Barlegend").html("");

    var svg = d3.select("#MainBarChart")
        .append("svg")
        .attr("width", "100%")
        .attr("height", "100%")
        .attr("viewBox", "0 0 " + " " + svgWidth2 + " " + svgheight2)
        .attr("preserveAspectRatio", "xMidYMid meet")
        .append("g")
        .attr("transform", "translate(" + (margin.left) + "," + margin.top + ")");

    var svg3 = d3.select("#Barlegend")
        .append("svg")
        .attr("width", "100%")
        .attr("height", "100%")
        .attr("preserveAspectRatio", "xMidYMid meet")
        .attr("viewBox", "0 0 250 500")
        .attr('transform', "translate(" + 15 + "," + 15 + ")")
        .append("g")
        .attr("class", "legend2");

    var xScale = d3.scaleLinear()
        .range([0, width2]);
    var yScale = d3.scaleLinear()
        .range([height, 0]);

    var rangearray = [];  //based on the no of bars
    for (i = 0; i < NoOfBars; i++) {
        rangearray.push(i);
    }
    var Qscale = d3.scaleQuantize().domain([0, 100]).range(rangearray);

    var PositionsArray = [],
        PositionsArray2 = [],
        positionsData = [],
        effpowArray = [],
        effpowArray2 = [],
        namesArray = [],
        highestRange = 0,
        rows = [],
        selectedRect,
        selectedLegend;
var selectedScenNum = selectedScen;
    ActorsObj2 = JSON.parse(sessionStorage.getItem("ActorsObj"));

    var xAxis = d3.axisBottom(xScale).scale(xScale);
    var yAxis = d3.axisLeft(yScale).scale(yScale);
    turn = currentTurn; //current turn from slider

   
    for (var i = 0; i < allpos[selectedScenNum][selectedDimNum].length; i += 1) {
        positionsData.push(allpos[selectedScenNum][selectedDimNum][i].positions);
    }

    for (i = 0; i < AllEffcPow[selectedScenNum][selectedDimNum].length; i++) {
        namesArray.push(AllEffcPow[selectedScenNum][selectedDimNum][0][i].Name)//sec [] for dim
    }

    for (i = 0; i < AllEffcPow[selectedScenNum][selectedDimNum].length; i++) {
        effpowArray.push(AllEffcPow[selectedScenNum][selectedDimNum][0][i].fpower)//sec [] for dim
    }

    effpowArray2 = effpowArray.slice();

    for (var i = 0; i < effpowArray.length; i++) {
        var index = namesArray.indexOf(namesArray[i])
        if (index !== -1) {
            effpowArray2[index] = effpowArray[i];
        }
    }
    findHighestEffpow();

    for (var i = 0; i < positionsData.length; i++) {
        PositionsArray.push(positionsData[i][turn]); // it should be a var based on which turn is chosen
    }
  
    groupActors(PositionsArray);

    //adding range position
    namesArray.unshift("Actor");

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

    if (document.getElementById('Fixedbtn').checked) {
        y_range = "fixed"
        yScale.domain([0, highestRange]).nice();
    }

    else if (document.getElementById('Responsivebtn').checked) {
        yScale.domain([0, d3.max(idheights)]).nice();
        y_range = "responsive"
    }

    var stack = d3.stack().keys(keys)(result);
    ActorsObj2.forEach(function (obj, i) {
        obj.values = stack[i]
    });

    var fKeys = keys.slice();
    var fKeyReference = fKeys.map(function (key) {
        return true; //used to indicate if the corresponding key is active
    });

    function getActiveKeys(reference) {
        return reference.map(function (state, index) {
            if (state) {
                return keys[index]; //just keep keys whoes state is true
            }
            return false; //return false to be filtered
        }).filter(function (name) {
            return name
        });
    }

    // gridlines in x axis function
    function make_x_gridlines() {
        return d3.axisBottom(xScale)
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

    DrawBars(ActorsObj2);
    //draw the axis
    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis);

    // text label for the x axis
    svg.append("text")
        .attr("class", "CharLabel")//used it to view and hide lablels when downloading
        .attr("transform", "translate(" + (width2 / 2) + " ," + (height + 30) + ")")
        .style("text-anchor", "middle")
        .text("Position");

    svg.append("g")
        .attr("class", "y axis")
        .call(yAxis);


    svg.append("text")
        .attr("class", "CharLabel")
        .attr("transform", "rotate(-90)")
        .attr("y", 0 - margin.left)
        .attr("x", 0 - (height / 2))
        .attr("dy", "1em")
        .style("text-anchor", "middle")
        .text("Effective Power (Influence x Salience)");

    var legend = svg3.append("g")
        .selectAll("g")
        .data(ActorsObj2)
        .enter().append("g")
        .attr("transform", function (d, i) {
            var xOff = (i % 3) * 85
            var yOff = Math.floor(i / 3) * 20
            return "translate(" + xOff + "," + (yOff + 30) + ")"
        });

    legend.append("rect")
        .attr("width", 10)
        .attr("height", 10)
        .attr("id", function (d) { return 'Blegend_' + d.actor_name.replace(/\s+/g, '').replace(".", '') })
        .attr("fill", function (d) { return d.color; })
        .on("click", function (d, i) {

            if (fKeys.length === 1 && fKeys[0] === d) {
                return;
            }
            fKeyReference[i] = !fKeyReference[i]; // toggle state of fKeyReference
            fKeys = getActiveKeys(fKeyReference);

            if (d.visible == true) {
                d.visible = false;
            }
            else if (d.visible == false) {
                d.visible = true;
            }
            d3.select(this).attr("fill", function () {
                return d.visible ? d.color : "#F1F1F2";
            })
            RedrawonLegendClick(fKeys);
        })
        .on("mouseover", function (d) {
            selectedRect = "#Actor_" + d.actor_name;
            selectedLegend = "#Blegend_" + d.actor_name;
            onMouseover();
        })
        .on("mouseout", onMouseout);

    legend.append("text")
        .attr("x", 15)
        .attr("y", 5)
        .attr("dy", "0.32em")
        .text(function (d) { return d.actor_name; });

    // Prep the tooltip bits, initial display is hidden
    var tooltip = svg.append("g")
        .attr("class", "tooltip")
        .style("display", "none")
        .style("opacity", 1);

    tooltip.append("text")
        .attr("x", 50)
        .attr("dy", "1.2em")
        .style("text-anchor", "middle")
        .attr("font-size", "10px")
        .attr("font-weight", "bold");

        if (axisLables == true){
    $(".CharLabel").show();
    $(".axis text").show();
}
else if (axisLables == false){
    $(".CharLabel").hide();
    $(".axis text").hide();
}
    function onMouseover() {

        ActorsObj2.forEach(function (d, i) {
            d3.selectAll("#Actor_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
                .transition()
                .duration(50)
                .style("opacity", function () {
                    return ("#Actor_" + d.actor_name === selectedRect) ? 1.0 : 0.2;
                })

            d3.selectAll("#Blegend_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
                .attr("fill", function () {
                    return ("#Blegend_" + d.actor_name === selectedLegend) ? d.color : "#F1F1F2"
                })
        })
    }

    function RedrawonLegendClick(fkeys) {
        var activeActorsObj = []; //to use index for getting colors
        stack = d3.stack().keys(fkeys)(result);
        var updatedData = fkeys.map(function (name, i) {
            var obj1 = ActorsObj2.findIndex(o => o.actor_name === name);
            activeActorsObj.push(ActorsObj2[obj1].color);
            return {
                actor_name: name,
                visible: true,
                values: stack[i],
                color: activeActorsObj[i]
            };
        })
        d3.selectAll(".bars") //remove all bars
            .remove();
        DrawBars(updatedData);
        //redraw after update
    }

    function onMouseout() {

        ActorsObj2.forEach(function (d, i) {
            d3.selectAll("#Actor_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
                .transition()
                .duration(50)
                .style("opacity", 1);

            d3.selectAll("#Blegend_" + d.actor_name.replace(/\s+/g, '').replace(".", ''))
                .attr("fill", function () {
                    return d.visible ? d.color : "#F1F1F2";
                })
        })
    }

    function findHighestEffpow() {

        for (var turnNo = 0; turnNo < NumOfTurns - 1; turnNo++) {
            // repeat for all turns to find the highest range and set it as y-axes max value
            for (var i = 0; i < positionsData.length; i++) {
                PositionsArray2.push(positionsData[i][turnNo + 1]);
            }
            roundPositions(PositionsArray2);
        }
    }
    if (GroupsDetails.length>0){

        for (j =0 ; j < GroupsDetails.length;j++){
               //add the legend
               svg3.append("rect")
         .attr("width", 10)
         .attr("height", 10)
               .attr("class",'GroupsLeg')
               .attr("transform", function (d, i) {
                var xOff = ((ActorsObj2.length-1+j) % 3) * 85
                var yOff = Math.floor((ActorsObj2.length-1+j) / 3) * 20
                return "translate(" + xOff + "," + (yOff + 70) + ")"
            })
          .attr("id", 'Barlegend_' + GroupsDetails[j].Gname.replace(/\s+/g, '').replace(".", ''))
         .attr("fill",  GroupsDetails[j].Gcolor )
        
         svg3.append("text")
         .attr("transform", function (d, i) {
            var xOff = ((ActorsObj2.length-1+j) % 3) * 85
            var yOff = Math.floor((ActorsObj2.length-1+j) / 3) * 20
            return "translate(" + (xOff+15) + "," + (yOff + 78) + ")"
        })
         .text(function () { return GroupsDetails[j].Gname })    
        
        }
          }
            
    function DrawBars(data) {
        svg.append("g")
            .selectAll("g")
            .data(data)
            .enter().append("g")
            .attr("fill", function (d) {
                return d.color
            })
            .attr("class", "bars")
            .attr("id", function (d, i) {
                return 'Actor_' + d.actor_name.replace(/\s+/g, '').replace(".", '')
            }) // assign ID)  
            .on("mouseover", function (d, i) {
                selectedRect = "#Actor_" + d.actor_name;
                selectedLegend = "#Blegend_" + d.actor_name;
                onMouseover();
            })
            .on("mouseout", onMouseout)
            .selectAll("rect")
            .data(function (d) { 
                return d.values; })
            .enter().append("rect")
            .attr("x", function (d, i) {
                return xScale((barnames[i])) + 2.5;
            }) // + to shift bars 
            .attr("width", function (d) {
                var barWidth = width2 / (data[0].values.length)- 4;
                return barWidth
            })
            .attr("y", height)
            .attr("height", 0)
            .on("mouseover", function () { tooltip.style("display", null); })
            .on("mouseout", function () { tooltip.style("display", "none"); })
            .on("mousemove", function (d) {
                var xPosition = d3.mouse(this)[0];
                var yPosition = d3.mouse(this)[1];
                tooltip.attr("transform", "translate(" + (xPosition - 50) + "," + (yPosition + 10) + ")");
                tooltip.select("text").text(" Effective Power: " + (d[1] - d[0]));
            })
            .transition()
            .duration(3000)
            .attr("y", function (d) { return yScale(d[1]); })
            .attr("height", function (d) { return yScale(d[0]) - yScale(d[1]); });
//groupLegend
    // legend.append("rect")
    //         .attr("width", 10)
    //         .attr("height", 10)
    //         .attr("id", function (d) { return 'Blegend_' + d.actor_name.replace(/\s+/g, '').replace(".", '') })
    //         .attr("fill", function (d) { return d.color; })
            
        // legend.append("text")
        //     .attr("x", 15)
        //     .attr("y", 5)
        //     .attr("dy", "0.32em")
        //     .text(function (d) { return d.actor_name; });
          
        }
    function roundPositions(y) {
      
        //keep PositionsArray for the specified turn and PositionsArray2 for all other turns
        groupActors(PositionsArray2);
        //make sure arrays are empty for rounding another turn's positions
        PositionsArray2.splice(0, PositionsArray2.length);
    }

    function groupActors(e) {

        var NoOfActors = e.length;

        for (i = 0; i <= NoOfBars - 1; i++) {
            rows[i] = Array(NoOfActors).fill(0);

        }
        
        for (i = 0; i < NoOfActors; i++) {

            var temp = Qscale(e[i]);
            rows[temp][i] = +effpowArray2[i];
           
        }
        for (i = 0; i <= NoOfBars - 1; i++) {
            var temp2 = Qscale.invertExtent(i)[0];
            rows[i].unshift(temp2);
        }

        // sum all ranges and find the highest		
        for (var i = 0; i < NoOfBars; i++) {
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