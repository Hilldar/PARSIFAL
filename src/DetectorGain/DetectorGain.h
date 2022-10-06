/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef DetectorGain_h
#define DetectorGain_h
#include "Common/Common.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
using namespace std;
namespace PARSIFAL{
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
		bool               Get_PrintInfo     () {return PrintInfo;};
		bool               Get_PrintNTuple   () {return PrintNTuple;};
		vector<Secondary*> Gain              (vector<Secondary*>);
		int                Get_Gain          ();
		void               Set_TuningFactor  (double io) {tuning_factor_gain[0]=io;};
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
		//Function Get
		void      Set_PrintInfo     (bool io) {PrintInfo=io;};
		void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
		void      Set_Setup         (int io)  {Setup=io;};
		void      Set_MagneticField (bool io) {MagneticField=io;};
		//Function Set
		double    Get_Setup         ()        {return Setup;}
		bool      Get_MagneticField ()        {return MagneticField;}
		double    Get_TuningFactor  ()        {return tuning_factor_gain[0];}
};
}
#endif
