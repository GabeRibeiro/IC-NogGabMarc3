#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <vector>
using namespace std;

void fcmMain(string ficheiros[],int n_files,int k,string output_file)
{
    vector<char> circularBuffer;
    vector<int> appCnt,ctxtotal;
    vector<double> entropy_cnt;
    unordered_map<string,unordered_map<char,int>> map1;
    char character1;
    int total_ctx_count = 0, sum_non = 0, fillcb = 0, occur = 0,aux_cnt, cb_ptr = 0,total_letters = 0;
    double prob, percent, log_val, entropy = 0, model_entropy;
    string aux, aux2;
    string file, line;
    ofstream ofs(output_file); 

    for (int i = 0; i < n_files;i++)
    {
        ifstream ifs(ficheiros[i]);
        circularBuffer.clear();
        if (!ifs.is_open())
        {
            cout << "Can't open file." << endl;
            exit(1);
        }


        
        //loop until the file ends
        while (getline(ifs, line))
        {
            fillcb=0; //contador de elementos do buffer fica a 0 quando muda a linha
            circularBuffer.clear(); //limpa o buffer quando muda a linha
            for (int j = 0; j < line.size(); ++j)
            {
                
                character1 = line[j]; //recolhe novo caracter
                
                if (character1 >= 'A' && character1 <= 'Z' || character1 >= 'a' && character1 <= 'z') //se for letra
                {   
                    total_letters++; //conta o número de letras
                    if(fillcb < k)// enquanto o buffer ainda não está cheio
                    {   
                        circularBuffer.push_back(character1); //primeiras k letras só adiciona ao buffer
                        fillcb++;
                        cb_ptr = circularBuffer.size() - 1;
                    }

                    else // quando o buffer está cheio
                    {
                        aux = "";
                        aux2;
                        aux_cnt=cb_ptr;
                        
                        for (int i = 1; i<=k; i++)
                        {
                            aux_cnt = ((cb_ptr + i) % k); 
                            aux2 = string(1,circularBuffer[aux_cnt]);
                            aux = aux + aux2  ; //escreve o contexto
                        }
                        cb_ptr = ((cb_ptr + 1) % k); //incrementa o ponteiro do circular buffer
                        circularBuffer[cb_ptr] = character1; //adiciona o novo caracter ao buffer
                        map1[aux][character1]++; //adiciona contexto e letra ao mapa
                    }
                    
                }
                
                else
                {
                    ++sum_non; //não letras (não usado)
                    continue;
                }
                
            }
            
        }
        ifs.close();
    }
    

    ofs << "ctx\tletter\tcount" << endl;

    for (auto it:map1) //percorre o mapa principal
    {   
        occur = 0;
        entropy = 0;
        appCnt.clear();
        auto aux_map = it.second;
        for(auto it2:aux_map) //percorre o mapa secundário
        {
            occur += it2.second; //conta numero de vezes que um certo contexto apareceu
            appCnt.push_back(it2.second); // guarda o numero de vezes que cada letra apareceu depois do contexto
            ofs << it.first << "\t" << it2.first << "\t\t" << it2.second << endl;
        }
        total_ctx_count += occur; // conta total dos totais da soma dos contextos
        for(auto it3:appCnt) 
        {
            prob = (double) it3 / (double) occur; //calcula prob de uma certa letra a seguir a um ctx

            if(prob == 0)
                log_val = 0;
            else
                log_val = log2(prob);

            entropy += prob * log_val; //entropia da linha



        }
        entropy_cnt.push_back(-1 * entropy); //guardar entropia de cada ctx
        ctxtotal.push_back(occur); // guarda o numero de vezes que cada ctx ocorreu
    }

    for (int i=0; i < entropy_cnt.size();i++)
    {   
        model_entropy += (double) entropy_cnt[i] * ((double) ctxtotal[i]/ (double) total_ctx_count); //entropia final do modelo seguindo a formula (soma (Entropia(ctx) * Prob(ctx)))
    }

    ofs << "--------------------------------" << endl;
    cout << "Entropy = " << model_entropy << endl;
    ofs << "Entropy = " << model_entropy << endl;
    ofs << "Total Letters = " << total_letters << endl;
    ofs << "Total Contexts = " << total_ctx_count << endl;
    ofs.close();
}

int main(int argc, char *argv[])
{   
    
    if(argc != 3){
        puts("\nUsage: output_file order");
        exit(1);
    }

    int k = atoi(argv[2]);
    string ficheiros[] = {"texto.txt","texto2.txt","texto3.txt"};
    
    
    // MainFunction
    fcmMain(ficheiros,sizeof(ficheiros)/sizeof(ficheiros[0]),k,argv[1]);
    
    
    return 0;
}