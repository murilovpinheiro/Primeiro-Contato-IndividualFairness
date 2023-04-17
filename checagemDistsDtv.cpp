#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <queue>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;
using namespace std::chrono;

void escrever(vector<vector<double>> matriz){
    // Matriz com 3 colunas e 2 linhas

    // Nome do arquivo a ser criado
    string nome_arquivo = to_string(int(matriz[0][0])) + ".csv";

    // Criação do arquivo
    ofstream arquivo(".//distanciasQuebradas//"+ nome_arquivo);

    // Percorre a matriz e escreve cada linha no arquivo
    int lastNumber = -1;
    for (auto linha : matriz) {
        arquivo << linha[0] << "," << linha[1] << "," << linha[2] << "\n";
        if (linha[0] != lastNumber){
            lastNumber = linha[0];
            cout << "ESCRITA: " << lastNumber << endl;
        }
    }
    // Fecha o arquivo
    arquivo.close();

}

// Defina o caminho da pasta que deseja ler
string folder_path = ".//Dtvs//";
string folder_path2 = ".//distanciasOrganizadas//";

vector<int> ler_nomesArquivos(string pathDistance){
    vector<int> filenames;
    string filename;
    // Itere sobre os arquivos na pasta
    for (const auto& file : fs::directory_iterator(pathDistance))
    {
        filename = file.path().filename().string();
        filename = filename.substr(0, filename.find("."));
        filenames.push_back(stoi(filename));
        // Imprime o nome do arquivo
    }
    sort(filenames.begin(), filenames.end());
    // for (int i = 0; i<filenames.size(); i++){
    //     cout << i <<" "<<filenames[i] << endl;
    // }
    return filenames;
}

queue<vector<double>> pesquisa(int importantToken, vector<int> &filenames){
    string archive = "-1.csv";
    // , queue<vector<double>> distsQueue
    for (int i = 0; i < filenames.size(); i++){
        if (importantToken == filenames[i]){
            archive = to_string(importantToken) + ".csv";
            break;
        }
        else if (importantToken < filenames[i]){
            archive = to_string(filenames[i-1]) + ".csv";
            break;
        }
    }
    if (archive == "-1.csv") archive = "47356.csv";
    string line;
    ifstream myfile (folder_path2 + archive);

    int index = 0;
    queue<vector<double>> newDistsQueue;
    vector<double> temporario(3);
    
    string token;
    int tkNumber;
    bool END;
    bool breakWhile = false;
    if (myfile.is_open()) {
        //Se o Arquivo tiver aberto, a gente pega linha por linha
        int lastToken = -1;

        while (getline(myfile, line)){
            istringstream iss(line);
            getline(iss,token,',');
            if(stoi(token) == importantToken){
                temporario.at(0) = stoi(token);
                getline(iss,token,',');
                temporario.at(1) = stoi(token);
                getline(iss,token,',');
                temporario.at(2) = stof(token);
                break;
            }
        }
        newDistsQueue.push(temporario);
        while (getline(myfile,line)) {
            tkNumber = 0;
            istringstream iss(line);

            //tkNumber vai servir para saber qual Token ele tá pegando se é 1, 2 ou 3
            //Agora a gente pega cada token separado por vírgula da linha i

            while (getline(iss, token, ',')) {
                //Salva o token, em forma de double, num vector temporário de tamanho 3, até porque toda linha só tem 3 tokens
                temporario.at(tkNumber) = stof(token);
                tkNumber++;
                // cout << "Token Atual: " << token << " --- Ultimo Token: " << lastToken << " --- Token Number: "<<  tkNumber<<endl;
                if (tkNumber == 1 && stoi(token) != lastToken){
                    if(lastToken != -1){
                    breakWhile = true;
                    break;
                    // Aqui pesquisa
                    }
                    //Se o token for o primeiro, o mais importante e ele não for igual ao último token observado quer dizer que o token mudou
                    lastToken = stof(token);
                    //Mudamos o token para o novo
                }
                else if(tkNumber == 3){
                    newDistsQueue.push(temporario);
                }
            }
            if (breakWhile) break;
        }
        myfile.close();
    } else {
        cout << "Erro ao abrir o arquivo";
    }
    //pesquisa pronta
    return newDistsQueue;
}

void comparacao(queue<vector<double>> &fila1, queue<vector<double>> &fila2, vector<vector<double>> &saida){
    // Percorre os elementos das filas e compara as chaves

    vector<double> elemento1;
    int chave1;
    int chave12;
    double valor1;

    vector<double> elemento2;
    int chave2;
    int chave22;
    double valor2;

    while (!fila1.empty()) {
        elemento1 = fila1.front();
        chave1 = elemento1[0];
        chave12 = elemento1[1];
        valor1 = elemento1[2];

        while (!fila2.empty()) {
            elemento2 = fila2.front();
            chave2 = elemento2[0];
            chave22 = elemento2[1];
            valor2 = elemento2[2];

            if (chave1 != chave2) break;

            else if (chave12 < chave22) break;

            else if (valor1 < valor2) {
                saida.push_back(elemento1);
                break;
            }
            fila2.pop();
        }
        fila1.pop();
    }
}

void checagemDistsDtv(){
    /*
    Esse código basicamente roda todas as Dtvs e checa se elas são maiores ou menores que a Distância calculada.
    As Dtvs que forem contra a Restrição de Lipschitz ele salva em ./distanciasQuebradas/num.csv
    Salvando em arquivos csv
    Poderiam ser feitas algumas otimizações como por exemplo:
        --> no cálculo da Dist criar páginas maiores, acredito que tenhamos mais capacidade e isso vai diminuir o número de arquivos abertos nesse código
        --> organizar os Dtvs, organizando o Dtv assim como as Dists tornaria tudo mais "equilibrado"
        --> no mais nã consigo pensar em nenhuma outra forma de otimizar mais, não com os conhecimentos que tenho atualmente.
    */

    vector<int> filenames = ler_nomesArquivos(".//distanciasOrganizadas//");
    auto start = high_resolution_clock::now();
    for (int i = 0; i <= 9700; i = i + 100){
        
        vector<vector<double>> saida;
        string archive = to_string(i) + ".csv";
        cout << archive << endl;
        string line;
        ifstream myfile (folder_path + archive);

        int index = 0;
        queue<vector<double>> distsQueue;
        queue<vector<double>> dtvsQueue;
        vector<double> temporario(3);
        string token;

        int tkNumber;
        bool END;
        if (myfile.is_open()) {
            //Se o Arquivo tiver aberto, a gente pega linha por linha
            int lastToken = -1;
            while (getline(myfile,line)) {
                istringstream iss(line);
                tkNumber = 0;
                //tkNumber vai servir para saber qual Token ele tá pegando se é 1, 2 ou 3
                //Agora a gente pega cada token separado por vírgula da linha i
                while (getline(iss, token, ',')) {
                    //Salva o token, em forma de double, num vector temporário de tamanho 3, até porque toda linha só tem 3 tokens
                    temporario.at(tkNumber) = stof(token);
                    tkNumber++;
                    //cout << "Token Atual: " << token << " --- Ultimo Token: " << lastToken << " --- Token Number: "<<  tkNumber<<endl;
                    if (tkNumber == 1 && stoi(token) != lastToken){
                        if(lastToken != -1){
                    //      cout << "BUSCA POR DIVERGENCIAS: " << lastToken << endl;
                            dtvsQueue = pesquisa(lastToken, filenames);
                            comparacao(distsQueue, dtvsQueue, saida);
                        }
                        //Se o token for o primeiro, o mais importante e ele não for igual ao último token observado quer dizer que o token mudou
                        lastToken = stof(token);
                        //Mudamos o token para o novo
                    }
                    else if(tkNumber == 3){
                        distsQueue.push(temporario);
                    }
                }
            }           
            dtvsQueue = pesquisa(lastToken, filenames);
            comparacao(distsQueue, dtvsQueue, saida);
            myfile.close();
            auto stop = high_resolution_clock::now(); // registra o tempo atual novamente
            cout << "Arquivo " << archive << " Terminado." << endl;
            auto duration = duration_cast<milliseconds>(stop - start); // calcula a diferença de tempo
            cout << "Tempo de execucao atual: " << duration.count() / 1000 << " Segundos;" << endl; // exibe o tempo em milissegundos
        } else {
            cout << "Erro ao abrir o arquivo";
        }
        escrever(saida);
    }
    auto stopfinal = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stopfinal - start);
    cout << "Tempo de Execucao Total: " << duration.count() / 1000 << " segundos;" << endl;
}

unsigned int contagem(string path){
    unsigned int contagem = 0;
    vector<int> filenames = ler_nomesArquivos(path);

    for (int i = 0; i < filenames.size(); i++){
        cout << filenames[i] << endl;
        string archive = to_string(filenames[i]) + ".csv";
        string line;
        ifstream myfile(path + archive);
        if (myfile.is_open()){
            while(getline(myfile, line)){
                contagem++;
            }
        }
        else{
            cout << archive << endl;
            cout << "Erro ao abrir o arquivo" << endl;
        }
    }
    return contagem; 
}

void proporcaoInvalidos(){
    unsigned int contagemDtvs = contagem(".//Dtvs//");
    unsigned int contagemDistsQuebradas = contagem(".//distanciasQuebradas//");
    cout << "DTVs: " << contagemDtvs << endl;
    cout << "Distancias Quebradas: " << contagemDistsQuebradas << endl;

    cout << "Proporcao: "<< double(contagemDistsQuebradas) / double(contagemDtvs) << endl;
}
int main(){
    proporcaoInvalidos();
    // Resultado: 
    //DTVs: 47711796
    //Distancias Quebradas: 43472863
    //Proporcao: 0.911155, logo 90% das distâncias não são válidas.
    return 0;
}