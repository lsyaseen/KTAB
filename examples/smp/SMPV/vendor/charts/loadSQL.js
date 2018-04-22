var positionalData = [],
    ActorsNames = [],
    positionsArray = [],
    effpowArray,
    ScenarioArray = [],
    EffectivePowArray,
    DimAraay = [],
    effpow = {},
    NumOfTurns;

function getfile() {
    var files = document.getElementById("uploadInput").files;
    var file = files[0];
    var fr = new FileReader();
    fr.onload = function () {
        var Uints = new Uint8Array(fr.result);
        var db = new window.SQL.Database(Uints);

        var EffPowerData = db.exec("select distinct c.Name , d.Cap * b.Sal as fpower, b.Dim_k  from SpatialCapability d, ActorDescription c, SpatialSalience b " +
            "where c.Act_i = b.Act_i and b.Act_i = d.Act_i and b.Turn_t = d.Turn_t ");

        var ActorsData = db.exec("select c.Name , c.\"Desc\", a.Turn_t, a.Dim_k , a.Pos_Coord , b.Sal from actordescription c, VectorPosition a," +
            " SpatialSalience b where c.Act_i = a.Act_i and c.Act_i = b.Act_i and a.ScenarioId = b.ScenarioId and a.Act_i = b.Act_i and " +
            " a.Turn_t = b.Turn_t and a.Dim_k = b.Dim_k");

        var ScenarioData = db.exec("select Scenario, Desc from scenarioDesc");

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

        // to find num of dim
        DimAraay = [...new Set(objects.map(item => item.Dim_k))];

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

        //group effpow by dim
        objects2.forEach(function (item) {
            var list = effpow[item.Dim_k];
            if (list) {
                list.push(item);
            } else {
                effpow[item.Dim_k] = [item];
            }
        });

        //group positions by dim
        var allposuions = {};
        objects.forEach(function (item) {
            var list = allposuions[item.Dim_k];
            if (list) {
                list.push(item);
            } else {
                allposuions[item.Dim_k] = [item];
            }
        });

        // var arr = [];  // creates a new array .
        for (var i = 0; i < DimAraay.length; i++) {
            positionalData[i] = [];
            allposuions[i].forEach(function (a) {
                if (!this[a.Name]) {
                    this[a.Name] = { name: a.Name, positions: [] };
                    positionalData[i].push(this[a.Name]);
                }
                this[a.Name].positions.push(a.Pos_Coord);
            }, Object.create(null));
        }

        for (var i = 0; i < positionalData[0].length; i += 1) {
            ActorsNames.push(positionalData[0][i].name);
        }
        for (var i = 0; i < positionalData[0].length; i += 1) {
            positionsArray.push(positionalData[0][i].positions);
        }
        NumOfTurns = positionsArray[0].length - 1; // -1 cuz we start from 0

        document.getElementById('SecnarioName').innerHTML = ScenarioArray[0][0];;
        document.getElementById('SecnarioDesc').innerHTML = ScenarioArray[0][1];;
        document.getElementById('NumOfActors').innerHTML = ActorsNames.length;;
        document.getElementById('NumOfDim').innerHTML = DimAraay.length;
        document.getElementById('currentTurn').innerHTML = NumOfTurns;

        sendData();
    }
    fr.readAsArrayBuffer(file);


    $("#fileUpload").hide();
    $("#content").show();
}

function sendData() {
    drawLinechart();
    drawBarchart();
}

