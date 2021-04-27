#include "main/Parsifal.h"
bool run_parallel = true;
const int nshot = 100;//2000;//10000;

using namespace PARSIFAL2;
void Run_i(string folder_i, int angle_i){
  double aangle = angle_i*TMath::DegToRad();
  ifstream infile;
  infile.open("run_list.txt", ios::out | ios::app | ios::binary);
  string var[4];
  while(infile>>var[0]>>var[1]>>var[2]>>var[3]) if(var[0]==folder_i) break;
  string name_ang = Form("run_angle%i.root",(int)angle_i);
  /*
  PARSIFAL *tripleGEM = new PARSIFAL();
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
  */
  PARSIFAL *rwell = new PARSIFAL();
  rwell->Set_Particle_AngleXZ(aangle);
  rwell->Set_OutfileName(folder_i,name_ang);
  rwell->Set_nShots(nshot);
  rwell->Initialization_rwell();
  rwell->Set_GainFactor(stod(var[1]));
  rwell->Set_SpaceDiffusionFactor(stod(var[2]));
  rwell->Set_TimeDiffusionFactor(stod(var[3]));
  rwell->Run(); 
  rwell->Terminate(); 
  delete rwell;
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
    //for(int iangle=0;iangle<=40;iangle+=10){
    for(int iangle=0;iangle<7;iangle++){
      if(run_parallel){
	//gSystem->Exec(Form("ts $exe_parsifal2 -S %i %i",stoi(folder),iangle));
	gSystem->Exec(Form("ts $exe_parsifal2 -S %i %i",stoi(folder),angles[iangle]));
	gSystem->Exec(Form("sleep 0.1s"));
      }
      else{
	//Run_i(folder,iangle);
	Run_i(folder,angles[iangle]);
      }
    }
  }
  else {
    string folder = argv[1];
    int angle =atoi(argv[2]);
    Run_i(folder,angle);
  }

  return 1;
};
