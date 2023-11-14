#pragma once

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

struct Estado {
    bool TomandoTiro;
    bool NoAr;
    bool TemJogadorNaMira;
    long Vida;
};

enum Acao {
    FazerNada=0,
    AndarDireita=1,
    AndarEsquerda=2,
    Pular=3,
    Atirar=4,
};

extern std::vector<Acao> acoes_feitas;

bool deve_tomar_acao(long, int, Acao, Estado);
long tomar_acao(Acao, Estado estado);

extern int rodadas;

extern unsigned long deu_bom;
extern unsigned long deu_ruim;

extern long acoes_nao_tomadas_mas_deveria;
extern long acoes_nao_tomadas_certo;

class Neuronio {
    private:
        Acao m_acao;
    public:
        long m_peso = 0;
        Neuronio(Acao acao) : m_acao(acao) {
        }
        int executar(unsigned int estado_quantificado, Estado estado_real) {
            if(deve_tomar_acao(m_peso, estado_quantificado, m_acao, estado_real)) {
                long resultado = tomar_acao(m_acao, estado_real);
                //std::cout << "resultado: " << resultado << "\t\tpeso: " << m_peso << "\n";
                if(resultado >= 0)
                    ++deu_bom;
                else
                    ++deu_ruim;
                return resultado; 
            } else {
                //std::cout << "SEM ACAO PARA " << m_acao;
                if(tomar_acao(m_acao, estado_real) > 0) {
                    //std::cout << "\t\t\tMAS DEVIA\n";
                    ++acoes_nao_tomadas_mas_deveria;
                    ++deu_ruim;
                    return -6;
                }
                ++acoes_nao_tomadas_certo;
                ++deu_bom;
                //std::cout << "\t\t\tGGG\n";
                return 6;
            }
        }
};

class IA {
    private:
    public:
        std::vector<Neuronio*> m_neuronios;
        int desempenho_ia = 0;
        void executar(unsigned int estado_quantificado, Estado estado_real) {
            for(Neuronio* neuronio : m_neuronios) {
                desempenho_ia += neuronio->executar(estado_quantificado, estado_real);
            }
        }

        IA(std::vector<Neuronio*> neuronios) : m_neuronios(neuronios) {
            for(Neuronio* neuronio : m_neuronios) {
                std::random_device rd;
                std::mt19937 gen(rd());

                std::uniform_int_distribution<> distribValor(0, 5);

                neuronio->m_peso = distribValor(gen);
            }
        }

};
