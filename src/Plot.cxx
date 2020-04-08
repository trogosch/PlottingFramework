// Plotting Framework
//
// Copyright (C) 2019-2020  Mario Krüger
// Contact: mario.kruger@cern.ch
// For a full list of contributors please see docs/Credits
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Plot.h"

using namespace PlottingFramework;
namespace PlottingFramework {


Plot::Plot()
{
  mName = "dummyName";
  mPlotStyle = "default";
  mControlString.clear();
  mFigureGroup = "";
  mFigureCategory = "";
  mCurrPad = 1;
}
  
  
Plot::Plot(string name, string figureGroup, string plotStyle)
: Plot()
{
  mName = name;
  mFigureGroup = figureGroup;
  mPlotStyle = plotStyle;
  
  // in case category was specified via figureGroup:my/category/tree
  auto subPathPos = figureGroup.find(":");
  if(subPathPos != string::npos)
  {
    mFigureGroup = figureGroup.substr(0, subPathPos);
    mFigureCategory = figureGroup.substr(subPathPos+1);
  }
}


Plot::Plot(ptree &plotTree)
{
  try{
    mName = plotTree.get<string>("name");
    mFigureGroup = plotTree.get<string>("figureGroup");
    mFigureCategory = plotTree.get<string>("figureCategory");
    mPlotStyle = plotTree.get<string>("plotStyle");
    
    // loop over pads defined in property tree
    int padID = 1;
    for(auto& pad : plotTree)
    {
      if(pad.first.find("PAD") != string::npos)
      {
        mControlString[padID] = pad.second.get<string>("controlString");
        for(auto& content : pad.second)
        {
          // add data
          if(content.first.find("DATA") != string::npos)
          {
            string type = content.second.get<string>("type");
            if(type == "data")
            {
              mData[padID].push_back(std::make_shared<Data>(content.second));
            }
            if(type == "ratio")
            {
              mData[padID].push_back(std::make_shared<Ratio>(content.second));
            }
          }
          
          // add boxes
          if(content.first.find("BOX") != string::npos)
          {
            string type = content.second.get<string>("type");
            if(type == "legend")
            {
              mBoxes[padID].push_back(std::make_shared<LegendBox>(content.second));
            }
            if(type == "text")
            {
              mBoxes[padID].push_back(std::make_shared<TextBox>(content.second));
            }
          }
          
          // add axes
          if(content.first.find("AXIS") != string::npos)
          {
            string axis = content.second.get<string>("name");
            mAxes[padID][axis] = std::make_shared<Axis>(content.second);
          }
        }
        padID++;
      }
    }
    
    
  }catch(...){
    ERROR("Could not construct data from ptree.");
  }
  mCurrPad = 1;
}

void Plot::AddFrame(string dataName, string inputIdentifier)
{
  if(inputIdentifier == "") inputIdentifier = mFigureGroup; // by default set identifier equal to figuregroup of plot
  mData[mCurrPad].insert(mData[mCurrPad].begin(), std::make_shared<Data>(dataName, inputIdentifier, "", 0, 0, 0, "AXIS", 1., std::make_pair(-999, -999), std::make_pair(0,0)));
}


void Plot::AddData(string dataName, string inputIdentifier, string lable, int marker, int color, string drawingOptions, double cutoff, double cutoffLow)
{
  if(inputIdentifier == "") inputIdentifier = mFigureGroup; // by default set identifier equal to figuregroup of plot
  mData[mCurrPad].push_back(std::make_shared<Data>(dataName, inputIdentifier, lable, color, marker, 0, drawingOptions, 1., std::make_pair(cutoffLow, cutoff), std::make_pair(0,0)));
}


void Plot::AddRatio(string numerHist, string numerHistIdentifier, string denomHist, string denomHistIdentifier, string lable, int marker, int color, string drawingOptions, double cutoff, double cutoffLow)
{
  if(numerHistIdentifier == "") numerHistIdentifier = mFigureGroup;
  if(denomHistIdentifier == "") denomHistIdentifier = mFigureGroup;
  int padID = (mData[mCurrPad].empty()) ? mCurrPad : 2; //mCurrPad TODO:: remove this stupid hack
  mData[padID].push_back(std::make_shared<Ratio>(numerHist, numerHistIdentifier, denomHist, denomHistIdentifier, lable, color, marker, 0, drawingOptions, "", 1., std::make_pair(cutoffLow, cutoff), std::make_pair(0,0)));
}


void Plot::AddTextBox(double xPos, double yPos, string text, bool userCoordinates, int borderStyle, int borderSize, int borderColor)
{
  mBoxes[mCurrPad].push_back(std::make_shared<TextBox>(userCoordinates, false, xPos, yPos, borderStyle, borderSize, borderColor, text));
}
// todo user coord should be possible in both cases
void Plot::AddLegendBox(double xPos, double yPos, string title, bool userCoordinates, int nColumns, int borderStyle, int borderSize, int borderColor)
{
  // TODO: add userCoordinates to arguments
  mBoxes[mCurrPad].push_back(std::make_shared<LegendBox>(userCoordinates, false, xPos, yPos, borderStyle, borderSize, borderColor, title, nColumns));
}
void Plot::AddLegendBox(string title, int nColumns, int borderStyle, int borderSize, int borderColor)
{
  // TODO: add userCoordinates to arguments
  mBoxes[mCurrPad].push_back(std::make_shared<LegendBox>(false, true, 0, 0, borderStyle, borderSize, borderColor, title, nColumns));
}

void Plot::SetAxisRange(string axis, double low, double high)
{
  int seletedPad = mCurrPad;
  vector<string> allowedAxes = {"X", "Y", "Z"};
  if(std::find(allowedAxes.begin(), allowedAxes.end(), axis) == allowedAxes.end())
  {
    // an axis alias was specified, which refers to specific padID-axis combination
    seletedPad = -1;
  }
  if(mAxes[seletedPad].find(axis) != mAxes[seletedPad].end())
  {
    mAxes[seletedPad][axis]->SetAxisRange(low, high);
  }
  else
  {
    mAxes[seletedPad][axis] = std::make_shared<Axis>(axis, std::make_pair(low, high));
  }
}


void Plot::SetAxisTitle(string axis, string axisTitle)
{
  int seletedPad = mCurrPad;
  vector<string> allowedAxes = {"X", "Y", "Z"};
  if(std::find(allowedAxes.begin(), allowedAxes.end(), axis) == allowedAxes.end())
  {
    // an axis alias was specified, which refers to specific padID-axis combination
    seletedPad = -1;
  }
  if(mAxes[seletedPad].find(axis) != mAxes[seletedPad].end())
  {
    mAxes[seletedPad][axis]->SetAxisTitle(axisTitle);
  }
  else
  {
    mAxes[seletedPad][axis] = std::make_shared<Axis>(axis, axisTitle);
  }
}


void Plot::Print()
{
}

// DELETE this inefficient nonsense
map<string, set<string>> Plot::GetRequiredInputData()
{
  map<string, set<string>> requiredDataPlot;
  for(auto& padData : mData)
  {
    for(auto& data : padData.second)
    {
      requiredDataPlot[data->GetInputIdentifier()].insert(data->GetName());
      if(data->GetType() == "ratio")
      {          requiredDataPlot[std::dynamic_pointer_cast<Plot::Ratio>(data)->GetDenomIdentifier()].insert(std::dynamic_pointer_cast<Plot::Ratio>(data)->GetDenomName());
      }
    }
  }
  return requiredDataPlot;
}


ptree Plot::GetPropetyTree()
{
  // convert properties of plot to ptree
  ptree plotTree;
  plotTree.put("name", mName);
  plotTree.put("figureGroup", mFigureGroup);
  plotTree.put("figureCategory", mFigureCategory);
  plotTree.put("plotStyle", mPlotStyle);
  
  for(auto& padData : mData)
  {
    int padID = padData.first;
    ptree padTree;
    padTree.put("controlString", mControlString[padID]);
    
    // add data of pad
    int dataID = 1;
    for(auto& data : padData.second){
      padTree.put_child("DATA_" + std::to_string(dataID), data->GetPropertyTree());
      dataID++;
    }
    
    // add boxes of pad
    int boxID = 1;
    for(auto& box : mBoxes[padID])
    {
      padTree.put_child("BOX_" + std::to_string(boxID), box->GetPropertyTree());
      boxID++;
    }
    
    // add axes of pad
    vector<string> axisTitles = {"X", "Y", "Z"};
    for(auto& axisTitle : axisTitles)
    {
      if(IsAxisDefined(padID, axisTitle))
      {
        padTree.put_child("AXIS_" + axisTitle, GetAxis(padID, axisTitle)->GetPropertyTree());
      }
    }
    plotTree.put_child("PAD_" + std::to_string(padID), padTree);
  }
  return plotTree;
}

} // end namespace PlottingFramework
