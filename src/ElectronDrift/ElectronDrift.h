/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef ElectronDrift_h
#define ElectronDrift_h
#include "Geometry/Geometry.h"
#include "Common/Common.h"
#include "Common/Position.h"
#include "Common/Secondary.h"
using namespace std;
namespace PARSIFAL2{
  class ElectronDrift{
	public:
	  //Constructur
	  ElectronDrift(int setup, bool MagField, Geometry* geo);
	  //Destructor
	  ~ElectronDrift() {
	    delete r;
	  };
	  //Function
	  bool               Get_PrintInfo     () {return PrintInfo;};
	  bool               Get_PrintNTuple   () {return PrintNTuple;};
	  Position*          Get_FinalPosition (Position *pos_init);
	  vector<Secondary*> Drift             (vector<Secondary*> primary);
	  void Set_SpaceTuningFactor (double io)    {tuning_factor_diff=io;};
	  void Set_TimeTuningFactor  (double io)    {tuning_factor_time=io;};
          double    Get_Drift_X       (double z);
          double    Get_Drift_Y       (double z);
          double    Get_Drift_T       (double z);

	private:
	  //Const 
	  double    tuning_factor_diff;
	  double    tuning_factor_time;
	  TRandom3 *r;
	  //Variable
	  bool      PrintInfo;
	  bool      PrintNTuple;
	  int 	  Setup;
	  bool      MagneticField;
	  Geometry* geometry;
	  //Without mag field
	  double    SpatialShiftDrift[2];         //mm
	  double    SpatialShiftTrans;            //mm
	  double    SpatialShiftInduc;            //mm
	  double    TemporalShiftDrift[2];        //ns
	  double    TemporalShiftTrans;           //ns
          double    TemporalShiftInduc;           //ns
	  double    SpatialDiffusionDrift[4];     //mm
	  double    TemporalDiffusionDrift[4];    //ns
	  double    SpatialDiffusionTrans;        //mm
	  double    SpatialDiffusionInduc;        //mm
	  double    TemporalDiffusionTrans;       //ns
	  double    TemporalDiffusionInduc;       //ns
	  //With mag field
	  double    SpatialShiftDriftMagField[2];         //mm
	  double    SpatialShiftTransMagField;            //mm
	  double    SpatialShiftInducMagField;            //mm
	  double    TemporalShiftDriftMagField[2];        //ns
	  double    TemporalShiftTransMagField;           //ns
	  double    TemporalShiftInducMagField;           //ns
	  double    SpatialDiffusionDriftMagField[4];     //mm
	  double    TemporalDiffusionDriftMagField[4];    //ns
	  double    SpatialDiffusionTransMagField;    	  //mm
	  double    SpatialDiffusionInducMagField;    	  //mm
	  double    TemporalDiffusionTransMagField;   	  //ns
	  double    TemporalDiffusionInducMagField;   	  //ns

	  //Function Set
	  void      Set_PrintInfo     (bool io) {PrintInfo=io;};
	  void      Set_PrintNTuple   (bool io) {PrintNTuple=io;};
	  void      Set_Setup         (int io)  {Setup=io;};
	  void      Set_MagneticField (bool io) {MagneticField=io;};
	  //Function Get
	  double    Get_Setup         ()        {return Setup;}
	  bool      Get_MagneticField ()        {return MagneticField;}
	  double    Get_SpaceTuningFactor ()    {return tuning_factor_diff;};
          double    Get_TimeTuningFactor  ()    {return tuning_factor_time;};
	  double    Get_X             (double x, double z);
	  double    Get_Y             (double y, double z);
	  double    Get_Z             ();
	  double    Get_T             (double t, double z);
	};
}
#endif
