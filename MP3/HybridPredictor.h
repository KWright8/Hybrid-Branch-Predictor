#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;

class HybridPredictor {
    private:
        string gbh;
        int gshareCurrentIndex;
        int bimodalCurrentIndex;
        vector<int> gsharePredictionTable;
        vector<int> chooserTable;
        vector<int> bimodalPredictionTable;
        int totalPredictions;
        int missPredictions;
        string command;
      
        void updateBimodal(char actual, char prediction);
        void updategshare(char actual, char prediction);
        void init(int bimodalPCBits, int chooserTablePCBits, int gsharePCBits, int numgbh);
        void hybridPrediction(string branch, int k,char bimodalPrediction, char gsharePrediction);
        void printTables();
        char bimodalP1(string branch,int bimodalPCBits);
        char gshareP1(string branch, int gsharePCBits, int numgbh);
        string getBinaryString(string address);
        string xorstring(string value1, string value2);
        void updategbh(char actual);
    
     public:
        HybridPredictor(int bimodalPCBits, int chooserTablePCBits, int gsharePCBits, int numgbh, string trace_file);
};