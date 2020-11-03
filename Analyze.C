#define Analyze_cxx
#include "Analyze.h"
#include <TH2.h>
#include <TStyle.h>

//**********Definition section**********
TH1* chi2Hist = NULL;
TH1* ebeamHist = NULL;
TH2* chi2ebeamHist = NULL;
TH1* thetaHist = NULL;
TH1* ptHist = NULL;

void Analyze::Begin(TTree * /*tree*/)
{
   TString option = GetOption();

   //**********Initialization section**********
   chi2Hist = new TH1D("chi2", "Histogram of Chi2", 100, 0, 3);
   chi2Hist->GetXaxis()->SetTitle("chi2");
   chi2Hist->GetYaxis()->SetTitle("number of events");

   // Cria um histograma chamado de ebeamHist e coloca os nomes nos eixos
   ebeamHist = new TH1D("ebeam", "Histogram of ebeam", 100, 148., 152.);
   ebeamHist->GetXaxis()->SetTitle("ebeam (GeV)");
   ebeamHist->GetYaxis()->SetTitle("number of events");

   // Cria um scatterplot para chi2 versus ebeamHist e coloca os nomes nos eixos
   chi2ebeamHist = new TH2D("chi2ebeam", "ScatterPlot of chi2 versus ebeamHist", 100, 0, 3, 100, 148., 152.);
   chi2ebeamHist->GetXaxis()->SetTitle("chi2");
   chi2ebeamHist->GetYaxis()->SetTitle("ebeam (GeV)");

   // Cria um histograma chamado ptHist com seus eixos nomeados
   ptHist = new TH1D("pt", "Histogram of pt", 100, 0, 28);
   ptHist->GetXaxis()->SetTitle("pt (GeV)");
   ptHist->GetYaxis()->SetTitle("number of events");

   // Cria um histograma chamado de thetaHist com seus respectivos eixos
   thetaHist = new TH1D("theta", "Histogram of theta", 100, -0.6, 0.6);
   thetaHist->GetXaxis()->SetTitle("theta");
   thetaHist->GetYaxis()->SetTitle("number of events");


}

void Analyze::SlaveBegin(TTree * /*tree*/){}

Bool_t Analyze::Process(Long64_t entry)
{
   // Don’t delete this line! Without it the program will crash
   fReader.SetLocalEntry(entry);

   //**********Loop section**********
   GetEntry(entry);
   chi2Hist->Fill(*chi2);
   // Preenche o ebeamHist com a variável ebeam
   ebeamHist->Fill(*ebeam);
   // Preenche o chi2ebeamHist com as variáveis ebeam e chi2 
   chi2ebeamHist->Fill(*chi2,*ebeam);

   // Calcula o pt:
   pt = TMath::Sqrt((*px)*(*px)+(*py)*(*py));
   ptHist->Fill(pt);

   // Calcula o valor de theta:
   theta = TMath::ATan2((pt),(*pz));
   thetaHist->Fill(theta);
   
   // i conta o numero de eventos
   i++;
   // j conta o numero de eventos com *pz < 145 GeV
   if (TMath::Abs(*pz)<145.) {
      std::cout << *pz << j << std::endl; 
      j++;
   }

   return kTRUE;
}

void Analyze::SlaveTerminate(){}

void Analyze::Terminate()
{
    //**********Wrap-up section**********
    // Desenha chi2Hist com barras de erros
    chi2Hist->Draw("BE1");

    //Desenha o histograma ebeam com barras de erros
    ebeamHist->Draw("BE1");
    // Faz o Fit do histrograma ebeamHist usando uma gaussiana e com barras de erros
    ebeamHist->Fit("gaus","V","BE1",148.,152.);

    // Desenha os histogramas ptHist e thetaHist 
    thetaHist->Draw();  
    ptHist->Draw(); 
    // Mostra o numero de eventos com *pz < 145 GeV
    std::cout << "The number of events with pz<145.0 GeV: " << j << std::endl;

    // Recria um arquivo chamado "histograma.root" com os histogramas criando ao longo do desenvolvimento
    TFile f("histograma.root","recreate");
    chi2Hist->Write();
    ebeamHist->Write();
    chi2ebeamHist->Write();
    ptHist->Write();
    thetaHist->Write();
    f.Write();
    f.Close();
}
