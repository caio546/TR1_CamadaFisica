#include <iostream>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "CamadaEnlace.hpp"
#include "CamadaFisica.hpp"

void CamadaEnlaceDadosTransmissora (vector<int> quadro) {
  vector<int> quadroEmBits;

  quadro = CamadaEnlaceDadosTransmissoraEnquadramento(quadro);

  quadro = CamadaEnlaceDadosTransmissoraControleDeErro(quadro);

  // O problema ta aqui
  for (int i = 0; i < quadro.size(); i++) {
    bitset<8> x = quadro[i];

    for (int j = 7; j >= 0; j--)
      quadroEmBits.push_back(x[j]);
  }

  CamadaFisicaTransmissora(quadroEmBits);
} 

vector<int> CamadaEnlaceDadosTransmissoraEnquadramento (vector<int> quadro) {
  int tipoDeEnquadramento = 1;
  vector<int> quadroEnquadrado;

  switch (tipoDeEnquadramento) {
    case 0: // contagem de caracteres
      quadroEnquadrado = CamadaEnlaceDadosTransmissoraEnquadramentoContagemDeCaracteres(quadro);
      break;
    case 1: // insercao de bytes
      quadroEnquadrado = CamadaEnlaceDadosTransmissoraEnquadramentoInsercaoDeBytes(quadro);
      break;
  }

  return quadroEnquadrado;
}

vector<int> CamadaEnlaceDadosTransmissoraEnquadramentoContagemDeCaracteres (vector<int> quadro) {
  vector<int> quadroEnquadrado;

  int quadrosCheios = (int) quadro.size() / 4;
  int quadrosParciais = (int) quadro.size() % 4;

  for (int i = 0; i < quadrosCheios; i++) {
    quadroEnquadrado.push_back(5);

    for (int j = 0 + i * 4; j < 4 + i * 4; j++) {
      quadroEnquadrado.push_back(quadro[j]);
    }
  }

  if (quadrosParciais != 0) {
    quadroEnquadrado.push_back(quadrosParciais+1);

    for (int i = quadrosCheios * 4; i < quadro.size(); i++) {
      quadroEnquadrado.push_back(quadro[i]);
    }
  }

  return quadroEnquadrado;
}

vector<int> CamadaEnlaceDadosTransmissoraEnquadramentoInsercaoDeBytes (vector<int> quadro) {
  vector<int> quadroEnquadrado;

  int quadrosCheios = (int) quadro.size() / 4;
  int quadrosParciais = (int) quadro.size() % 4;

  for (int i = 0; i < quadrosCheios; i++) {
    quadroEnquadrado.push_back(27);

    for (int j = 0 + i * 4; j < 4 + i * 4; j++) {
      quadroEnquadrado.push_back(quadro[j]);
    }

    quadroEnquadrado.push_back(27);
  }

  if (quadrosParciais != 0) {
    quadroEnquadrado.push_back(27);

    for (int i = quadrosCheios * 4; i < quadro.size(); i++) {
      quadroEnquadrado.push_back(quadro[i]);
    }

    quadroEnquadrado.push_back(27);
  }

  return quadroEnquadrado;
}

vector<int> CamadaEnlaceDadosTransmissoraControleDeErro (vector<int> quadro) {
  int tipoDeControleDeErro = 0;

  switch (tipoDeControleDeErro) {
    case 0: // bit de paridade par
      // codigo
      break;
    case 1: // CRC
      // codigo
      break;
  }

  return quadro;
}

void CamadaEnlaceDadosReceptora (vector<int> quadro) {
  quadro = CamadaEnlaceDadosReceptoraEnquadramento(quadro);

  quadro = CamadaEnlaceDadosReceptoraControleDeErro(quadro);

  CamadaDeAplicacaoReceptora(quadro);
}

vector<int> CamadaEnlaceDadosReceptoraEnquadramento (vector<int> quadro) {
  int tipoDeEnquadramento = 1;
  vector<int> quadroDesenquadrado;

  switch (tipoDeEnquadramento) {
    case 0: // contagem de caracteres
      quadroDesenquadrado = CamadaEnlaceDadosReceptoraEnquadramentoContagemDeCaracteres(quadro);
      break;
    case 1:
      quadroDesenquadrado = CamadaEnlaceDadosReceptoraEnquadramentoInsercaoDeBytes(quadro);
      break;
  }

  return quadroDesenquadrado;
}

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

vector<int> CamadaEnlaceDadosReceptoraControleDeErro (vector<int> quadro) {
  int tipoDeControleDeErro = 0;

  switch (tipoDeControleDeErro) {
    case 0: // bit de paridade par
      // codigo
      break;
    case 1: // CRC
      // codigo
      break;
  }

  return quadro;
}

void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar (vector<int> quadro) {
  //
}

void CamadaEnlaceDadosTransmissoraControleDeErroCRC (vector<int> quadro) {
  //
}

void CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar (vector<int> quadro) {
  //
}

void CamadaEnlaceDadosReceptoraControleDeErroCRC (vector<int> quadro) {
  //
}