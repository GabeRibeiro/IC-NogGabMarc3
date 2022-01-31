#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <vector>
using namespace std;

#define N 10
typedef unordered_map<string, unordered_map<char, int>> mapa;


int getModelCSV(mapa &model, string filename){
    ifstream ri(filename);
    if(!ri){
        cerr << "Error opening file!" << endl;
        exit(0);
    }
    
    int idx, k = 0;
    string s, w, rdline;
    char c;
    while( getline(ri, rdline) ){ //ler cada linha do ficheiro 
        idx = 0;
        w="";
        for(auto x : rdline){
            if(x=='\t'){ //csv separado por tabs
                if(idx++==0) s = w; //contexto
                else c = w[0];  //caracter dps do contexto
                w = "";
            }else w += x;
        }
        k = s.length();
        model[s][c] = stoi(w); //mapear o contexto para o caracter com o numero de ocorrencias
    }
    ri.close();
    return k;
}


void getModelTxt(mapa &model, string modelFilename, int k){
    ifstream ri(modelFilename);
    if(!ri){
        cerr << "Error opening Ri file!" << endl;
        exit(0);
    }

    char c;
    vector<char> circularBuffer;
    mapa t_map;
    int fillcb, aux_cnt, total_letters=0, cb_ptr = 0, num_symbols=0;
    string rdline, aux, aux2;

    while( getline(ri, rdline) ){
        fillcb=0; //contador de elementos do buffer fica a 0 quando muda a linha
        circularBuffer.clear(); //limpa o buffer quando muda a linha
        for (int j = 0; j < rdline.size(); ++j){
            c = tolower(rdline[j]); //recolhe novo caracter
            if (!isspace(c)) //se for letra
            {   
                total_letters++; //conta o número de letras
                if(fillcb < k)// enquanto o buffer ainda não está cheio
                {   
                    circularBuffer.push_back(c); //primeiras k letras só adiciona ao buffer
                    fillcb++;
                    cb_ptr = circularBuffer.size() - 1;
                }else // quando o buffer está cheio
                {   
                    num_symbols++;
                    aux = "";
                    aux_cnt=cb_ptr;
                    
                    for (int i = 1; i<=k; i++)
                    {
                        aux_cnt = ((cb_ptr + i) % k); 
                        aux2 = string(1,circularBuffer[aux_cnt]);
                        aux = aux + aux2  ; //escreve o contexto
                    }
                    cb_ptr = ((cb_ptr + 1) % k); //incrementa o ponteiro do circular buffer
                    circularBuffer[cb_ptr] = c; //adiciona o novo caracter ao buffer
                    model[aux][c]++;
                }
            }
        }
    }
    ri.close();
}


int main(int argc, char *argv[]){

    if(argc < 4){
        puts("\nUsage: text order alpha");
        exit(1);
    }
    
    int k = atoi(argv[argc-2]);
    double alpha = stof(argv[argc-1]);
    if(alpha < 0 || alpha > 1){
        cout << "Error alpha value: 0 <= alpha <= 1" << endl;
        exit(1);
    }
    if(k < 1){
        cout << "Error order value: 1 <= order" << endl;
        exit(1);
    }

    
    string dir = "./LanguageTexts/", texts[] = {"czech.txt","danish.txt","english.txt","finnish.txt","french.txt", "italian.txt", "polish.txt", "portugues.txt", "romanian.txt", "spanish.txt"};
    mapa models[N];
    for(int i=0; i<N; i++)
        getModelTxt(models[i], dir+texts[i], k);
        


    ifstream t(argv[1]);
    if(!t){
        cerr << "Error opening file!" << endl;
        exit(1);
    }

    char c;
    vector<char> circularBuffer;
    int fillcb, aux_cnt, total_letters=0, cb_ptr = 0, num_symbols=0, nc=0;
    string rdline, aux, aux2;
    double nBits[N];

    while( getline(t, rdline) ){
        fillcb=0; //contador de elementos do buffer fica a 0 quando muda a linha
        circularBuffer.clear(); //limpa o buffer quando muda a linha
        for (int j = 0; j < rdline.size(); ++j){
            c = tolower(rdline[j]); //recolhe novo caracter
            if (!isspace(c)) //se for letra
            {   
                total_letters++; //conta o número de letras
                if(fillcb < k)// enquanto o buffer ainda não está cheio
                {   
                    circularBuffer.push_back(c); //primeiras k letras só adiciona ao buffer
                    fillcb++;
                    cb_ptr = circularBuffer.size() - 1;
                }else // quando o buffer está cheio
                {   
                    num_symbols++;
                    aux = "";
                    aux_cnt=cb_ptr;
                    
                    for (int i = 1; i<=k; i++)
                    {
                        aux_cnt = ((cb_ptr + i) % k); 
                        aux2 = string(1,circularBuffer[aux_cnt]);
                        aux = aux + aux2  ; //escreve o contexto
                    }
                    cb_ptr = ((cb_ptr + 1) % k); //incrementa o ponteiro do circular buffer
                    circularBuffer[cb_ptr] = c; //adiciona o novo caracter ao buffer

                    //para o symbolo encotrado calcular a sua probabilidade para todos os contextos 
                    for(int i=0; i<N; i++){
                        for(auto x: models[i][aux])
                            nc += x.second;
                        nBits[i] += double(-log2( (models[i][aux][c]+alpha)/( nc+alpha*60)));
                        nc = 0;
                    }
                }
            }
        }
    }
    t.close();

    cout << "Nº simbolos de " << num_symbols << endl;

    //encontrar o modelo com menor numero de bits necessarios
    int pos = 0;
    double min= -1;
    for(int i=0; i<N; i++){
        cout << "Results of model " << texts[i] << " -> bits/symbol = " << nBits[i]/num_symbols << endl;
        if(min < 0 || nBits[i] < min){
            min = nBits[i];
            pos = i;
        } 
    }

    cout << "Language of the text is the same of model " << texts[pos] << endl;

    return 0;
}