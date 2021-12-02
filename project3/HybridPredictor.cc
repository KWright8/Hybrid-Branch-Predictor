#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <string>
#include <math.h> 
#include "HybridPredictor.h"

using namespace std;

HybridPredictor::HybridPredictor(int bimodalPCBits, int chooserTablePCBits, int gsharePCBits, int numgbh, string trace_file) { 
    char bimodalPrediction;
    char gsharePrediction;
    totalPredictions = 0;
    missPredictions = 0;

    command = "./sim hybrid ";
    command += to_string(chooserTablePCBits);
    command += " ";
    command += to_string(gsharePCBits);
    command += " ";
    command += to_string(numgbh);
    command += " ";
    command += to_string(bimodalPCBits);
    command += " ";    
    command += trace_file.c_str();

    init(bimodalPCBits, chooserTablePCBits, gsharePCBits, numgbh);

    // open file
    ifstream myfile;
    myfile.open (trace_file);
    string line;

    int i = 1;
    while ( getline (myfile,line) )
    {
        totalPredictions++;
        bimodalPrediction = bimodalP1(line, bimodalPCBits);
        gsharePrediction = gshareP1(line, gsharePCBits, numgbh);
        i++;

        hybridPrediction(line, chooserTablePCBits, bimodalPrediction, gsharePrediction);

    }
    printTables();
}

void HybridPredictor::hybridPrediction(string branch, int k, char bimodalPrediction, char gsharePrediction) {
    char actual;
    string address;
    for (int i  = 0; i < branch.length(); i++){
        if (isalpha(branch.at(i))|| isdigit(branch.at(i))){
            if( branch.at(i) == 'n' or branch.at(i) == 't') {
                actual = branch.at(i);
            } else {
                address.push_back(branch.at(i));
            }
        }
    }

    string binaryAddress =  getBinaryString(address);
    binaryAddress.pop_back();
    binaryAddress.pop_back();

    string temp;
    int adIndex = (binaryAddress.length() - 1);
    for (int i  = 0; i < k; i++) {
        temp = binaryAddress.at(adIndex) + temp;
        adIndex--;
    }

    int index = std::stoull(temp, NULL,2);

    if (chooserTable.at(index) >= 2) {
        // gshare
        updategshare(actual,gsharePrediction);
       
    } else {
            // bimodal
            updateBimodal(actual,bimodalPrediction);
    }

    // update chooser table
    if (bimodalPrediction == actual && gsharePrediction != actual){
        if(chooserTable.at(index) > 0) {
            int inter =  chooserTable.at(index) - 1;
            chooserTable.at(index) = inter;
        }
    }  

    if (bimodalPrediction != actual && gsharePrediction == actual) {
        if(chooserTable.at(index) < 3) {
            int interadd =  chooserTable.at(index) + 1;
            chooserTable.at(index) =  interadd;
        }  
    }  
}

void HybridPredictor::init(int bimodalPCBits, int chooserTablePCBits, int gsharePCBits, int numgbh) {
    // init chooser table
    chooserTable.assign(pow(2.0, (float)chooserTablePCBits), 1);

    // init bimodal prediction table 
    bimodalPredictionTable.assign(pow(2.0, (float)bimodalPCBits), 4);

    // intit gsharePredictionTable
    gsharePredictionTable.assign(pow(2.0, (float)gsharePCBits), 4);

    // int gbh
    for (int i = 0; i < numgbh; i++){
        gbh.push_back('0');
    }
}

char HybridPredictor::bimodalP1(string branch, int bimodalPCBits) {
    char prediction;
    char actual;
    string address;

    for (int i  = 0; i < branch.length(); i++){
        if (isalpha(branch.at(i))|| isdigit(branch.at(i))){
            if( branch.at(i) == 'n' or branch.at(i) == 't') {
                actual = branch.at(i);
            } else {
                address.push_back(branch.at(i));
            }
        }
    }

    string binaryAddress =  getBinaryString(address);

    binaryAddress.pop_back();
    binaryAddress.pop_back();

    string temp;
    int in = (binaryAddress.length() - 1);

    for (int i  = 0; i < bimodalPCBits; i++) {

        temp = binaryAddress.at(in) + temp;
        in--;
    }

    int index = std::stoull(temp, NULL,2);
    bimodalCurrentIndex = index;

    if (bimodalPredictionTable.at(index) >= 4){
        prediction = 't';
        
    } else {
        prediction = 'n';
    }  
    
    return prediction;
}

char HybridPredictor::gshareP1(string branch, int gsharePCBits, int numgbh) {
    string address;
    char actual;
    char prediction;
    int numMSB = gsharePCBits - numgbh;

    for (int i  = 0; i < branch.length(); i++){
        if (isalpha(branch.at(i))|| isdigit(branch.at(i))){
            if( branch.at(i) == 'n' or branch.at(i) == 't') {
                actual = branch.at(i);
            } else {
                address.push_back(branch.at(i));
            }
        }
    }
    
    string binaryAddress =  getBinaryString(address);
    binaryAddress.pop_back();
    binaryAddress.pop_back();

    string temp;
    int in = (binaryAddress.length() - 1);

    for (int i  = 0; i < gsharePCBits; i++) {
        temp = binaryAddress.at(in) + temp;
        in--;
    }

    string MSB;
    for (int i = 0; i < numMSB; i++) {
        MSB.push_back(temp.at(i));
    }

    string LSB;
    for (int i = 1; i <= numgbh; i++) {
        LSB = temp.at((temp.length()) - i) + LSB;
    }

    string xorvalue = xorstring(gbh, LSB);
    string xortemp; 
   
    if (xorvalue.size() > numgbh){
        int numRemove = xorvalue.size() - numgbh;
        int ind =  numRemove;
        for (int i = 0; i < numgbh; i++) {
            xortemp = xortemp + xorvalue.at(ind) ;
            ind = ind + 1;
        } 
    } else {

        if (xorvalue.size() < numgbh){
            int numz =  numgbh - xorvalue.size();
            string temp;
            for (int i = 0; i < numz; i++) {
                temp = temp + "0";
            }
            xortemp =  temp + xorvalue;
        } else {
            xortemp = xorvalue;
        }
    } 

    string m_Index = MSB + xortemp;
    int index = std::stoull(m_Index, NULL,2);
    gshareCurrentIndex =  index;

    if (gsharePredictionTable.at(index) >= 4){
        prediction = 't';
        
    } else {
        prediction = 'n';
    }   

    return prediction;
}

void HybridPredictor::updateBimodal(char actual, char prediction) {
    if (prediction != actual){
        missPredictions++;    
    } 

    if (actual == 'n') {
        if(bimodalPredictionTable.at(bimodalCurrentIndex) > 0) {
            int inter =  bimodalPredictionTable.at(bimodalCurrentIndex) - 1;
            bimodalPredictionTable.at(bimodalCurrentIndex) = inter;
        }
    } else {
        if(bimodalPredictionTable.at(bimodalCurrentIndex) < 7) {
            int interadd =  bimodalPredictionTable.at(bimodalCurrentIndex) + 1;
            bimodalPredictionTable.at(bimodalCurrentIndex) =  interadd;
        }  
    } 

    updategbh(actual); 
}

string HybridPredictor::getBinaryString(string address){
    string temp;
    for (int i = 0; i < address.length(); i++) {
        switch(address.at(i)) {
            case '0':
                temp.append("0000");
                break;
            case '1':
                temp.append("0001");
                break;
            case '2':
                temp.append("0010");
                break;
            case '3':
                temp.append("0011");
                break;
            case '4':
                temp.append("0100");
                break;
            case '5':
                temp.append("0101");
                break;
            case '6':
                temp.append("0110");
                break;
            case '7':
                temp.append("0111");
                break;
            case '8':
                temp.append("1000");
                break;
            case '9':
                temp.append("1001");
                break;
            case 'A':
            case 'a':
                temp.append("1010");
                break;
            case 'B':
            case 'b':
                temp.append("1011");
                break;
            case 'C':
            case 'c':
                temp.append("1100");
                break;
            case 'D':
            case 'd':
                temp.append("1101");
                break;
            case 'E':
            case 'e':
                temp.append("1110");
                break;
            case 'F':
            case 'f':
                temp.append("1111");
                break;
            default:
                printf("OH NO");
        }
    }
    return temp;
}

string HybridPredictor::xorstring(string value1, string value2) {
    // convert to int
    int v1 = std::stoull(value1, NULL,2);
    int v2 = std::stoull(value2, NULL,2);
    int xor1 = v1^v2;

    stringstream ss;
    ss << std::hex << xor1; // int decimal_value
    std::string res ( ss.str() );
    string final_value = res;

    return getBinaryString( final_value);
}

void HybridPredictor::printTables(){
    printf("COMMAND\n");
    printf("%s\n", command.c_str());
    printf("OUTPUT\n");
    printf("number of predictions:		%d\n", totalPredictions);
    printf("number of mispredictions:	%d\n", missPredictions);
    printf("misprediction rate:		%.2f%%\n", (((float)missPredictions/(float)totalPredictions)*100));
    printf("FINAL CHOOSER CONTENTS\n");

    for (int i = 0; i < chooserTable.size(); i++){
        printf("%d   %d\n",i, chooserTable.at(i));
    }

    printf("FINAL GSHARE CONTENTS\n");
    for (int i = 0; i < gsharePredictionTable.size(); i++){
        printf("%d   %d\n",i, gsharePredictionTable.at(i));
    }

    printf("FINAL BIMODAL CONTENTS\n");
    for (int i = 0; i < bimodalPredictionTable.size(); i++){
        printf("%d   %d\n",i, bimodalPredictionTable.at(i));
    }

}

void HybridPredictor::updategshare(char actual, char prediction){   
    if (prediction != actual){
        missPredictions++; 
    }

    if (actual == 'n') {
        if(gsharePredictionTable.at(gshareCurrentIndex) > 0) {
            int inter =  (gsharePredictionTable.at(gshareCurrentIndex) - 1);
            gsharePredictionTable.at(gshareCurrentIndex) = inter;
        }
    } else {
        if(gsharePredictionTable.at(gshareCurrentIndex) < 7) {
            int interadd =  (gsharePredictionTable.at(gshareCurrentIndex) + 1);
            gsharePredictionTable.at(gshareCurrentIndex) =  interadd;
        }  
    } 

    updategbh(actual);
}

void HybridPredictor::updategbh(char actual) {
    string outcome;
    if (actual == 'n') {
        outcome =  outcome + "0"; 
        for (int i = 0; i < (gbh.length()-1); i++) {
            outcome = outcome + gbh.at(i);
        }
    } else {
        outcome =  outcome + "1"; 
            for (int i = 0; i < (gbh.length()-1); i++) {
            outcome = outcome + gbh.at(i);
        }
    } 
    gbh =  outcome;
}


