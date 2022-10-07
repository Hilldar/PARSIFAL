/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#include "main/Parsifal.h"
const int nshot = 100;

using namespace PARSIFAL;
void Run_i(string folder_i, int angle_i){
  double aangle = angle_i*TMath::DegToRad();
  ifstream infile;
  infile.open("run_list.txt", ios::out | ios::app | ios::binary);
  string var[4];
  while(infile>>var[0]>>var[1]>>var[2]>>var[3]) if(var[0]==folder_i) break;
  string name_ang = Form("run_angle%i.root",(int)angle_i);
  PARSI *tripleGEM = new PARSI();
  tripleGEM->Set_Particle_AngleXZ(aangle);
  tripleGEM->Set_OutfileName(folder_i,name_ang);
  tripleGEM->Set_nShots(nshot);
  tripleGEM->Initialization_tripleGEM();
  tripleGEM->Set_GainFactor(stod(var[1]));
  tripleGEM->Set_SpaceDiffusionFactor(stod(var[2]));
  tripleGEM->Set_TimeDiffusionFactor(stod(var[3]));
  tripleGEM->Run();
  tripleGEM->Terminate();
  delete tripleGEM;
  return;
}


int main(int argc, const char* argv[]){
  if(argc<2) {
    cout<<"RUN the analysis with the configuration ID"<<endl;
    return 0;
  }
  if(argc<3){
    string folder = argv[1];
    int angles[7]={0,5,10,15,20,30,40};
    for(int iangle=0;iangle<7;iangle++){
      Run_i(folder,angles[iangle]);
    }
  }
  else {
    string folder = argv[1];
    int angle =atoi(argv[2]);
    Run_i(folder,angle);
  }
  return 1;
};
