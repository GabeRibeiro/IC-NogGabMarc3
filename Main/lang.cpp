#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

typedef unordered_map<string, unordered_map<char, int>> mapa;

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

void getModelCSV(mapa &model, string modelFilename){
    ifstream ri(modelFilename);
    if(!ri){
        cerr << "Error opening Ri file!" << endl;
        exit(0);
    }
    
    int idx;
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
        model[s][c] = stoi(w); //mapear o contexto para o caracter com o numero de ocorrencias
    }
    ri.close();
}



int main(int argc, char *argv[]){

    if(argc != 5){
        puts("\nUsage: path_ri path_text order alpha");
        exit(1);
    }
    
    int k = atoi(argv[3]);
    double alpha = stof(argv[4]);
    if(alpha < 0 || alpha > 1){
        cout << "Error alpha value: 0 <= alpha <= 1" << endl;
        exit(1);
    }
    if(k < 1){
        cout << "Error order value: 1 <= order" << endl;
        exit(1);
    }

    mapa model;
    //getModelCSV(model, argv[1]);
    getModelTxt(model, argv[1], k);


    //fazer o mapa do texto a analisar tendo em conta os valores do modelo
    ifstream t(argv[2]);
    if(!t){
        cerr << "Error opening t file!" << endl;
        exit(1);
    }

    //teste modelo
    /*
    cout << "mapa do Modelo : " << endl;
    for(auto x: model){
        for(auto y: x.second){
            cout << "ctx : " << x.first << ", next char : " << y.first << ", ocorrencias : " << y.second << endl;
        }
    }*/




    char c;
    vector<char> circularBuffer;
    mapa t_map;
    int fillcb, aux_cnt, total_letters=0, cb_ptr = 0, num_symbols=0;
    string rdline, aux, aux2;

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

                    if(model[aux].count(c)!=0) t_map[aux][c] = model[aux][c]; //contexto seguido do char esta no modelo
                    else t_map[aux][c] = 0; //contexto seguido do char nao aparece no modelo
                }
            }
        }
    }
    t.close();


    
    int total=0, tc_aux;
    vector<int> occur, total_ctx;
    vector<double> h_ctx, pi, hi;
    double model_entropy=0, acum=0;

    for(auto a1: t_map){
        tc_aux = 0;

        //cout << "\nctx : " << a1.first << " -> occur [";
        for(auto a2: a1.second){
            occur.push_back(a2.second);
            //cout << a2.first << ":" << a2.second << ", ";
            tc_aux += a2.second;
        }
        total += tc_aux;
        //cout << "], total_ctx = " << tc_aux;

        total_ctx.push_back(tc_aux); //total de ocurrencias no contexto

        //cout << "\npi  ";
        //probabilidade de cada elem do contexto
        for(int i=0; i<occur.size(); i++){
            pi.push_back( double((occur[i]+alpha)/(tc_aux+alpha*26)) );
            //cout << pi.back() << ", ";
        }//cout << "\nhi  ";
        for(int i=0; i<pi.size(); i++){
            hi.push_back( double(pi[i]*log2(pi[i])) );
            //cout << hi.back() << ", ";
        }//cout << endl;
        
        for(int i=0; i<hi.size(); i++)
            acum+= hi[i];
        h_ctx.push_back( -acum ); //entropia do contexto
        //cout << "entropy_ctx = " << h_ctx.back() << endl;


        occur.clear();
        hi.clear();
        pi.clear();
    }
    //entropia total do modelo
    for(int i=0; i<h_ctx.size(); i++) 
        model_entropy += double(h_ctx[i]*total_ctx[i]/total);

    cout << "\nEntropia modelo = " << model_entropy << ", n symbols = " << num_symbols << endl;
    cout << "\nEstimated number bits required to compress the text is " << model_entropy*num_symbols << endl;
    

    return 0;
}