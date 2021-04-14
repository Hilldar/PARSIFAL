#include "DetectorGain/DetectorGain.h"
namespace PARSIFAL2{
  DetectorGain::DetectorGain(int setup,bool MagField):
    Setup(setup),
    MagneticField(MagField),
    PrintInfo(true),
    PrintNTuple(true)
  {
    tuning_factor_gain[0] = {1};
    r = new TRandom3();
    r->SetSeed(SEED);
    if(Get_Setup()==0){
      collection_efficiency_stage1=0.9574;
      extraction_efficiency_stage1=0.40487;
      collection_efficiency_stage2=0.729592;
      extraction_efficiency_stage2=0.400875;
      collection_efficiency_stage3=0.718182;
      extraction_efficiency_stage3=0.515696;
      if(Get_MagneticField()) file_name="guadagni_10M_magfield.root";
      else file_name="guadagni_10M.root";			
      file_gain_eff = new TFile("src/DetectorGain/guadagni_10M.root", "READ");
      h_gain_eff = (TH1F*) file_gain_eff->Get("h_gain_eff_0_825");
    }
    if(Get_Setup()!=0){
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
      cout<<"<>-<>      DetectorGain     <>-<>"<<endl;
      cout<<"<>-<>  Check the setup ID   <>-<>"<<endl;
      cout<<"<>-<>  <>-<>  <>-<>  <>-<>  <>-<>"<<endl;
    }
    if(Get_PrintInfo()){
      cout<<"---- New DetectorGain ----"<<endl;
      cout<<"Gain configuration : "<<Get_Setup()<<endl;
      cout<<"Gain tuning factor : "<<Get_TuningFactor()<<endl;
      cout<<"----------------------"<<endl;
    }
  };
  vector<Secondary*> DetectorGain::Gain(vector<Secondary*> pre_gain){
    vector<Secondary*> post_gain = pre_gain;
    if(post_gain.empty()) return post_gain;
    for(int iele=0;iele<post_gain.size();iele++){
      //check if the electron reaches the first multiplication stage
      if(r->Rndm()>collection_efficiency_stage1){
	post_gain.at(iele)->Set_GainedElectrons(0);
      }
      else{
	//Multiply the electrons number due to the detector gain
	int tot_gain = h_gain_eff->GetRandom();
	tot_gain = (int) (tot_gain*Get_TuningFactor());
	if(NO_Gain) tot_gain = 1;
	post_gain.at(iele)->Set_GainedElectrons(tot_gain);
      }
    }
    return post_gain;
  };	

  int DetectorGain::Get_Gain(){
    //check if the electron reaches the first multiplication stage                                                                                                       
    if(r->Rndm()>collection_efficiency_stage1){
      return 0;
    }
    else{
      //Multiply the electrons number due to the detector gain                                                                                                           
      int tot_gain = h_gain_eff->GetRandom();
      tot_gain = (int) (tot_gain*Get_TuningFactor());
      if(NO_Gain) tot_gain = 1;
      return tot_gain;
    }
  }
}

