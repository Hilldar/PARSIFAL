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
#include "Common/Position.h"
#include "Common/Primary.h"
#include "Common/Secondary.h"
#include "Common/ElectronicChannel.h"
#include "Common/Hit.h"
#include "Common/Cluster.h"
#include "Geometry/Geometry.h"
#include "Particle/Particle.h"
#include "Ionization/Ionization.h"
#include "ElectronDrift/ElectronDrift.h"
#include "DetectorGain/DetectorGain.h"
#include "Readout/Readout.h"
#include "Reconstruction/Reconstruction.h"
namespace PARSIFAL{

	//Function 
	void Initialize_oFile();
	void Write_oFile();
	void Clean();
	
	//Input variables
	Geometry *geo;
	Position *D4;
	Particle *party;
	Ionization *ionio;
	ElectronDrift *drift;
	DetectorGain *gain;
	Readout *anode;
	Reconstruction *recon;
	vector<Primary*> primi;
	vector<Secondary*> secondary;
	vector<Secondary*> electrons;
	vector<ElectronicChannel*> channels;
	vector<Hit*> hit;
	vector<Cluster1D*> cluster1d;
  	TFile *file;
	TTree *tree;	
	
	//Output variables
	int event;
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
	//Hits
	int            nhit;
	vector<int>    hit_ID;
	vector<int>    hit_channelID;
	vector<int>    hit_clusterID;
	vector<int>    hit_type;
	vector<double> hit_charge;
	vector<double> hit_time;
	vector<double> hit_dtime;
	vector<double> hit_positionX;
	vector<double> hit_positionY;
	//Cluster1D
	int                 ncluster1d;
	vector<int>         cluster1d_ID;
	//vector<vector<int>> *cluster1d_hitID=0;
	vector<int>         cluster1d_type;
	vector<int>         cluster1d_size;
	vector<double>      cluster1d_charge;
	vector<double>      cluster1d_positionCC_X;
	vector<double>      cluster1d_positionCC_Y;
};
#endif

