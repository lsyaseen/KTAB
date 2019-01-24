// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2015 King Abdullah Petroleum Studies and Research Center
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// --------------------------------------------
// Demonstrate a very basic, but highly parameterizable, Spatial Model of Politics.
// --------------------------------------------
// TODO: consolidate error checking for read functions.
// These reader functions do a lot of very similar error checking,
// so duplicative code should be put in sub-functions.
// --------------------------------------------

#include <string>
#include <tinyxml2.h>
#include <vector>

#include "kmodel.h"
#include "smp.h"
#include "minicsv.h"


namespace SMPLib {
using std::function;
using std::get;
using std::string;

using KBase::PRNG;
using KBase::KMatrix;
using KBase::KException;
using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::VctrPstn;
using KBase::BigRAdjust;
using KBase::BigRRange;
using KBase::VPModel;
using KBase::State;
using KBase::StateTransMode;
using KBase::VotingRule;
using KBase::PCEModel;
using KBase::ReportingLevel;

using tinyxml2::XMLElement;
using tinyxml2::XMLDocument;

// --------------------------------------------

// --------------------------------------------

SMPModel * SMPModel::csvRead(string fName, uint64_t s, vector<bool> f) {
    using KBase::KException;
    char * errBuff; // as sprintf requires

    LOG(INFO) << "Start SMPModel::csvRead of" << fName;
    csv::ifstream inStream(fName.c_str());
    inStream.set_delimiter(',', "$$");
    inStream.enable_trim_quote_on_str(true, '\"');

    const bool opened = inStream.is_open();
    if (!opened) {
      throw KException(string("Could not open the input csv file ") + fName);
    }

    string scenName = "";
    string scenDesc = "";
    unsigned int numActor = 0;
    unsigned int numDim = 0;
    inStream.read_line();
    try {
      inStream >> scenName >> scenDesc >> numActor >> numDim;
    }
    catch (std::runtime_error &err) {
      LOG(INFO) << "Error in reading csv file";
      LOG(INFO) << err.what();
      return nullptr;
    }

    LOG(INFO) << "Scenario Name: -|" << scenName << "|-";
    LOG(INFO) << "Scenario Description: " << scenDesc;

    if (scenName.length() > Model::maxScenNameLen) {
      throw KException(string("Scenario name can't have more than ")
        + std::to_string(Model::maxScenNameLen) + " chars");
    }
    if (scenDesc.length() > Model::maxScenDescLen) {
      throw KException(string("Scenario description can't have more than ")
        + std::to_string(Model::maxScenDescLen) + " chars");
    }

    LOG(INFO) << "Number of actors:" << numActor;
    LOG(INFO) << "Number of dimensions:" << numDim;
    if (numDim < 1) { // lower limit
        throw(KBase::KException("SMPModel:csvRead: Invalid number of dimensions"));
    }
    if ((numActor < minNumActor) || (maxNumActor < numActor)) { // avoid impossibly low or ridiculously large
        throw(KBase::KException("SMPModel::csvRead: Invalid number of actors"));
    }


    // get the names of dimensions.
    // format (for 3 dimensions) is like this:
    // Actor,Description,Power,Pstn1,Sal1,Pstn2,Sal2,Pstn3,Sal3,
    inStream.read_line();
    string dummyField;
    try {
      inStream >> dummyField; // skip "Actor"
      inStream >> dummyField; // skip "Descripton"
      inStream >> dummyField; // skip "Power"
    }
    catch (std::runtime_error &err) {
      LOG(INFO) << "Error in reading csv file";
      LOG(INFO) << err.what();
      return nullptr;
    }
    auto dNames = vector<string>();
    for (unsigned int d = 0; d < numDim; d++) {
        string dimName, salName;
        try {
          inStream >> dimName;
          inStream >> salName;
        }
        catch (std::runtime_error &err) {
          LOG(INFO) << "Error in reading csv file";
          LOG(INFO) << err.what();
          return nullptr;
        }
        if (dimName.length() > maxDimDescLen) {
          throw KException("Dimension name is too long.");
        }
        dNames.push_back(dimName);
        LOG(INFO) << "Dimension" << d << ":" << dNames[d];
    }

    // Read actor data
    auto actorNames = vector<string>();
    auto actorDescs = vector<string>();
    auto cap = KMatrix(numActor, 1);
    auto nra = KMatrix(numActor, 1);
    auto pos = KMatrix(numActor, numDim);
    auto sal = KMatrix(numActor, numDim);

    for (unsigned int i = 0; i < numActor; i++) {
        string aName = "";
        string aDesc = "";
        double aCap = 0.0;
        inStream.read_line();
        try {
          inStream >> aName >> aDesc >> aCap;
        }
        catch (std::runtime_error &err) {
          LOG(INFO) << "Error in reading csv file";
          LOG(INFO) << err.what();
          return nullptr;
        }

        LOG(INFO) << "Actor:" << i << "name:" << aName;
        // names must have at least 1 character
        if (0 == aName.length() || aName.length() > Model::maxActNameLen) {
          throw KException("Actor's name is either not there or too long.");
        }
        actorNames.push_back(aName);

        LOG(INFO) << "Actor:" << i << "desc:" << aDesc;
        // empty descriptions are allowed
        if (aDesc.length() > Model::maxActDescLen) {
          throw KException("Actor's description is too long.");
        }
        actorDescs.push_back(aDesc);
        //LOG(INFO) << "Actor:" << i << "desc:" << actorDescs[i];

        LOG(INFO) << KBase::getFormattedString("Actor: %u power: %5.1f", i, aCap);
        if (0 > aCap) {
          throw KException("Negative capability is not possible.");
        }
        if (1E8 < aCap) {
          throw KException("Sanity check on upper limit of capability or weight.");
        }
        cap(i, 0) = aCap;


        // get position and salience for each dimension
        double salI = 0.0;
        for (unsigned int d = 0; d < numDim; d++) {
            double dPos = 0.0; // on [0, 100] scale
            double dSal = 0.0; // on [0, 100] scale
            try {
              inStream >> dPos >> dSal;
            }
            catch (std::runtime_error &err) {
              LOG(INFO) << "Error in reading csv file";
              LOG(INFO) << err.what();
              return nullptr;
            }

            LOG(INFO) << KBase::getFormattedString("pos[%u, %u] =  %5.3f", i, d, dPos);
            if ((dPos < 0.0) || (+100.0 < dPos)) { // lower and upper limit
                LOG(INFO) << "SMPModel::csvRead: Actor's position must be within [0.0,100.0]";
                string err = KBase::getFormattedString(
                  "SMPModel::csvRead: Out-of-bounds position for actor %u on dimension %u:  %f",
                  i, d, dPos);
                throw(KException(err));
            }
            pos(i, d) = dPos;

            if ((dSal < 0.0) || (+100.0 < dSal)) { // lower and upper limit
                LOG(INFO) << "SMPModel::csvRead: Error: Actor's salience must be within [0.0,100.0]";
                string err = KBase::getFormattedString(
                  "SMPModel::csvRead: Valid range of salience [0.0, 100.0]. Specified value for actor %u on dimension %u:  %f",
                  i, d, dSal);
                throw(KException(err));
            }
            salI = salI + dSal;
            LOG(INFO) << KBase::getFormattedString("sal[%u, %u] = %5.3f", i, d, dSal);
            if (+100.0 < salI) { // upper limit: no more than 100% of attention to all issues
                LOG(INFO) << "SMPModel::csvRead: Error: Actor's total salience must not be more than 100%";
                string err = KBase::getFormattedString(
                  "SMPModel::csvRead: Expected total salience to be less than 100%. Actual total salience for actor %u:  %f",
                  i, salI);
                throw(KException(err));
            }
            sal(i, d) = dSal;
        }
    }

    LOG(INFO) << "Position matrix:";
    pos.mPrintf("%5.1f  ");

    LOG(INFO) << "Salience matrix:";
    sal.mPrintf("%5.1f  ");

    // get them into the proper internal scale:
    pos = pos / 100.0;
    sal = sal / 100.0;

    double adjRate = 1.0; 
    LOG(INFO) << "Setting ideal-accomodation matrix to identity matrix times "<< adjRate;
    auto accM = adjRate * KBase::iMat(numActor);

    // now that it is read and verified, use the data
    auto sm0 = initModel(actorNames, actorDescs, dNames, cap, pos, sal, accM,  s, f, scenDesc, scenName);
    return sm0;
}
// end of csvRead

SMPModel * SMPModel::xmlRead(string fName, vector<bool> f) {
    using KBase::enumFromName;
    LOG(INFO) << "Start SMPModel::readXML of" << fName;

    auto getFirstChild = [](XMLElement* prntEl, const char * name) {
        XMLElement* childEl = prntEl->FirstChildElement(name);
        if (nullptr == childEl) {
          throw KException(string("Following Element is missing in the input xml: ") + string(name));
        }
        return childEl;
    };

    auto getXMLText = [getFirstChild](XMLElement *element, const char *firstChildName) {
      auto  nameNode = getFirstChild(element, firstChildName);
      auto text = nameNode->GetText();
      if (0 == text) {
        string err = "Following xml element is empty: ";
        err += firstChildName;
        throw KException(err);
      }
      return text;
    };

    auto queryDoubleText = [getXMLText](XMLElement *node, double &data) {
      tinyxml2::XMLError xmlErr = node->QueryDoubleText(&data);
      if (xmlErr != tinyxml2::XML_SUCCESS) {
        string err;
        if (xmlErr == tinyxml2::XML_CAN_NOT_CONVERT_TEXT) {
          //const char* val = aEl->FirstChildElement("capability")->GetText();
          const char* val = getXMLText(node->Parent()->ToElement(), node->Name());
          err = "SMPModel::xmlRead: Invalid xml element-value pair which is ";
          err = err + "'" + node->Name() + " = " + val + "'";
          //LOG(INFO) << err;
        }
        else if (xmlErr == tinyxml2::XML_NO_TEXT_NODE) {
          err = "SMPModel::xmlRead: no text for the following xml element: ";
          err += node->Name();
          //LOG(INFO) << err;
        }

        throw KException(err);
      }
    };

    unsigned int numAct = 0;
    unsigned int numDim = 0;
    uint64_t seed = KBase::dSeed;
    XMLDocument d1;

    // declare variables out here, so as to be in-scope at 'return'
    SMPModel* smp = nullptr;  
    vector<string> actorNames = {};
    vector<string> actorDescs = {};
    vector<string> dNames = {};
    KMatrix capM, posM, salM, accM; // all size 0-by-0

    string sName;
    string sDesc;

    bool modelHasParams = false;

    KBase::VPModel vpmScen;
    KBase::VotingRule vrScen;
    KBase::PCEModel pcemScen;
    KBase::StateTransMode stmScen;
    KBase::BigRRange bigRRangScen;
    KBase::BigRAdjust bigRAdjScen;
    KBase::ThirdPartyCommit tpcScen;
    SMPLib::InterVecBrgn ivbScen;
    SMPLib::SMPBargnModel bModScen;


        d1.LoadFile(fName.c_str());
        auto eid = d1.ErrorID();
        if (0 != eid) {
         string errMsg = string("Tinyxml2 ErrorID: ") + std::to_string(eid)
                + ", Error Name: " + d1.ErrorName(); //  this fails to link: d1.GetErrorStr1();
            throw KException(errMsg);
        }
        // missing data causes the missing XMLElement* to come back as nullptr
        XMLElement* scenEl = d1.FirstChildElement("Scenario");
        if (nullptr == scenEl) {
          throw KException("SMPModel::xmlRead: Root element 'Scenario' is missing in the input xml file.");
        }

        try {
          auto scenNameEl = getFirstChild(scenEl, "name");
          const char *name = scenNameEl->GetText();
          if (name) {
            sName = name;
          }
          LOG(INFO) << "Scenario Name: -|" << sName << "|-";
        }
        catch (KException &ke) {
          // Do nothing.
          LOG(INFO) << ke.msg;
        }

        try {
          auto scenDescEl = getFirstChild(scenEl, "desc");
          const char *desc = scenDescEl->GetText();
          if (desc) {
            sDesc = desc;
          }
          LOG(INFO) << "Scenario Description:" << sDesc;
        }
        catch (KException &ke) {
          // Do nothing.
          LOG(INFO) << ke.msg;
        }

        const char* sd2 = getXMLText( scenEl, "prngSeed");
        seed = std::stoull(sd2);
        LOG(INFO) << KBase::getFormattedString("Read PRNG seed:  %020llu", seed);

        XMLElement *modelParamsEl = nullptr;
        try {
          modelParamsEl = getFirstChild(scenEl, "ModelParameters");
        }
        catch(KException &ke) {
          LOG(INFO) << "No model parameters in XML scenario. Using defaults";
        }
        //auto modelParamsEl = getFirstChild(scenEl, "ModelParameters");
        if (nullptr != modelParamsEl) {
            function <string (const char*)> showChild = [getFirstChild, modelParamsEl](const char* name ) {
                auto el = getFirstChild(modelParamsEl, name);
                auto txt = el->GetText();
                if( !txt ) {
                  string err = "SMPModel::xmlRead: Follwing Text element is empty: ";
                  err += name;
                  throw KException(err);
                }
                string s = txt;
                return s;
            };
            LOG(INFO) << "Reading model parameters from XML scenario ...";
            vpmScen = enumFromName<VPModel>(showChild("VictoryProbModel"), KBase::VPModelNames);
            vrScen = enumFromName<VotingRule>(showChild("VotingRule"), KBase::VotingRuleNames);
            pcemScen = enumFromName<PCEModel>(showChild("PCEModel"), KBase::PCEModelNames);
            stmScen = enumFromName<StateTransMode>(showChild("StateTransitions"), KBase::StateTransModeNames);
            bigRRangScen = enumFromName<BigRRange>(showChild("BigRRange"), KBase::BigRRangeNames);
            bigRAdjScen = enumFromName<BigRAdjust>(showChild("BigRAdjust"), KBase::BigRAdjustNames);
            tpcScen = enumFromName<ThirdPartyCommit>(showChild("ThirdPartyCommit"), KBase::ThirdPartyCommitNames);
            ivbScen = enumFromName<InterVecBrgn>(showChild("InterVecBrgn"), InterVecBrgnNames);
            bModScen = enumFromName<SMPBargnModel>(showChild("BargnModel"), SMPBargnModelNames);

            modelHasParams = true;
        }

        // read all the dimensions
            numDim = 0;
            XMLElement* dimsEl = getFirstChild(scenEl, "Dimensions");
            XMLElement* dEl = getFirstChild(dimsEl, "dName");
            while (nullptr != dEl) {
                const char* dn = dEl->GetText();
                if (nullptr != dn) {
                  dNames.push_back(string(dn));
                }
                else {
                  throw KException("A dimension element dName is there but it is empty.");
                }
                // move to the next, if any
                numDim++;
                dEl = dEl->NextSiblingElement("dName");
            }
            LOG(INFO) << "Found" << numDim << "dimensions";
        
        // Count the actors
          numAct = 0;
          XMLElement* actors = getFirstChild(scenEl, "Actors");
          XMLElement* actor = getFirstChild(actors, "Actor");
          while (nullptr != actor) {
            // move to the next, if any
            numAct++;
            actor = actor->NextSiblingElement("Actor"); 
          }
        LOG(INFO) << "Found" << numAct << "actors";
 
        capM = KMatrix(numAct, 1);
        posM = KMatrix(numAct, numDim);
        salM = KMatrix(numAct, numDim);

        // Read all the actors
            XMLElement* actorsEl = getFirstChild(scenEl, "Actors");
            XMLElement* aEl = getFirstChild(actorsEl, "Actor");
            unsigned int actCntr = 0;
            while (nullptr != aEl) { 
                const char* aName = getXMLText(aEl, "name");
                const char* aDesc = getXMLText(aEl, "description");

                actorNames.push_back(string(aName));
                actorDescs.push_back(string(aDesc));

                double cap = -10.0; // another impossible value
                auto capNode = getFirstChild(aEl, "capability");

                queryDoubleText(capNode, cap);

                if (0.0 >= cap) { // could be quite large
                  throw KException("SMPModel::xmlRead: capability value can't be negative");
                }
                capM(actCntr, 0) = cap;
                auto  posEl = getFirstChild(aEl, "Position");
                auto vPos = KMatrix(numDim, 1);
                unsigned int dimCntr = 0; // count and verify dimensions
                auto pdEl = getFirstChild(posEl, "dCoord");
                while (nullptr != pdEl) {
                  double pd = 0.0;
                  queryDoubleText(pdEl, pd);
                  LOG(INFO) << KBase::getFormattedString(
                    "Read dimension %u: %.2f", dimCntr, pd);
                  vPos(dimCntr, 0) = pd;
                  pdEl = pdEl->NextSiblingElement("dCoord");
                  dimCntr++; // got one
                }
                LOG(INFO) << "Read" << dimCntr << "dimensional components";
                if (numDim != dimCntr) {
                  throw KException("SMPModel::xmlRead: Count of dimensions doesn't match with the Positions data.");
                }

                auto  salEl = getFirstChild(aEl, "Salience");
                auto vSal = KMatrix(numDim, 1);
                dimCntr = 0;
                auto vsEl = getFirstChild(salEl, "dSal");
                double totalSal = 0.0;
                while (nullptr != vsEl) {
                  double vs = 0.0;
                  queryDoubleText(vsEl, vs);
                  if((vs < 0.0) || (+100.0 < vs)) {
                    LOG(INFO) << "SMPModel::xmlRead: Error: Actor's salience must be within [0.0,100.0]";
                    string err = KBase::getFormattedString(
                      "SMPModel::xmlRead: Valid range of salience [0.0, 100.0]. Specified value for actor %u on dimension %u:  %f",
                      actCntr, dimCntr, vs);
                    throw(KException(err));
                  }
                  totalSal += vs;
                  LOG(INFO) << KBase::getFormattedString("sal[%u, %u] = %5.3f", actCntr, dimCntr, vs);
                  vSal(dimCntr, 0) = vs;
                  vsEl = vsEl->NextSiblingElement("dSal");
                  dimCntr++; // got one
                }
                if (+100.0 < totalSal) { // upper limit: no more than 100% of attention to all issues
                  LOG(INFO) << "SMPModel::xmlRead: Error: Actor's total salience must not be more than 100%";
                  string err = KBase::getFormattedString(
                    "SMPModel::xmlRead: Expected total salience to be less than 100%. Actual total salience for actor %u:  %f",
                    actCntr, totalSal);
                  throw(KException(err));
                }

                LOG(INFO) << "Read" << dimCntr << "salience components";
                if (numDim != dimCntr) {
                  throw KException("SMPModel::xmlRead: Count of dimensions doesn't match with the Salience data.");
                }

                capM(actCntr,0)=cap;
                for (unsigned int i=0; i<numDim; i++){
                    salM(actCntr, i)=vSal(i,0);
                    posM(actCntr, i) = vPos(i,0);
                }
                // move to the next, if any
                actCntr++;
                aEl = aEl->NextSiblingElement("Actor");
            }
        
        // Read the accomodation matrix
        LOG(INFO) << "Setting ideal-accomodation matrix to identity matrix (initially, needs reset)";
        accM = KBase::iMat(numAct);
            XMLElement* iadEl = getFirstChild(scenEl, "IdealAdjustment");
            if (nullptr != iadEl) {
                LOG(INFO) << "Reading IdealAdjustment matrix";
                auto nameNdx = [numAct, actorNames] (const string n) {
                    int ndx = -1;
                    for (unsigned int i=0; i<numAct; i++){
                        if (actorNames[i] == n){
                            ndx = i;
                            break;
                        }
                    }
                    if (0 > ndx) {
                      throw KException("SMPModel::xmlRead: Actor not found.");
                    }
                    return ndx;
                };
                unsigned int numIAPairs = 0;
                XMLElement* iaEl = getFirstChild(iadEl, "iaPair");
                while (nullptr != iaEl){
                    auto aiStr = string(getXMLText(iaEl, "adjustingIdeal"));
                    auto aiNdx = nameNdx(aiStr);
                    auto rpStr = string(getXMLText(iaEl, "referencePos"));
                    auto rpNdx = nameNdx(rpStr);
                    auto adjEl = getFirstChild(iaEl, "adjust");
                    double adjV = 0.0;
                    queryDoubleText(adjEl, adjV);
                    LOG(INFO) << KBase::getFormattedString(" %6s (%u) :  %6s  (%u) = %.3f",
                      aiStr.c_str(), aiNdx, rpStr.c_str(), rpNdx, adjV);
                    accM(aiNdx, rpNdx)=adjV;
                    
                    numIAPairs++;
                    iaEl = iaEl->NextSiblingElement("iaPair");
                }
                LOG(INFO) << "Found" << numIAPairs << "iaPair";
            }

    posM = posM / 100.0;
    salM = salM / 100.0;
    LOG(INFO) << "End SMPModel::readXML of" << fName;
    // now that it is read and verified, use the data  
    smp = initModel(actorNames, actorDescs, dNames, capM, posM, salM, accM, seed, f, sDesc, sName);
    if (nullptr == smp) {
      throw KException("SMPModel::xmlRead: Model Initialization failed to provide a valid smp object.");
    }

    if (modelHasParams) {
        LOG(INFO) << "Setting SMPModel parameters from XML scenario ...";
        smp->vpm = vpmScen;
        smp->vrCltn = vrScen;
        smp->pcem = pcemScen;
        smp->stm = stmScen;
        smp->bigRRng = bigRRangScen;
        smp->bigRAdj = bigRAdjScen;
        smp->tpCommit = tpcScen;
        smp->ivBrgn = ivbScen;
        smp->brgnMod = bModScen;
    }

    return smp;
}
// end of readXML


}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
