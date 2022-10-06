/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef Main_h
#define Main_h
#include "Common/Common.h"
#include "Geometry/Geometry.h"
#include "Particle/Particle.h"
#include "Ionization/Ionization.h"
#include "ElectronDrift/ElectronDrift.h"
#include "DetectorGain/DetectorGain.h"
#include "Readout/Readout.h"
namespace PARSIFAL{

	//Function 
	void Initialize_oFile();
	void Write_oFile();
	
	//Input variables
	Geometry *geo;
	Position *D4;
	Particle *party;
	Ionization *ionio;
	ElectronDrift *drift;
	DetectorGain *gain;
	Readout *anode;
	vector<Primary*> primi;
	vector<Secondary*> secondary;
	vector<Secondary*> electrons;
	vector<ElectronicChannel*> channels;
  	TFile *file;
	TTree *tree;	
	
	//Output variables
	//Geometry
	int    geo_numview;
	double geo_pitch1;
	double geo_pitch2;
	double geo_driftgap;
	//Particle
	int    party_partID;
	double party_angleXZ;
	double party_angleYZ;
	double party_pos[4];
	double party_primary;
	double party_prob[100];
	//Ionization
	double ionio_positionI[4];
	double ionio_positionF[4];
	int    ionio_primary;
	int    ionio_secondary; 
	// Primary
	vector<double> primary_positionX;
	vector<double> primary_positionY;
	vector<double> primary_positionZ;
	vector<double> primary_positionT;
	vector<int>    primary_secondary;
	vector<int>    primary_secondary_gain;
	vector<int>    primary_ID;
	//Secondary
	vector<int>    secondary_gain;
	//Electrons
	vector<double> electron_positionX_initial;
	vector<double> electron_positionY_initial;
	vector<double> electron_positionZ_initial;
	vector<double> electron_positionT_initial;
	vector<double> electron_positionX_final;
	vector<double> electron_positionY_final;
	vector<double> electron_positionZ_final;
	vector<double> electron_positionT_final;
	vector<int>    electron_primary_ID;
	vector<int>    electron_electronic_channel;
	//Electronic Channel
	vector<int>    channel_ID;
	vector<int>    channel_type;
	vector<double> channel_charge;
	vector<double> channel_time;
	vector<double> channel_dtime;
	vector<double> channel_positionX;
	vector<double> channel_positionY;
};
#endif

