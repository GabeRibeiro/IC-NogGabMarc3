#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <vector>
using namespace std;

int main(int argc, char *argv[]){

    if(argc != 2){
        puts("\nUsage: path_text");
        exit(1);
    }
    
    string *langModels = {"portugues.txt", "frances.txt", "espanhol.txt", "ingles.txt", "italiano.txt"};
    ifstream txt = open(argv[1]);
    string lang = "";


    cout << "Language of the text is " << lang << endl;

    return 0;
}