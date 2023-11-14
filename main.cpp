#include "objetos.h"

#include <stdlib.h>
#include <iostream>
#include <random>

std::vector<Acao> acoes_feitas = {};

long deu_tiro = 0;
long tomou_tiro = 0;

long tomar_acao(Acao acao, Estado estado) {
    if(acao == Atirar) {
        if (estado.TemJogadorNaMira) {
            deu_tiro++;
            return 4;
        } else { 
            tomou_tiro++;
            return -2;
        }
    } else if (acao == FazerNada) {
        if(estado.TemJogadorNaMira && estado.TomandoTiro) 
            return -3;
        else if (estado.TemJogadorNaMira) 
            return -4;
        else
            return 6;
        
    } else if (acao == AndarDireita || acao == AndarEsquerda) {
        if(estado.TomandoTiro && estado.TemJogadorNaMira)
            return -4;
        else if (estado.TemJogadorNaMira && !(estado.TomandoTiro))
            return -3;
        else
            return 5;
    } else if (acao == Pular) {
        if(estado.TomandoTiro && estado.TemJogadorNaMira) 
            return -3;
        else if (estado.TomandoTiro && !(estado.TemJogadorNaMira))
            return 6;
        else
            return 0;
    }
}
long acoes_nao_tomadas_mas_deveria = 0;
long acoes_nao_tomadas_certo = 0;

bool deve_tomar_acao(long peso, int estado_quantificado, Acao acao, Estado estado) {
    // Aumenta o fator de escala para tornar a função mais sensível a pesos negativos
    double fator_escala = 0.15; // Ajuste conforme necessário

    // Ajusta o peso
    double peso_ajustado = peso * fator_escala;

    // Calcula o valor da função sigmóide
    double sigmoide = 1 / (1 + exp(-peso_ajustado * estado_quantificado));

    // Aumenta o limiar para tornar a decisão mais rigorosa
    double limiar = 0.6; // Ajuste conforme necessário

    return sigmoide > limiar;
}

int quantificar_estado(Estado estado) {
    int resultado = 0;
    if(estado.TemJogadorNaMira)
        resultado += 4;
    if(estado.TomandoTiro)
        resultado += 1;
    if(estado.NoAr)
        resultado += 2;
    return resultado;
}

void aleatorizar_estado(Estado* estado) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distribBool(0, 1);  // Para valores booleanos (true ou false)
    std::uniform_int_distribution<> distribVida(0, 100); // Para o valor de vida

    estado->TomandoTiro = distribBool(gen) == 1;
    estado->NoAr = distribBool(gen) == 1;
    estado->TemJogadorNaMira = distribBool(gen) == 1;

}

long desempenho_ia = 0;
int rodadas = 0;

IA* criar_ia() {
    std::vector<Neuronio*> neuronios = {
        new Neuronio(Atirar),
        new Neuronio(Pular),
        new Neuronio(FazerNada),
        new Neuronio(AndarDireita),
        new Neuronio(AndarEsquerda),
    };
    return new IA(neuronios);
}

long penultimo_melhor_desempenho = -999999;
bool permanecer_primeira_ia = false;

void copiar_ias(std::vector<IA*>* IAs, IA* ultima_ia) {
    *IAs = {
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
        ultima_ia,
    };
    int iterations = 0;
    for(IA* ia : *IAs) {
        if(iterations == 0 && permanecer_primeira_ia) {
            ++iterations;
            continue;
        }
        for(Neuronio* neuronio : ia->m_neuronios) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(-2, 2);
            neuronio->m_peso += distrib(gen);
        }
        ++iterations;
    }
}

unsigned long deu_bom = 0;
unsigned long deu_ruim = 0;

int main() {
    _sleep(1000);
    std::vector<IA*> IAs = {
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
        criar_ia(),
    };
    Estado estado = {};
    int rodada = 0;
    while(true) {
        aleatorizar_estado(&estado);
        for(IA* ia : IAs) {
            ia->executar(quantificar_estado(estado), estado);
        }
        int ultimo_desempenho = IAs[0]->desempenho_ia;
        IA* ultima_ia = IAs[0];
        for(IA* ia : IAs) {
            if(ia->desempenho_ia > ultimo_desempenho) {
                ultimo_desempenho = ia->desempenho_ia;
                ultima_ia = ia;
            }
        }
        if(penultimo_melhor_desempenho > ultimo_desempenho) {
            permanecer_primeira_ia = true;
            //std::cout << "PERMANECENDO PRIMEIRA IA\n";
        } else {
            //std::cout << ">>NAO<< PERMANECENDO PRIMEIRA IA\n";
            permanecer_primeira_ia = false;
            penultimo_melhor_desempenho = ultimo_desempenho;
        }
        //if(ultima_ia == nullptr) {
            //std::cout << "Nao houve ultima IA boa\n";
            //exit(1);
        //}
        if(rodada == 50) {
            std::cout << "NOVA GERACAO\n" << "\tIA mais performatica: " << ultimo_desempenho;
            std::cout << "\n\tAcertos: " << deu_bom << "\t\tErros: " << deu_ruim << "\t\tDiferenca: " << deu_bom-deu_ruim << "\n";
            std::cout << "\tTaxa de tiros: " << deu_tiro - tomou_tiro << "\n";
            copiar_ias(&IAs, ultima_ia);
            rodada = 0;
        }
        ++rodada;
    }
}
