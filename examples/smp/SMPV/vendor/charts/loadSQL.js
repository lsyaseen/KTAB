var positionalData = [],
    ActorsNames = [],
    positionsArray = [],
    effpowArray,
    ScenarioArray = [],
    EffectivePowArray,
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
        let objects = values.map(function(array){ 
            let object = {};
            keys.forEach(function(key, i) {
                return  object[key] = array[i]});
            return object;
         });

        objects.forEach(function (a) {
            if (!this[a.Name]) {
                this[a.Name] = { name: a.Name, positions: [] };
                positionalData.push(this[a.Name]);
            }
            this[a.Name].positions.push(a.Pos_Coord);
        }, Object.create(null));

        for (var i = 0; i < positionalData.length; i += 1) {
            ActorsNames.push(positionalData[i].name);
        }
        for (var i = 0; i < positionalData.length; i += 1) {
            positionsArray.push(positionalData[i].positions);
        }
        NumOfTurns = positionsArray[0].length - 1; // -1 cuz we start from 0

        document.getElementById('SecnarioName').innerHTML = ScenarioArray[0][0];;
        document.getElementById('SecnarioDesc').innerHTML = ScenarioArray[0][1];;
        document.getElementById('NumOfActors').innerHTML = ActorsNames.length;;
        document.getElementById('NumOfDim').innerHTML = 1;
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

