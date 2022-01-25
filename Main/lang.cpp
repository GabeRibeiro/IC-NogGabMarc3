#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <vector>
using namespace std;

int main(int argc, char *argv[]){

    if(argc != 5){
        puts("\nUsage: path_ri path_text order alpha");
        exit(1);
    }
    
    ifstream ri(argv[1]);
    ifstream t(argv[2]);
    int nbits, order = atoi(argv[3]), alpha = atoi(argv[4]);
    string rdline;

    //ler modelo para um mapa
    if(!ri){
        cerr << "Error opening Ri file!" << endl;
        exit(0);
    }
    int total=-1;
    unordered_map<string, unordered_map<char, int>> model;

    while( getline(ri, rdline) ){
        
    }

    //fazer modelo 
    if(!t){
        cerr << "Error opening t file!" << endl;
        exit(1);
    }
    vector<char> circularBuffer;
    unordered_map<string, unordered_map<char, int>> model;
    

    while( getline(t, rdline) ){
        
        rdline.split();
    }

    cout << "Estimated number bits required to compress the text is " << nbits << endl;
    
    ri.close();
    t.close();
    return 0;
}