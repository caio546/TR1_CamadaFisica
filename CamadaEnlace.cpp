#include <iostream>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <set>
#include "CamadaEnlace.hpp"
#include "CamadaFisica.hpp"

// Variáveis Globais
vector<int> chaveCRC{1, 0, 0, 1, 1};

int tipoDeEnquadramento = CONTAGEM_CARACTERES; //CONTAGEM_CARACTERES ou INSERCAO_BYTES
int tipoDeControleDeErro = HAMMING; // BIT_PARIDADE, CRC ou HAMMING 

/*****************************************************************************
Funcao responsavel por realizar o enquadramento e controle de erros na mensagem
vinda das camadas superiores
*****************************************************************************/
void CamadaEnlaceDadosTransmissora (vector<int> quadro) {
  vector<int> quadroEmBits;

  quadro = CamadaEnlaceDadosTransmissoraEnquadramento(quadro);

  // Transforma bytes para bits
  for (int i = 0; i < quadro.size(); i++) {
    bitset<8> x = quadro[i];

    for (int j = 7; j >= 0; j--)
      quadroEmBits.push_back(x[j]);
  }

  quadroEmBits = CamadaEnlaceDadosTransmissoraControleDeErro(quadroEmBits);

  CamadaFisicaTransmissora(quadroEmBits);
} 

/*****************************************************************************
Funcao responsavel por aplicar o protocolo de enquadramento escolhido pelo
usuário
*****************************************************************************/
vector<int> CamadaEnlaceDadosTransmissoraEnquadramento (vector<int> quadro) {
  vector<int> quadroEnquadrado;

  switch (tipoDeEnquadramento) {
    case CONTAGEM_CARACTERES:
      quadroEnquadrado = CamadaEnlaceDadosTransmissoraEnquadramentoContagemDeCaracteres(quadro);
      break;
    case INSERCAO_BYTES:
      quadroEnquadrado = CamadaEnlaceDadosTransmissoraEnquadramentoInsercaoDeBytes(quadro);
      break;
  }

  return quadroEnquadrado;
}

/*****************************************************************************
Funcao responsavel por realizar o enquadramento de Contagem de Caracteres
*****************************************************************************/
vector<int> CamadaEnlaceDadosTransmissoraEnquadramentoContagemDeCaracteres (vector<int> quadro) {
  vector<int> quadroEnquadrado;

  int quadrosCheios = (int) quadro.size() / 4;
  int quadrosParciais = (int) quadro.size() % 4;

  // Definir cada quadro com 5 bits
  for (int i = 0; i < quadrosCheios; i++) {
    quadroEnquadrado.push_back(5);

    for (int j = 0 + i * 4; j < 4 + i * 4; j++) {
      quadroEnquadrado.push_back(quadro[j]);
    }
  }

  // O restante dos bits serao enquadrados de acordo com seu tamanho
  if (quadrosParciais != 0) {
    quadroEnquadrado.push_back(quadrosParciais+1);

    for (int i = quadrosCheios * 4; i < quadro.size(); i++) {
      quadroEnquadrado.push_back(quadro[i]);
    }
  }

  return quadroEnquadrado;
}

/*****************************************************************************
Funcao responsavel por realizar o enquadramento de Insercao de Bytes
*****************************************************************************/
vector<int> CamadaEnlaceDadosTransmissoraEnquadramentoInsercaoDeBytes (vector<int> quadro) {
  // O byte de flag escolhido é o numero 27
  vector<int> quadroEnquadrado;

  int quadrosCheios = (int) quadro.size() / 4;
  int quadrosParciais = (int) quadro.size() % 4;


  // Cada quadro foi limitado a 4 bits
  for (int i = 0; i < quadrosCheios; i++) {
    quadroEnquadrado.push_back(27);

    for (int j = 0 + i * 4; j < 4 + i * 4; j++) {
      quadroEnquadrado.push_back(quadro[j]);
    }

    quadroEnquadrado.push_back(27);
  }

  // Enquadramento dos bits restantes
  if (quadrosParciais != 0) {
    quadroEnquadrado.push_back(27);

    for (int i = quadrosCheios * 4; i < quadro.size(); i++) {
      quadroEnquadrado.push_back(quadro[i]);
    }

    quadroEnquadrado.push_back(27);
  }

  return quadroEnquadrado;
}

/*****************************************************************************
Funcao responsavel por realizar o controle de erro escolhido pelo usuário
*****************************************************************************/
vector<int> CamadaEnlaceDadosTransmissoraControleDeErro (vector<int> quadro) {
  vector<int> quadroComControleDeErros;

  switch (tipoDeControleDeErro) {
    case BIT_PARIDADE:
      quadroComControleDeErros = CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(quadro);
      break;
    case CRC:
      quadroComControleDeErros = CamadaEnlaceDadosTransmissoraControleDeErroCRC(quadro);
      break;
    case HAMMING:
      quadroComControleDeErros = CamadaEnlaceDadosTransmissoraControleDeErroCodigoDeHamming(quadro);
      break;
  }

  return quadroComControleDeErros;
}

/*****************************************************************************
Funcao responsavel por realizar o controle de erro por bit de paridade par
*****************************************************************************/
vector<int> CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar (vector<int> quadro) {
  int quantidadeDeBits1 = count(quadro.begin(), quadro.end(), 1);

  if (quantidadeDeBits1 % 2 == 0) {
    quadro.push_back(0);
  } else {
    quadro.push_back(1);
  }

  return quadro;
}

/*****************************************************************************
Funcao responsavel por realizar aplicar o controle de erro CRC (polinomial)
*****************************************************************************/
vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCRC (vector<int> quadro) {
  vector<int> temp(20), rem(20);

  int aux = (int) quadro.size();

  // Completar com zeros o quadro
  for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
    quadro.push_back(0);
  }

  for (int i = 0; i < (int) chaveCRC.size(); i++) {
    rem[i] = quadro[i];
  }

  for (int j = (int) chaveCRC.size(); j <= (int) quadro.size(); j++) {
    for (int i = 0; i < (int) chaveCRC.size(); i++) {
      temp[i] = rem[i];
    }

    if (rem[0] == 0) {
      for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
        rem[i] = temp[i+1];
      }
    } else {
      for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
        rem[i] = (temp[i+1] ^ chaveCRC[i+1]);
      }
    }

    if (j != (int) quadro.size()) {
      rem[(int) chaveCRC.size() - 1] = quadro[j];
    }
  }

  for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
    quadro[aux + i] = rem[i];
  }

  return quadro;
}

/*****************************************************************************
Funcao responsavel por realizar aplicar o controle de erro de Hamming
*****************************************************************************/
vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCodigoDeHamming (vector<int> quadro) {
  vector<int> ans;

  for (int i = 0; i < (int) quadro.size(); i+=8) {
    vector<int> temp; // de 8 em 8

    for (int j = 0; j < 8; j++) {
      temp.push_back(quadro[i+j]);
    }

    vector<int> aux(12);

    aux[2] = temp[0];
    aux[4] = temp[1];
    aux[5] = temp[2];
    aux[6] = temp[3];
    aux[8] = temp[4];
    aux[9] = temp[5];
    aux[10] = temp[6];
    aux[11] = temp[7];

    aux[0] = aux[2] ^ aux[4] ^ aux[6] ^ aux[8] ^ aux[10];
    aux[1] = aux[2] ^ aux[5] ^ aux[6] ^ aux[9] ^ aux[10];
    aux[3] = aux[4] ^ aux[5] ^ aux[6] ^ aux[11];
    aux[7] = aux[8] ^ aux[9] ^ aux[10] ^ aux[11];

    ans.insert(ans.end(), aux.begin(), aux.end());
  }

  return ans;
}

/*****************************************************************************
Funcao responsavel por receber o sinal das camadas anteriores, detectar ou 
corrigir os erros e desenquadrar
*****************************************************************************/
void CamadaEnlaceDadosReceptora (vector<int> quadro) {
  quadro = CamadaEnlaceDadosReceptoraControleDeErro(quadro);

  quadro = CamadaEnlaceDadosReceptoraEnquadramento(quadro);

  CamadaDeAplicacaoReceptora(quadro);
}

/*****************************************************************************
Funcao responsavel por detectar ou corrigir os erros do sinal
*****************************************************************************/
vector<int> CamadaEnlaceDadosReceptoraControleDeErro (vector<int> quadro) {
  vector<int> quadroSemControleDeErros;

  switch (tipoDeControleDeErro) {
    case BIT_PARIDADE:
      quadroSemControleDeErros = CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(quadro);
      break;
    case CRC:
      quadroSemControleDeErros = CamadaEnlaceDadosReceptoraControleDeErroCRC(quadro);
      break;
    case HAMMING:
      quadroSemControleDeErros = CamadaEnlaceDadosReceptoraControleDeErroCodigoDeHamming(quadro);
      break;
  }

  return quadroSemControleDeErros;
}

/*****************************************************************************
Funcao responsavel por detectar os erros por bit de paridade par e avisar
caso surja algum erro
*****************************************************************************/
vector<int> CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar (vector<int> quadro) {
  int quantidadeDeBits1 = count(quadro.begin(), quadro.end(), 1);

  int aux = 0;

  if (quantidadeDeBits1 % 2 == 0) {
    quadro.pop_back();
  } else {
    quadro.pop_back();
    cout << "Houve um erro na transmissão\n";
  }

  return quadro;
}

/*****************************************************************************
Funcao responsavel por detectar os erros por CRC (polinomial) e avisar caso
algum erro seja encontrado
*****************************************************************************/
vector<int> CamadaEnlaceDadosReceptoraControleDeErroCRC (vector<int> quadro) {
  vector<int> temp(20), rem(20);

  int aux = (int) quadro.size();

  for (int i = 0; i < (int) chaveCRC.size(); i++) {
    rem[i] = quadro[i];
  }

  for (int j = (int) chaveCRC.size(); j <= (int) quadro.size(); j++) {
    for (int i = 0; i < (int) chaveCRC.size(); i++) {
      temp[i] = rem[i];
    }

    if (rem[0] == 0) {
      for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
        rem[i] = temp[i+1];
      }
    } else {
      for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
        rem[i] = (temp[i+1] ^ chaveCRC[i+1]);
      }
    }

    if (j != (int) quadro.size()) {
      rem[(int) chaveCRC.size() - 1] = quadro[j];
    }
  }

  for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
    quadro[aux + i] = rem[i];
  }

  for (int i = 0; i < (int) chaveCRC.size() -1; i++) {
    aux += rem[i];
  }

  int aux2 = 0;

  if (aux2 != 0) {
    cout << "Houve um erro na transmissão\n";
  } else {
    for (int i = 0; i < (int) chaveCRC.size() - 1; i++) {
      quadro.pop_back();
    }
  }

  return quadro;
}

/*****************************************************************************
Funcao responsavel por corrigir os erros por código de Hamming
*****************************************************************************/
vector<int> CamadaEnlaceDadosReceptoraControleDeErroCodigoDeHamming (vector<int> quadro) {
  vector<int> ans;

  for (int i = 0; i < (int) quadro.size(); i+=12) {
    vector<int> temp; // de 12 em 12

    for (int j = 0; j < 12; j++) {
      temp.push_back(quadro[i+j]);
    }

    int k1 = temp[0] ^ temp[2] ^ temp[4] ^ temp[6] ^ temp[8] ^ temp[10];
    int k2 = temp[1] ^ temp[2] ^ temp[5] ^ temp[6] ^ temp[9] ^ temp[10];
    int k3 = temp[3] ^ temp[4] ^ temp[5] ^ temp[6] ^ temp[11];
    int k4 = temp[7] ^ temp[8] ^ temp[9] ^ temp[10] ^ temp[11];

    vector<int> aux(8);

    if (k1 == 0 && k2 == 0 && k3 == 0 && k4 == 0) {
      // Não houve erro
    } else {
      // Houve erro
      int indexBitErrado = 8*k4 + 4*k3 + 2*k2 + k1 - 1;

      temp[indexBitErrado] = temp[indexBitErrado] == 1 ? 0 : 1;
    }

    aux[0] = temp[2];
    aux[1] = temp[4];
    aux[2] = temp[5];
    aux[3] = temp[6];
    aux[4] = temp[8];
    aux[5] = temp[9];
    aux[6] = temp[10];
    aux[7] = temp[11];

    ans.insert(ans.end(), aux.begin(), aux.end());
  }

  return ans;
}

/*****************************************************************************
Funcao responsavel por desenquadrar o quadro da forma escolhida pelo usuário
*****************************************************************************/
vector<int> CamadaEnlaceDadosReceptoraEnquadramento (vector<int> quadro) {
  vector<int> quadroDesenquadrado;

  switch (tipoDeEnquadramento) {
    case CONTAGEM_CARACTERES:
      quadroDesenquadrado = CamadaEnlaceDadosReceptoraEnquadramentoContagemDeCaracteres(quadro);
      break;
    case INSERCAO_BYTES:
      quadroDesenquadrado = CamadaEnlaceDadosReceptoraEnquadramentoInsercaoDeBytes(quadro);
      break;
  }

  return quadroDesenquadrado;
}

/*****************************************************************************
Funcao responsavel por desenquadrar o quadro por contagem de caracteres
*****************************************************************************/
vector<int> CamadaEnlaceDadosReceptoraEnquadramentoContagemDeCaracteres(vector<int> quadro) {
  vector<int> mensagemRecebida;
  vector<int> quadroDesenquadrado;

  for (int i = 0; i < quadro.size(); i+=8){
    bitset<8> aux;
    for(int j = i; j < i + 8; j++){
      aux.set(7-(j-i), quadro[j]);
    }
    
    mensagemRecebida.push_back((int) aux.to_ulong());
  }

  for (int i = 0; i < (int) mensagemRecebida.size(); i++) {
    if (mensagemRecebida[i] >= 32) {
      quadroDesenquadrado.push_back(mensagemRecebida[i]);
    }
  }

  return quadroDesenquadrado;
}

/*****************************************************************************
Funcao responsavel por desenquadrar o quadro por insercao de bytes
*****************************************************************************/
vector<int> CamadaEnlaceDadosReceptoraEnquadramentoInsercaoDeBytes(vector<int> quadro) {
  vector<int> mensagemRecebida;
  vector<int> quadroDesenquadrado;

  for (int i = 0; i < quadro.size(); i+=8){
    bitset<8> aux;
    for(int j = i; j < i + 8; j++){
      aux.set(7-(j-i), quadro[j]);
    }
    
    mensagemRecebida.push_back((int) aux.to_ulong());
  }

  for (int i = 0; i < (int) mensagemRecebida.size(); i++) {
    if (mensagemRecebida[i] >= 32) {
      quadroDesenquadrado.push_back(mensagemRecebida[i]);
    }
  }

  return quadroDesenquadrado;
}