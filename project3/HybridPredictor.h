#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;

class HybridPredictor {

    public:
        // hybrid
        vector<int> chooserTable;

        // bimodal
        vector<int> bimodalPredictionTable;
        int bimodalCurrentIndex;
        void updateBimodal(char actual, char prediction);
   

        // ghsare
        string gbh;
        vector<int> gsharePredictionTable;
        int gshareCurrentIndex;
        void updategshare(char actual, char prediction);

        HybridPredictor(int bimodalPCBits, int chooserTablePCBits, int gsharePCBits, int numgbh, string trace_file);
        void init(int bimodalPCBits, int chooserTablePCBits, int gsharePCBits, int numgbh);
        void hybridPrediction(string branch, int k,char bimodalPrediction, char gsharePrediction);
        void printTables();
        char bimodalP1(string branch,int bimodalPCBits);
        char gshareP1(string branch, int gsharePCBits, int numgbh);
        string getBinaryString(string address);
        string xorstring(string value1, string value2);
        void updategbh(char actual);
        string command;

        int totalPredictions;
        int missPredictions;  
};