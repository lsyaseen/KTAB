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

#ifndef DEMO_WATERMIN_PROD_H
#define DEMO_WATERMIN_PROD_H

#include <string>
#include <vector>

#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"

// --------------------------------------------
// just the names of products
// --------------------------------------------


namespace DemoWaterMin {
using KBase::KMatrix;
using std::vector;
using std::string;
// 38 elements
//product
vector<string> prodNames = {
    "waterchange" ,   // 0 
    "alfalfa" ,       // 1
    "Other_Fodder" ,  // 2
    "Dates" ,         // 3
    "Fruit_fresh" ,   // 4
    "Wheat" ,         // 5
    "Milk_cow" ,
    "Fruit_citrus" ,
    "Sorghum" ,
    "Maize" ,
    "Meat_chicken" ,
    "Potatoes" ,
    "Grapes" ,
    "Tomatoes" ,
    "Watermelons" ,
    "Melons_other" ,
    "Meat_cow" ,
    "Vegetables" ,
    "Eggs" ,
    "Gourds" ,
    "Pulses" ,
    "Eggplants" ,
    "Milk_camel" ,
    "Cucumbers" ,
    "Onions" ,
    "Milk_goat" ,
    "Carrots_turn" ,
    "Milk_sheep" ,
    "Millet" ,
    "Okra" ,
    "Barley" ,
    "Sesameseed" ,
    "Meat_camel" ,
    "Meat_sheep" ,
    "Groundnuts" ,
    "Cabbages" ,
    "Meat_goat" ,
    "Honey"
};
} ; // end of namespace
// --------------------------------------------
#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
