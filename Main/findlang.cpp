#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <vector>
using namespace std;

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


int getModelTxt( mapa &t_map, string filename, int k){
    ifstream t(filename);
    if(!t){
        cerr << "Error opening file!" << endl;
        exit(1);
    }

    char c;
    vector<char> circularBuffer;
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

                    t_map[aux][c] = 0; //inicializar a zero
                }
            }
        }
    }
    t.close();
    return num_symbols;
}


void joinModelTxt(mapa &mdl, mapa &txt, mapa &result){
    for(auto x: txt){
        for(auto y: x.second){
            result[x.first][y.first] = mdl[x.first].count(y.first)!=0? mdl[x.first][y.first] : 0;
        }
    }
}


int getCompressBits(mapa &t_map, int num_symbols, double alpha){
    int total=0, tc_aux;
    vector<int> occur, total_ctx;
    vector<double> h_ctx, pi, hi;
    double model_entropy=0, acum=0;

    for(auto a1: t_map){
        tc_aux = 0;

        for(auto a2: a1.second){
            occur.push_back(a2.second);
            tc_aux += a2.second;
        }
        total += tc_aux;
        total_ctx.push_back(tc_aux); 

        for(int i=0; i<occur.size(); i++)
            pi.push_back( double((occur[i]+alpha)/(tc_aux+alpha*26)) );
        for(int i=0; i<pi.size(); i++)
            hi.push_back( double(pi[i]*log2(pi[i])) );
        for(int i=0; i<hi.size(); i++)
            acum+= hi[i];
        h_ctx.push_back( -acum ); 
        occur.clear();
        hi.clear();
        pi.clear();
    }
    
    for(int i=0; i<h_ctx.size(); i++) 
        model_entropy += double(h_ctx[i]*total_ctx[i]/total);
    return model_entropy*num_symbols;
}



int main(int argc, char *argv[]){

    if(argc < 5){
        puts("\nUsage: model1 model2 [modelx, ...] text alpha");
        exit(1);
    }
    
    int N = argc-3, k = -1, ki;
    cout << "argc = " << argc << ", N = " << N << endl;

    mapa models[N], results[N];
    for(int i=0; i<N; i++){
        ki = getModelCSV(models[i], argv[i+1]);
        //verificaçoes do valor da ordem dos modelos
        if(k < 0) k = ki;
        else if(k != ki){
            cout << "Diferentes valores de k entre modelos." << endl;
            exit(2);
        }
        cout << "\n\nModelo " << i+1 << endl;
        /*
        for(auto x: models[i]){
            for(auto y: x.second){
                cout << "ctx: " << x.first << ", char: " << y.first << ", ocur: " << y.second << endl;
            }
        }*/
    }
        

    
    mapa model_txt;
    int num_symbols = getModelTxt(model_txt, argv[N+1], k);
    double alpha = stof(argv[argc-1]);
    /*
    cout << "\nModelo do texto: " << endl;
    for(auto x: model_txt){
        for(auto y: x.second)
            cout << "ctx: " << x.first << ", char: " << y.first << endl;
    }*/
    cout << "Numero de symbols do texto = " << num_symbols <<endl;
    cout << "alpha = " << alpha <<endl;


    int pos = 0;
    double minBits = -1, resBits;
    for(int i=0; i<N; i++){
        cout << "\nResults " << i;
        joinModelTxt(models[i], model_txt, results[i]);
        resBits = getCompressBits(results[i], num_symbols, alpha);
        cout << " -> num bits min = " << resBits << endl;
        if(minBits < 0 || resBits < minBits){
            minBits = resBits;
            pos = i;
        } 
    }
    cout << "Language of the text is the same of model " << pos+1 << endl;

    return 0;
}