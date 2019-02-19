
var svgWidth2 = 900;
var svgheight2 = 300;
function drawNetwork() {
    this.actorsById = {};
    this.linksByBid = {};
    var __this = this;
    var actorsData = NetworkactorsData;
    var ActorsObj = JSON.parse(sessionStorage.getItem("ActorsObj"));
    var turn = currentTurn;
    this.init = function () {

        initActorData();
        loadTurnData(turn);
    }


    this.init();

    function clearBargainNodes() {

        var clone = {};
        for (var key in __this.actorsById) {
            if (__this.actorsById.hasOwnProperty(key)) {
                if (__this.actorsById[key].type == 'actor') {
                    clone[key] = __this.actorsById[key];
                }
            }
        }
        __this.actorsById = clone;
    }

    function loadTurnData() {

        __this.linksByBid = {};
        clearBargainNodes();
        networkData[0].values.forEach(link => {
            var from = link[networkData[0].columns.indexOf('Init_Act_i')];
            var to = link[networkData[0].columns.indexOf('Recd_Act_j')];
            var bgn = link[2];
            var id1 = from + " - " + bgn;
            var id2 = bgn + " - " + to;
            var toself = (from == to);
            if (!toself) {
                __this.linksByBid[id1] = {
                    source: from,
                    // acceptanceprop: link[networkData[0].columns.indexOf('Init_Prob')],
                    target: bgn,
                    strength: 2.0,
                    dist: 50,
                    type: "tob",
                    toself: toself,
                    accepted: false
                };
                __this.linksByBid[id2] = {
                    source: bgn,
                    // acceptanceprop: link[networkData[0].columns.indexOf('Recd_Prob')],
                    target: to,
                    strength: 2.0,
                    dist: 50,
                    type: "fromb",
                    toself: toself,
                    accepted: false
                };
                __this.actorsById[bgn] = {
                    id: bgn,
                    label: '',
                    type: 'bargain'
                };
            }
        });
        acceptedBgn[0].values.forEach(bargn => {
            var from = bargn[acceptedBgn[0].columns.indexOf('Init_Act_i')];
            var to = bargn[acceptedBgn[0].columns.indexOf('Recd_Act_j')];
            var bgn = bargn[acceptedBgn[0].columns.indexOf('BargnId')];
            var q = bargn[acceptedBgn[0].columns.indexOf('Q')];
            var id1 = from + " - " + bgn;
            var id2 = bgn + " - " + to;
            if (__this.linksByBid[id1]) {
                __this.linksByBid[id1].accepted = __this.linksByBid[id2].toself || __this.linksByBid[id1].accepted || (q == 'Init' ? true : false);
            }
            if (__this.linksByBid[id2]) {
                __this.linksByBid[id2].accepted = __this.linksByBid[id2].toself || __this.linksByBid[id2].accepted || (q == 'Rcvr' ? true : false);
            }
        });

        loadEfPower();
        loadPosition();

        if (__this.gren) {
            __this.gren.destroy();
        }
        __this.gren = new GraphRenderer(getActors(), getLinks());

        if (++__this.turn <= __this.turns) {
            setTimeout(() => {
            }, 5000);
        }
    }

    function loadEfPower() {

        var effPowerData = arreff[selectedScen][selectedDimNum][0];

        effPowerData.forEach(bargn => {
            var aid = bargn.Act_i;
            var fp = bargn.fpower;
            __this.actorsById[aid].fpower = fp;
        });

    }

    function loadPosition() {

        var actorsPosData = arrPos[selectedScen][selectedDimNum];
        actorsPosData.forEach(bargn => {
            var aid = bargn.aid;
            var pos = bargn.positions[currentTurn];
            __this.actorsById[aid].position = pos;
        });

    }

    function initActorData() {
        actorsData[0].values.forEach((actor, index) => {

            var aid = actor[actorsData[0].columns.indexOf('id')];
            __this.actorsById[aid] = {
                id: aid,
                label: ActorsObj[index].actor_name,
                color: ActorsObj[index].color,
                type: 'actor'
            };
        });

    }

    function getActors() {
        var ret = [];
        for (var key in __this.actorsById) {
            if (__this.actorsById.hasOwnProperty(key)) {
                ret.push(__this.actorsById[key]);
            }
        }
        return ret;
    }

    function getLinks() {

        var ret = [];
        for (var key in __this.linksByBid) {
            if (__this.linksByBid.hasOwnProperty(key)) {
                ret.push(__this.linksByBid[key]);
            }
        }
        return ret;
    }
}
function getNewResoulution(w, h) {
    svgWidth2 = w;
    svgheight2 = h;

}
function GraphRenderer(actors, links) {

    var __this = this;
    this.nodes = actors;
    this.links = links;
    this.destroy = function () {
        d3.select("#MainNetwork").html("");
    };
    this.init = function () {

        var margin = { top: 30, right: 20, bottom: 10, left: 20 },
            width = svgWidth2 - margin.left - margin.right,
            height = svgheight2 - margin.top - margin.bottom;

        var svg = d3.select("#MainNetwork")
            .append("svg")
            .attr("width", "100%")
            .attr("height", "100%")
            .attr("viewBox", "0 0 " + " " + svgWidth2 + " " + svgheight2)
            .attr("preserveAspectRatio", "xMidYMid meet")

        var marker = svg.append('defs').selectAll("marker")
            .data(['fromb', 'tob'])
            .enter().append("marker")
            .attr("id", function (d) { return d; })
            .attr("viewBox", '-0 -5 10 10')
            .attr("refX", function (d) { return d === 'fromb' ? 5.6 : 5.6; })
            .attr("refY", 2.5)//pos
            .attr("orient", "auto")
            .attr('markerUnits', 'userSpaceOnUse')
            .attr("markerWidth", function (d) { return d === 'fromb' ? 8 : 8; })
            .attr("markerHeight", function (d) { return d === 'fromb' ? 8 : 8; })
            .attr("xoverflow", 'visible')
            .append('svg:path')
            .attr('d', 'M0,0 L6.5,2.5 L0,5')
            .attr('fill', 'gray')

        // simulation setup with all forces
        var linkForce = d3
            .forceLink()
            .id(function (link) { return link.id })
            .distance(function (d) {
                return d.dist ? d.dist : 50;
            })
            .strength(function (link) { return link.strength });

        var dragDrop = d3.drag().on('start', function (node) {
            node.fx = node.x
            node.fy = node.y
        }).on('drag', function (node) {
            __this.simulation.alphaTarget(0.7).restart()
            node.fx = d3.event.x
            node.fy = d3.event.y
        }).on('end', function (node) {
            if (!d3.event.active) {
                __this.simulation.alphaTarget(0)
            }
            node.fx = null
            node.fy = null
        })
        var edgeWidthScale = d3.scaleLinear().domain([0, 1]).range([2, 10]);

        __this.simulation = d3
            .forceSimulation()
            .force('link', linkForce)
            .force('charge', d3.forceManyBody().strength(-120))
            .force('center', d3.forceCenter(width / 2, height / 2));
        var linkElements = svg.append("g")
            .attr("class", "links")
            .selectAll("path")
            .data(__this.links)
            .enter().append("path")
            .attr("class", function (d) {
                return "link " + d.type + " " + (d.accepted ? "" : "rejectb");
            })
            .attr("stroke-width", "1.5px")
            .attr("stroke", "rgba(50, 50, 50, 0.2)")
            .attr('marker-end', function (d) { return d.type == 'fromb' ? "url(#" + d.type + ")" : "" })

        var nodeElements = svg.append("g")
            .attr("class", "nodes")
            .selectAll("circle")
            .data(__this.nodes)
            .enter().append("circle")
            .attr("r", __this.getNodeRadius)
            .attr("class", function (n) { return (n.type == 'actor') ? "actor" : "bargain" })
            .attr("fill", __this.getNodeColor)
            .call(dragDrop)

        var textElements = svg.append("g")
            .attr("class", "texts")
            .selectAll("text")
            .data(__this.nodes)
            .enter().append("text")
            .attr("class", "CharLabel")
            .text(function (node) { return node.label })
            .attr("font-size", 10)
            .attr("dx", 15)
            .attr("dy", 4)
        __this.simulation.nodes(__this.nodes).on('tick', () => {
            nodeElements
                .attr('cx', function (node) { return node.x = Math.max(node.radius, Math.min(width - node.radius, node.x)); }) //cuz we have different radius nodes
                .attr('cy', function (node) { return node.y = Math.max(node.radius, Math.min(height - node.radius, node.y)); })
            textElements
                .attr('x', function (node) { return node.x })
                .attr('y', function (node) { return node.y })
            linkElements
                .attr("d", __this.linkArc);

        })
        __this.simulation.force("link").links(__this.links)

    }

    this.getNodeColor = function (node) {
        return node.type == 'actor' ? node.color : 'gray'
    }

    this.getNodeRadius = function (node) {
        if (node.type === 'actor') {
            node.radius = (node.fpower / 2) + ((Math.random() * 100) % 10);
        } else {
            node.radius = 3;
        }
        return node.radius;
    }

    this.linkArc = function (d) {
        var dx = d.target.x - d.source.x,
            dy = d.target.y - d.source.y,
            dr = Math.sqrt(dx * dx + dy * dy);
        if (!d.toself) {
            return "M" + d.source.x + "," + d.source.y + "L" + d.target.x + "," + d.target.y;
        }
        return "M" + d.source.x + "," + d.source.y + "A" + dr + "," + dr + " 0 0,1 " + d.target.x + "," + d.target.y;
    }//why twice?

    this.linkArc = function (d) {
        var dx = d.target.x - d.source.x,
            dy = d.target.y - d.source.y,
            dr = Math.sqrt(dx * dx + dy * dy);
        diffX = d.target.x - d.source.x;
        diffY = d.target.y - d.source.y;

        // Length of path from center of source node to center of target node
        pathLength = Math.sqrt((diffX * diffX) + (diffY * diffY));

        // x and y distances from center to outside edge of target node
        offsetX = (diffX * d.target.radius) / pathLength;
        offsetY = (diffY * d.target.radius) / pathLength;

        if (!d.toself) {
            return "M" + d.source.x + "," + d.source.y + "L" + (d.target.x - offsetX) + "," + (d.target.y - offsetY);
        }
        return "M" + (d.source.x) + "," + (d.source.y) + "A" + (dr) + "," + (dr) + " 0 0,1 " + (d.target.x - offsetX) + "," + (d.target.y - offsetY);
    }


    this.transform = function (d) {
        return "translate(" + d.x + "," + d.y + ")";
    }

    this.init();
}
