var positionalData = [],
    ActorsNames = [],
    positionsArray = [],
    effpowArray,
    ScenarioArray = [],
    EffectivePowArray,
    whyActorMoved,
    arrPos = [],
    arreff = [],
    arrBargns = [],
    arrNetwork = [],
    effpow = {},
    selectedScen = 0,
    NetworkPositions,
    actorsObj,
    networkdata,
    EffPowerData,
    NumOfTurns;
var db, ActorsData, NetworkactorsData;


var groupedScenarios, scenaioIds, NoOfDim;
function GraphData(file, onloaddb) {
    
    this.dbfile = file;
    this.actorsById = {};
    this.linksByBid = {};
    var __this = this;
    this.onloaddb = onloaddb
    var fr = new FileReader();

    if(file.size > 1717986918){
       alert("File must be smaller than 1.5 GB");
      return location.reload();
    };

    fr.onload = function () {
        var Uints = new Uint8Array(fr.result);
        db = new window.SQL.Database(Uints);
        var maxturn = db.exec("select max(Turn_t) as turns from Bargn");
        __this.turns = maxturn[0].values[0][0];
        __this.initScenarioData();
        __this.loadActorsData();
        __this.loadBargainsData();
        loadCurrentTurnData(1);//initial turn
        updateDesc();
        __this.onloaddb();

    }
    fr.readAsArrayBuffer(this.dbfile);


    this.loadActorsData = function () {
        ActorsData = db.exec("select c.Name,  c.Act_i as id , c.\"Desc\", a.Turn_t, a.Dim_k , a.Pos_Coord , b.Sal, b.scenarioid, d.Scenario from actordescription c, VectorPosition a," +
            " SpatialSalience b, scenarioDesc d where c.Act_i = a.Act_i and c.Act_i = b.Act_i and a.ScenarioId = b.ScenarioId and c.ScenarioId = b.ScenarioId and d.ScenarioId = b.ScenarioId and a.Act_i = b.Act_i and " +
            " a.Turn_t = b.Turn_t and a.Dim_k = b.Dim_k");
        let keys = ActorsData[0].columns;
        let values = ActorsData[0].values;
        actorsObj = values.map(function (array) {
            let object = {};
            keys.forEach(function (key, i) {
                return object[key] = array[i]
            });
            return object;
        });
        __this.loadPositionsData();
        __this.loadEfPower();

    }
    this.initScenarioData = function () {

        var NewScenarioData = db.exec("select s.Scenario, d.ScenarioId, d.Dim_k, d.'Desc' " +
            " FROM DimensionDescription d, ScenarioDesc s where s.ScenarioId = d.ScenarioId ");
        var NewScenarioArray = NewScenarioData[0].values;
        var NewactorsObj = NewScenarioArray.map(function (array) {
            let object = {};
            NewScenarioData[0].columns.forEach(function (key, i) {
                return object[key] = array[i]
            });
            return object;
        });

        //group by scenario 
        const group = (arr, k) => arr.reduce((r, c) => (r[c[k]] = [...r[c[k]] || [], c], r), {});
        groupedScenarios = group(NewactorsObj, 'ScenarioId')

        //to find the no. of Scenarios
        scenaioIds = Object.keys(groupedScenarios);

        NoOfDim = groupedScenarios[scenaioIds[selectedScen]].length;

        // to find num of scenarios
        var ScenarioData = db.exec("select Scenario, Desc, scenarioid from scenarioDesc");
        ScenarioArray = ScenarioData[0].values;

        // option for scenarios
        for (i = 0; i < scenaioIds.length; i++) {
            $("#SecnarioPicker").append('<option value="' + i + '">' + groupedScenarios[scenaioIds[i]][0]['Scenario'] + '</option>');
        }
        // option for Dim
        for (i = 0; i < NoOfDim; i++) {
            $("#Dimpicker").append('<option value="' + i + '">' + (i + 1) + '</option>');
        }
        $("#Dimpicker").val(0); //select first opt by default

    }


    this.loadPositionsData = function () {
        //group pos data by scenario
        var allscenarioData = {};
        actorsObj.forEach(function (item) {
            var list = allscenarioData[item.ScenarioId];
            if (list) {
                list.push(item);
            } else {
                allscenarioData[item.ScenarioId] = [item];
            }
        });

        //group Positions by Scenario and Dim   
        var GroupedPosData = GroupbySceAndDim(actorsObj);


        var arrdim = [];
        for (var i = 0; i < GroupedPosData.length; i++) {
            arrPos[i] = [];
            var scenroname = scenaioIds[i];
            arrdim[i] = [...new Set(allscenarioData[scenroname].map(item => item.Dim_k))];
            for (var j = 0; j < GroupedPosData[i].data.length; j++) {
                arrPos[i][j] = [];
                GroupedPosData[i].data[j].data.forEach(function (a) {
                    if (!this[a.Name]) {
                        this[a.Name] = { name: a.Name, positions: [], aid: a.id };
                        arrPos[i][j].push(this[a.Name]);
                    }
                    this[a.Name].positions.push(a.Pos_Coord);

                }, Object.create(null));
            }
        }
        for (var i = 0; i < arrPos.length; i += 1) {
            ActorsNames[i] = [];
            for (var j = 0; j < arrPos[i][0].length; j++) {
                ActorsNames[i].push(arrPos[i][0][j].name);
            }
        }
    }

    this.loadEfPower = function () {

        EffPowerData = db.exec("select distinct d.scenarioid, c.Act_i, c.Name , d.Cap * b.Sal as fpower, b.Dim_k  from SpatialCapability d, ActorDescription c, SpatialSalience b " +
            "where c.Act_i = b.Act_i and b.Act_i = d.Act_i and b.Turn_t = d.Turn_t and d.ScenarioId = b.ScenarioId and d.ScenarioId = c.ScenarioId ");
        EffectivePowArray = EffPowerData[0].values;

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

        //group effePoer by Scenario and Dim   
        var effpowData = GroupbySceAndDim(objects2);

        for (var i = 0; i < effpowData.length; i++) {
            arreff[i] = [];
            for (var j = 0; j < effpowData[i].data.length; j++) {
                arreff[i][j] = [];

                effpowData[i].data[j].data.forEach(function (a) {
                    arreff[i][j].push(effpowData[i].data[j].data);
                }, Object.create(null));

            }
        }

    }

    this.loadBargainsData = function () {
        var BarginsData = db.exec("select M.Movd_Turn as turn, MN.Name as Moved_Name, M.Dim_k, M.PrevPos,"
            + " M.CurrPos,M.Diff , M.Mover_BargnID as BargnID , MI.Name as Initiator, MR.Name as Receiver ,B.scenarioid"
            + " from ( select L0.Act_i, L0.Dim_k, L0.Turn_t as Movd_Turn, L0.Mover_BargnId, L0.scenarioid, L1.scenarioid,"
            + " L0.Pos_Coord as CurrPos, L1.Pos_Coord as PrevPos, L0.Pos_Coord - L1.Pos_Coord as Diff"
            + " from (select * from VectorPosition where Turn_t <> 0 ) "
            + " as L0 inner join (select * from VectorPosition )"
            + " as L1 on L0.Turn_t = (L1.Turn_t+1) and L0.Act_i = L1.Act_i and L0.Dim_k = L1.Dim_k and L0.scenarioid = L1.scenarioid "
            + " where L0.Pos_Coord <> L1.Pos_Coord ) as M inner join Bargn as B on M.Mover_BargnId = B.BargnId and M.scenarioid = B.scenarioid"
            + " inner join ( select Act_i,scenarioid, name from ActorDescription ) as MI "
            + " on B.Init_Act_i = MI.Act_i and MI.scenarioid = B.scenarioid inner join ( select Act_i, scenarioid, name from ActorDescription "
            + " ) as MR on B.Recd_Act_j = MR.Act_i and MR.scenarioid = B.scenarioid inner join"
            + " ActorDescription as MN on M.Act_i = MN.Act_i and MN.scenarioid = B.scenarioid ORDER BY M.Dim_k");

        whyActorMoved = BarginsData[0].values;
        //convert bargain array to obj
        let Bargnskeys = BarginsData[0].columns;
        let Bargnsvalues = BarginsData[0].values;
        let Bargnsobjects = Bargnsvalues.map(function (array) {
            let object = {};
            Bargnskeys.forEach(function (key, i) {
                return object[key] = array[i]
            });
            return object;
        });

        //group Bargains Data by Scenario and Dim   
        var BargainsData = GroupbySceAndDim(Bargnsobjects);

        for (var i = 0; i < BargainsData.length; i++) {
            arrBargns[i] = [];
            for (var j = 0; j < BargainsData[i].data.length; j++) {
                arrBargns[i][j] = [];
                arrBargns[i][j].push(BargainsData[i].data[j].data);
            }
        }


    }
    function GroupbySceAndDim(array) {
        var attrs = ['ScenarioId', 'Dim_k'];
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

    this.getActors = function () {
        var ret = [];
        for (var key in __this.actorsById) {
            if (__this.actorsById.hasOwnProperty(key)) {
                ret.push(__this.actorsById[key]);
            }
        }
        return ret;
    }
}

function loadCurrentTurnData(turn) {

    NetworkactorsData = db.exec("select c.Act_i as id, Name as name, a.Turn_t, a.Dim_k,  b.scenarioid from actordescription c, VectorPosition a," +
        " SpatialSalience b where c.Act_i = a.Act_i and c.Act_i = b.Act_i and a.ScenarioId = b.ScenarioId and b.ScenarioId = c.ScenarioId and b.ScenarioId = '" + scenaioIds[selectedScen] +
        "' and a.Dim_k = b.Dim_k and b.Dim_k = " + selectedDimNum + " and a.Turn_t = b.Turn_t and b.Turn_t= " + turn + " and b.scenarioid = '" + scenaioIds[selectedScen] + "'");
    networkData = db.exec("select B.ScenarioID, B.Turn_t, B.BargnID, B.Init_Act_i, B.Recd_Act_j,M.Dim_k, AI.Name as Init, AR.Name as Rcvr, " +
        " B.Init_Prob, B.Recd_Prob from Bargn as B inner join ActorDescription as AI on B.Init_Act_i = AI.Act_i and B.ScenarioID = AI.ScenarioID inner join " +
        " ActorDescription as AR on B.Recd_Act_j = AR.Act_i and B.ScenarioID = AR.ScenarioID inner join " +
        " VectorPosition as M on M.Act_i = B.Init_Act_i and M.ScenarioId = B.ScenarioID  and M.Turn_t = B.Turn_t  " +
        "  where M.Turn_t = " + turn + " and M.Dim_k = " + selectedDimNum + " and M.ScenarioId = '" + scenaioIds[selectedScen] +
        "' order by B.Turn_t, BargnID ");

        acceptedBgn = db.exec(" select B.*, M.Dim_k, AI.Name as Init, AR.Name as Rcvr " +
        " from (select ScenarioId, Turn_t, BargnId, Init_Act_i, Recd_Act_j " +
        " ,'Init' as Q from Bargn where Init_Seld = 1 and Turn_t =  " + turn + " and "+
        " ScenarioId ='" + scenaioIds[selectedScen] + "' union select ScenarioId," +
        " Turn_t, BargnId, Init_Act_i, Recd_Act_j ,'Rcvr' from Bargn  " +
        " where Recd_Seld = 1 and Turn_t =  " + turn + " and "+
        " ScenarioId ='" + scenaioIds[selectedScen] + "') as B inner join  ActorDescription as AI on " +
        " B.Init_Act_i = AI.Act_i and B.ScenarioID = AI.ScenarioID inner join " +
        " ActorDescription as AR on B.Recd_Act_j = AR.Act_i and " +
        " B.ScenarioID = AR.ScenarioID inner join " +
        " VectorPosition as M on M.ScenarioId = B.ScenarioID and BargnId = M.Mover_BargnId and M.Dim_k = " + selectedDimNum);

    }

$("#SecnarioPicker").on('change', function () {
    selectedScen = $('#SecnarioPicker').val();
    NoOfDim = groupedScenarios[scenaioIds[selectedScen]].length;
    selectedDimNum = 0;
    updateDesc();

});
function updateDesc() {
    NumOfTurns = arrPos[selectedScen][0][0].positions.length - 1; // -1 cuz we start from 0
    document.getElementById('NumOfscen').innerHTML = scenaioIds.length;
    document.getElementById('SecnarioDesc').innerHTML = ScenarioArray[selectedScen][1];;
    document.getElementById('NumOfActors').innerHTML = ActorsNames[selectedScen].length;;
    document.getElementById('NumOfDim').innerHTML = groupedScenarios[scenaioIds[selectedScen]].length;
    document.getElementById('currentTurn').innerHTML = currentTurn;
}

var gd;
function getfile() {

    var files = document.getElementById("uploadInput").files;
    var file = files[0];
   $(".loading").show();
    var SelecteDBfileName =file.name;
     document.getElementById("fileNameText").value = SelecteDBfileName;
    gd = new GraphData(file, function () {
        $(".loading").hide();
        $("#fileUpload").hide();
        $("#content").show();
        getData();
        InitializeSlider(NumOfTurns);
        drawChart();
        drawLine();
        drawNetwork();

    });
}
