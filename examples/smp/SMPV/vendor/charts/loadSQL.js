var positionalData = [],
    ActorsNames = [],
    positionsArray = [],
    effpowArray,
    ScenarioArray = [],
    EffectivePowArray,
    SceAraay = [],
    arrPos = [],
    arreff = [],
    effpow = {},
    selectedScen = 0,
    NumOfTurns;

function getfile() {
    var files = document.getElementById("uploadInput").files;
    var file = files[0];
    var fr = new FileReader();
    fr.onload = function () {
        var Uints = new Uint8Array(fr.result);
        var db = new window.SQL.Database(Uints);

        var EffPowerData = db.exec("select distinct d.scenarioid, c.Name , d.Cap * b.Sal as fpower, b.Dim_k  from SpatialCapability d, ActorDescription c, SpatialSalience b " +
            "where c.Act_i = b.Act_i and b.Act_i = d.Act_i and b.Turn_t = d.Turn_t and d.ScenarioId = b.ScenarioId and d.ScenarioId = c.ScenarioId ");

        var ActorsData = db.exec("select c.Name , c.\"Desc\", a.Turn_t, a.Dim_k , a.Pos_Coord , b.Sal, b.scenarioid, d.Scenario from actordescription c, VectorPosition a," +
            " SpatialSalience b, scenarioDesc d where c.Act_i = a.Act_i and c.Act_i = b.Act_i and a.ScenarioId = b.ScenarioId and c.ScenarioId = b.ScenarioId and d.ScenarioId = b.ScenarioId and a.Act_i = b.Act_i and " +
            " a.Turn_t = b.Turn_t and a.Dim_k = b.Dim_k");

        var ScenarioData = db.exec("select Scenario, Desc, scenarioid from scenarioDesc");

        EffectivePowArray = EffPowerData[0].values;
        ScenarioArray = ScenarioData[0].values;

        let keys = ActorsData[0].columns;
        let values = ActorsData[0].values;
        let objects = values.map(function (array) {
            let object = {};
            keys.forEach(function (key, i) {
                return object[key] = array[i]
            });
            return object;
        });

        // to find num of scenarios
        SceAraay = [...new Set(objects.map(item => item.ScenarioId))];

        //convert effpow array to obj
        let keys2 = EffPowerData[0].columns;
        let values2 = EffPowerData[0].values;
        let objects2 = values2.map(function (array) {
            let object = {};
            keys2.forEach(function (key, i) {
                return object[key] = array[i]
            });
            return object;
        });

        //group pos data by scenario
        var allscenarioData = {};
        objects.forEach(function (item) {
            var list = allscenarioData[item.ScenarioId];
            if (list) {
                list.push(item);
            } else {
                allscenarioData[item.ScenarioId] = [item];
            }
        });

        //attrs to group pos by
        var attrs = ['ScenarioId', 'Dim_k'];

        function GroupPositionsBySCeAndDim(array, attrs) {
            var output = [];
            for (var i = 0; i < array.length; ++i) {
                var ele = array[i];
                var groups = output;
                for (var j = 0; j < attrs.length; ++j) {
                    var attr = attrs[j];
                    var value = ele[attr];
                    var gs = groups.filter(function (g) {
                        return g.hasOwnProperty('num') && g['num'] == value;
                    });
                    if (gs.length == 0) {
                        var g = {};
                        g['num'] = value;
                        g['data'] = [];
                        groups.push(g);
                        groups = g['data'];
                    } else {
                        groups = gs[0]['data'];
                    }
                }
                groups.push(ele);
            }
            return output;
        }
        var GroupedPosData = GroupPositionsBySCeAndDim(objects, attrs);


        var arrdim = [];
        for (var i = 0; i < GroupedPosData.length; i++) {
            arrPos[i] = [];
            var scenroname = SceAraay[i];
            arrdim[i] = [...new Set(allscenarioData[scenroname].map(item => item.Dim_k))];

            for (var j = 0; j < GroupedPosData[i].data.length; j++) {
                arrPos[i][j] = [];
                GroupedPosData[i].data[j].data.forEach(function (a) {
                    if (!this[a.Name]) {
                        this[a.Name] = { name: a.Name, positions: [] };
                        arrPos[i][j].push(this[a.Name]);
                    }
                    this[a.Name].positions.push(a.Pos_Coord);
                }, Object.create(null));
            }
        }

        //attrs to group effpow by
        var attrs2 = ['ScenarioId', 'Dim_k'];

        function Groupeffpow(array, attrs2) {
            var effpowoutput = [];
            for (var i = 0; i < array.length; ++i) {
                var ele = array[i];
                var groups = effpowoutput;
                for (var j = 0; j < attrs2.length; ++j) {
                    var attr = attrs2[j];
                    var value = ele[attr];
                    var gs = groups.filter(function (g) {
                        return g.hasOwnProperty('num') && g['num'] == value;
                    });
                    if (gs.length == 0) {
                        var g = {};
                        g['num'] = value;
                        g['data'] = [];
                        groups.push(g);
                        groups = g['data'];
                    } else {
                        groups = gs[0]['data'];
                    }
                }
                groups.push(ele);
            }
            return effpowoutput;
        }

        var effpowData = Groupeffpow(objects2, attrs2);

        for (var i = 0; i < effpowData.length; i++) {
            arreff[i] = [];
            var scenroname = SceAraay[i];
            for (var j = 0; j < effpowData[i].data.length; j++) {
                arreff[i][j] = [];
                effpowData[i].data[j].data.forEach(function (a) {
                    arreff[i][j].push(effpowData[i].data[j].data);
                }, Object.create(null));
            }
        }

        for (var i = 0; i < arrPos.length; i += 1) {
            ActorsNames[i] = [];
            for (var j = 0; j < arrPos[i][0].length; j++) {
                ActorsNames[i].push(arrPos[i][0][j].name);
            }
        }

        sendData();
        // option for Dim
        for (i = 0; i < SceAraay.length; i++) {
            $("#SecnarioPicker").append('<option value="' + i + '">' + ScenarioArray[i][0] + '</option>');
        }
    }
    fr.readAsArrayBuffer(file);
    $("#fileUpload").hide();
    $("#content").show();
}

function sendData() {
    updateDesc();
    drawLinechart();
    drawBarchart();
}
$("#SecnarioPicker").on('change', function () {
    selectedScen = $('#SecnarioPicker').val();
    updateDesc();
    drawLinechart();
    drawBarchart();
});
function updateDesc() {

    NumOfTurns = arrPos[selectedScen][0][0].positions.length - 1; // -1 cuz we start from 0
    document.getElementById('NumOfscen').innerHTML = ScenarioArray.length;
    document.getElementById('SecnarioDesc').innerHTML = ScenarioArray[selectedScen][1];;
    document.getElementById('NumOfActors').innerHTML = ActorsNames[selectedScen].length;;
    document.getElementById('NumOfDim').innerHTML = arrPos[selectedScen].length;
    document.getElementById('currentTurn').innerHTML = NumOfTurns;
}