#ifndef Plot_h
#define Plot_h
#include "Plot.h"
#include "TObjArray.h"
#include "TCanvas.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace PlottingFramework {
  using std::cout;
  using std::endl;
  using std::flush;
  using std::string;
  using std::vector;
  using std::map;
  using std::set;

  using boost::property_tree::ptree;
  using boost::property_tree::write_json;
  using boost::property_tree::read_json;

  class Plot
  {
  public:
    
    typedef struct TextBox {
      double x;
      double y;
      string text;
      int borderStyle;
      int borderSize;
      int borderColor;
      int userCoordinates;
      int nColumns;
    } TextBox;
    
    typedef struct Axis {
      vector<double> range;
      string title;
    } Axis;
    
    
    struct Histogram {
      string name;
      string inputIdentifier;
      string lable;
      int marker;
      int color;
      string errorStyle; // normal, boxes, hist
      double cutoff; // todo multi-dimensional
      double cutoffLow; // todo multi-dimensional
      int lableBoxID;
      string GetUniqueName(){return name + "_@_" + inputIdentifier;};
    };
    
    struct Function {
      string name;
      string lable;
      string function;
      string fitHist;
      int color;
      int marker;
    };
    
    Plot();
    Plot(string name, string figureGroup, string plotStyle = "default") : Plot() {mName = name; mFigureGroup = figureGroup; mPlotStyle = plotStyle;}
    
    void Print();
    
    void AddGraph(string histName, string identifier = "", string lable = "", int marker = 0, int color = 0, string errorStyle = "", double cutoff = -999, double cutoffLow = -999);
    void AddHisto(string histName, string identifier = "", string lable = "", int marker = 0, int color = 0, string errorStyle = "", double cutoff = -999, double cutoffLow = -999);
    void AddRatio(string numerHist, string numerHistIdentifier, string denomHist, string denomHistIdentifier = "", string lable = "", int marker = 0, int color = 0, string errorStyle = "", double cutoff = -999, double cutoffLow = -999);
    
    void AddTextBox(double xPos, double yPos, string text = "", bool userCoord = false, int borderStyle = kSolid, int borderSize = 0, int borderColor = kBlack);
    void AddLegendBox(double xPos, double yPos, string title = "", int nColums = 1, int borderStyle = kSolid, int borderSize = 0, int borderColor = kBlack);
    
    void SetAxisTitle(string axis, string axisTitle);
    
    void SetAxisRange(string axis, double low, double high);
    void SetRatioRange(double yLow, double yHigh){}
    
    vector<double>& GetAxisRange(string axis);
    
    bool IsRatioPlot() const {return !mRatios.empty();}
    
    void ClearCutoffBin(){}
    string GetName(){return mName;}
    string GetControlString(){return mControlString;}
    string GetFigureGroup(){return mFigureGroup;}
    
    void SetDrawingProperties(string controlString){mControlString = controlString;}
    void SetPlotStyle(string plotStyle){mPlotStyle = plotStyle;}
    string GetPlotStyle(){return mPlotStyle;}
    
    vector<Histogram>& GetHistoTemplates() {return mHistos;}
    vector<Histogram>& GetRatioTemplates() {return mRatios;}
    string& GetAxisTitle(string axis);
    int GetNumSubPlots() {if(IsRatioPlot()) return 2; else return 1;}
    
    vector<TextBox>& GetLegends() {return mLegends;}
    vector<TextBox>& GetTextBoxes() {return mTexts;}
    
    vector<Histogram> mGraphs;
    vector<Histogram> mHistos;
    vector<Histogram> mRatios;
    
    ptree GetProperties();
    string& GetOutputFileName(){return mOutputFileName;}

//    void MoveToManager(PlottingFramework::PlotManager plotEnv){plotEnv.AddPlot(*this)}
  private:
    Axis& GetAxis(string axis);
    
    string mName;
    string mPlotStyle;
    string mFigureGroup;
    string mOutputFileName;

    Axis mXaxis, mYaxis, mZaxis, mYaxisRatio;
    
    string mControlString;
    bool mClearCutoffBin;
    
    vector<TextBox> mTexts;
    vector<TextBox> mLegends;
  };
}
#endif
