#include "main/Parsifal.h"
int nshot = 1;

using namespace PARSIFAL2;
void Run_i(string folder_i, int angle_i){
  double aangle = angle_i*TMath::DegToRad();
  ifstream infile;
  infile.open("run_list.txt", ios::out | ios::app | ios::binary);
  const int var_size = 16;
  string var[var_size];
  // 0  -> runID
  // 1  -> nEvent
  // 2  -> Gain Factor
  // 3  -> Space Diffusion Factor
  // 4  -> Time  Diffusion Factor
  // 5  -> Resistive RC [ñs]
  // 6  -> Resistive Sigma0 [mm]
  // 7  -> Mean Detector Gain
  // 8  -> Noise [fC]
  // 9  -> Threshold [fC]
  // 10 -> Ion Tail signal length [ns]
  // 11 -> Ratio signal Slow(ion) over Fast(electron)+Slow(ion)
  // 12 -> APV ped
  // 13 -> APV threshold user factor
  // 14 -> TIGER v_thr E [fC]
  // 15 -> TIGER v_thr T [fC]
  var[0]="?";
  while(infile>>var[0]>>var[1]>>var[2]>>var[3]>>var[4]>>var[5]>>var[6]>>var[7]>>var[8]>>var[9]>>var[10]>>var[11]>>var[12]>>var[13]>>var[14]>>var[15]){
    if(var[0]==folder_i) {
      nshot = stoi(var[1]);
      break;
    }
    else{
      var[0]="?";
    }
  }
  
  string name_ang = Form("run_angle%i.root",(int)angle_i);
  if(var[0]=="?") {
    cout<<"ERROR: Check the file run_list.txt"<<endl;
    return;
  }
  //if(folder_i=="1" || folder_i=="2"){
  if(false){
    /*
    cout<<"SIMULATE TRIPLE-GEM"<<endl;
    PARSIFAL *tripleGEM = new PARSIFAL();
    tripleGEM->Set_Particle_AngleXZ(aangle);
    tripleGEM->Set_OutfileName(folder_i,name_ang);
    tripleGEM->Set_nShots(nshot);
    tripleGEM->Initialization_tripleGEM();
    tripleGEM->Set_GainFactor(stod(var[2]));
    tripleGEM->Set_SpaceDiffusionFactor(stod(var[3]));
    tripleGEM->Set_TimeDiffusionFactor(stod(var[4]));
    tripleGEM->Run();
    tripleGEM->Terminate();
    delete tripleGEM;
    */
  }
  else{
    cout<<"SIMULATER µRWELL"<<endl;
    PARSIFAL *rwell = new PARSIFAL();
    rwell->Set_Particle_AngleXZ(aangle);
    rwell->Set_OutfileName(folder_i,name_ang);
    rwell->Set_nShots(stoi(var[1])); 
    rwell->Initialization_rwell();
    rwell->Set_GainFactor(stod(var[2]));
    rwell->Set_SpaceDiffusionFactor(stod(var[3]));
    rwell->Set_TimeDiffusionFactor(stod(var[4]));
    rwell->Set_TauResist(stod(var[5]));
    rwell->Set_Sigma0Resist(stod(var[6]));
    rwell->Set_MeanDetectorGain(stod(var[7]));
    rwell->Set_Noise_fC(stod(var[8]));
    rwell->Set_Threshold_fC(stod(var[9]));
    rwell->Set_IonTailLength(stod(var[10]));
    rwell->Set_FastSlowRatio(stod(var[11]));
    rwell->Set_APV_ped(stod(var[12]));
    rwell->Set_APV_thr(stod(var[13]));
    rwell->Set_TIGER_thr_E(stod(var[14]));
    rwell->Set_TIGER_thr_T(stod(var[15]));
    rwell->Run();
    rwell->Terminate(); 
    delete rwell;
  }
  return;
}


int main(int argc, char* argv[]){
  if(argc<2) {
    cout<<"RUN the analysis with the configuration ID"<<endl;
    return 0;
  }
  if(argc<3){
    string folder = argv[1];
    const int n_angle = 7;
    int angles[n_angle]={0,5,7,10,15,20,30};
    for(int iangle=0;iangle<n_angle;iangle++){
      gSystem->Exec(Form("echo RUN %s angle: %i",folder.c_str(),angles[iangle])); 
      gSystem->Exec(Form("./bin/Simulate %s %i &",folder.c_str(),angles[iangle]));
    }
  }
  else {
    string folder = argv[1];
    int angle =atoi(argv[2]);
    Run_i(folder,angle);
  }
  return 1;
};
