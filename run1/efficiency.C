#include <iostream>
#include <tuple>
#include <vector>

#include <TCanvas.h>
#include <TEfficiency.h>
#include <TFile.h>

struct ParticleInfo{
double id;
double theta;
//double phi;
//double pT;
//double p;

};

void efficiency(){
  gStyle->SetOptFit(0011);
  gStyle->SetOptStat(0000);
  gStyle->SetPadLeftMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  //gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetTitleSize(0.05, "xy");
  gStyle->SetLabelSize(0.04, "xy");
  gStyle->SetTitleOffset(1.2, "x");
  gStyle->SetTitleOffset(3, "y");
  gStyle->SetNdivisions(510, "y");
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TFile* file1 = TFile::Open("FaserMC-MC22_FS_Aee_050MeV_1Em6-110023-00010-00019-s0008_3station_backward_seed_summary_circleFitTrackSeedTool.root", "read");
  TFile* file2 = TFile::Open("FaserMC-MC22_FS_Aee_050MeV_1Em6-110023-00010-00019-s0008-PHYS.root","read");

  TTreeReader* Reader1 = new TTreeReader("tree",file1);
  TTreeReader* Reader2 = new TTreeReader("nt",file2);
  TTree* t1 = Reader1->GetTree();
  TTree* t2 = Reader2->GetTree();
  
  TEfficiency* trackEff_vs_theta = new TEfficiency("eff","trackeff_vs_theta;Truth cos#theta;Efficiency",12, -1, 1);
  //TEfficiency* trackEff_vs_pT = new TEfficiency("eff","trackeff_vs_pt;Truth pT;Efficiency",12, -1, 1);
  std::vector<float> *v1 = nullptr;
  std::vector<double> *v2 = nullptr;
  std::vector<uint64_t> *v3 = nullptr;

  std::vector<double> *v4 = nullptr;
  std::vector<unsigned int> *v5 = nullptr;

  //All particles  
  t2->SetBranchAddress("truth_barcode",&v1);
  t2->SetBranchAddress("truth_theta",&v2);

  //All reco tracks
  t1->SetBranchAddress("majorityParticleId",&v3);
  t1->SetBranchAddress("t_truthHitRatio", &v4);
  t1->SetBranchAddress("nMeasurements", &v5);

  t1->AddFriend(t2);

  size_t entries = t1->GetEntries();
  std::cout<<"entries: "<<entries<<std::endl;
  std::vector<ParticleInfo> particles;

  //store truthParticleBarcode into vector;
  std::vector<int> particleIdWithTracks; //majorityParticleId

  for(size_t ievent = 0; ievent<entries; ievent++){
    t1->GetEntry(ievent);

    // loop over all tracks
    for(int iTrack = 0; iTrack < v3->size(); ++iTrack){
      if((v4->at(iTrack)) < 0.6) continue;
      if((v5->at(iTrack)) < 12)continue;
      particleIdWithTracks.push_back(v3->at(iTrack));
    } 


    // loop over all particles 
    for(int iparticle = 0; iparticle < v1->size(); ++iparticle){
      particles.push_back(  {v1->at(iparticle), v2->at(iparticle)} );

      //if (nTruthHits < 18 ) continue;
  

      //Fill TEfficiency
      bool hasTrack = false;
      if(std::find(particleIdWithTracks.begin(), particleIdWithTracks.end(), v1->at(iparticle)) != particleIdWithTracks.end()){ 
        hasTrack = true;
      }
      trackEff_vs_theta->Fill(hasTrack,cos(v2->at(iparticle)));
    }
  }
  
  std::cout<<"All good.Now plotting..."<<std::endl;
  /*TLegend* legend = new TLegend(0.7, 0.8, 0.9, 0.82);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  legend->SetTextFont(72);
  legend->AddEntry(trackEff_vs_theta,"trackEff_vs_theta","l");*/
  TCanvas* cs = new TCanvas("c1","efficiency",600,500);
  //TCanvas* cs = new TCanvas("c1","trackeff_vs_pT",600,500);
  //cs->Divide(1,2);
  cs->cd();
  trackEff_vs_theta->Draw();
  //cs->cd(2);
 // trackEff_vs_pT->Draw();
 // legend->Draw();
  


}
