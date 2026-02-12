#ifndef DetectorGain_h
#define DetectorGain_h
#include "Common/Common.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
using namespace std;
namespace PARSIFAL2{
	class DetectorGain{
	public:
	  //Constructur
	  DetectorGain(int setup,bool MagField);
	  //Destructor
	  ~DetectorGain() {
	    delete r;
	    if(use_function_gain) delete f_gain_eff;
	    else  delete file_gain_eff;
	  };
	  //Function
	  bool               Get_PrintInfo       () {return PrintInfo;};
	  bool               Get_PrintNTuple     () {return PrintNTuple;};
	  vector<Secondary*> Gain                (vector<Secondary*>);
	  int                Get_Gain            ();
	  void               Set_TuningFactor    (double io) {tuning_factor_gain[0]=io;};
	  void               Set_MeanDetectorGain(double io) {mean_detector_gain=io; f_gain_eff->SetParameters(io,0.85,1);};
	  int                Get_Total_Electron_Gained()   {return tot_ele_gained;};
	private:
	  // Const
	  double tuning_factor_gain[1];
	  TRandom3 *r;
	  //Variable
	  bool   PrintInfo;
	  bool   PrintNTuple;
	  int    Setup;
	  bool   MagneticField;
	  double collection_efficiency_stage1;
	  double extraction_efficiency_stage1; //is it needed? 
	  double collection_efficiency_stage2; //is it needed? 
	  double extraction_efficiency_stage2; //is it needed? 
	  double collection_efficiency_stage3; //is it needed? 
	  double extraction_efficiency_stage3; //is it needed? 
	  TString file_name,nome_g;
	  TFile *file_gain_eff;
	  TH1F*  h_gain_eff;
	  TF1*   f_gain_eff;
	  TH1I*  h_gain; //is it needed? 
	  bool   use_function_gain;
	  int    tot_ele_gained;
	  int    max_gain;
	  double mean_detector_gain;
	  //Function Get
	  void      Set_PrintInfo     (bool io) {PrintInfo=io;};
	  void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
	  void      Set_Setup         (int io)  {Setup=io;};
	  void      Set_MagneticField (bool io) {MagneticField=io;};
	  void      Set_Total_Electron_Gained(double io) {tot_ele_gained = io;};
	  //Function Set
	  double    Get_Setup         ()        {return Setup;}
	  bool      Get_MagneticField ()        {return MagneticField;}
	  double    Get_TuningFactor  ()        {return tuning_factor_gain[0];}
	};
}
#endif
