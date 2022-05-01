#include <iostream>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "CamadaEnlace.hpp"
#include "CamadaFisica.hpp"

// Declaracao de variaveis globais
int deslocamento;
int periodoDeTransmissao = 10; // em milisegundos
int porcentagemDeErros = 4; // 0%, 10%, 20%, ..., 100%

int tipoDeCodificacao = MANCHESTER; // BINARIA, MANCHESTER ou BIPOLAR

/*****************************************************************************
Funcao responsavel por receber a mensagem do usuario e repassar para as
proximas camadas
*****************************************************************************/
void AplicacaoTransmissora(void) {
  string mensagem;

  cout << "Digite uma mensagem:" << endl;

  cin >> mensagem;

  CamadaDeAplicacaoTransmissora(mensagem);
}

/*****************************************************************************
Funcao responsavel por transformar a mensagem original em um formato mais
adequado para ser transmitido
*****************************************************************************/
void CamadaDeAplicacaoTransmissora(string mensagem) {
  vector<int> quadro;

  for (int i = 0; i < (int) mensagem.size(); i++) {
    quadro.push_back(mensagem[i]);
  }
  
  // chama a proxima camada
  CamadaEnlaceDadosTransmissora(quadro);
}

/*****************************************************************************
Funcao responsavel por codificar a mensagem original
*****************************************************************************/
void CamadaFisicaTransmissora(vector<int> quadro) {
  vector<int> fluxoBrutoDeBits;

  switch (tipoDeCodificacao) {
    case BINARIA:
      fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoBinaria(quadro);
      break;
    case MANCHESTER:
      fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoManchester(quadro);
      break;
    case BIPOLAR:
      fluxoBrutoDeBits = CamadaFisicaTransmissoraCodificacaoBipolar(quadro);
      break;
  }

  MeioDeComunicacao(fluxoBrutoDeBits);
}

/*****************************************************************************
Funcao responsavel por utilizar a codificacao binaria
*****************************************************************************/
vector<int> CamadaFisicaTransmissoraCodificacaoBinaria(vector<int> quadro) {
  vector<int> result;

  for (int i = 0; i < quadro.size(); i++) {
    result.push_back(quadro[i] == 0 ? 0 : 5);
  }
  
  return result;
}

/*****************************************************************************
Funcao responsavel por utilizar a codificacao manchester
*****************************************************************************/
vector<int> CamadaFisicaTransmissoraCodificacaoManchester(vector<int> quadro) {
  vector<int> result;
  
  for (int i = 0; i < quadro.size(); i++) {
    result.push_back(quadro[i] ^ 0); // XOR com amplitude baixa do clock
    result.push_back(quadro[i] ^ 1); // XOR com amplitude alta do clock
  }

  return result;
}

/*****************************************************************************
Funcao responsavel por utilizar a codificacao bipolar
*****************************************************************************/
vector<int> CamadaFisicaTransmissoraCodificacaoBipolar(vector<int> quadro) {
  bool up = true;

  vector<int> result;

  for(int i = 0; i < quadro.size(); i++){
    result.push_back(quadro[i]==0 ? 0 : (up ? 1 : -1));

    if(quadro[i]) up = !up;
  }

  return result;
}

/* Este metodo simula a transmissao da informacao no meio de
 * comunicacao, passando de um pontoA (transmissor) para um
 * ponto B (receptor)
 */
void MeioDeComunicacao(vector<int> fluxoBrutoDeBits) {
  iniciarGUI(); // Inicializar interface grafica

  int erro;
  vector<int> fluxoBrutoBitsPontoA, fluxoBrutoBitsPontoB;

  fluxoBrutoBitsPontoA = fluxoBrutoDeBits;
  fluxoBrutoBitsPontoB = fluxoBrutoDeBits;

  for(int i = 0 ; i < fluxoBrutoBitsPontoA.size() ; i++) {
    int ruido = rand() % 100;

    if (ruido < porcentagemDeErros) {
      // Erro
      fluxoBrutoBitsPontoA[i] == 0 ?
      fluxoBrutoBitsPontoA[i] = 1  :
      fluxoBrutoBitsPontoA[i] = 0;
    }

    fluxoBrutoBitsPontoB[i] = ( fluxoBrutoBitsPontoA[i] ); // BITS! Sendo transferidos
  }

  transmitirInformacao(fluxoBrutoBitsPontoB); // Imprimir informacao codificada na tela

  // chama proxima camada
  CamadaFisicaReceptora(fluxoBrutoBitsPontoB);
}

/*****************************************************************************
Funcao responsavel por codificar a mensagem original
*****************************************************************************/
void CamadaFisicaReceptora(vector<int> quadro) {
  vector<int> fluxoBrutoDeBits;

  switch (tipoDeCodificacao) {
    case BINARIA:
      fluxoBrutoDeBits = CamadaFisicaReceptoraDecodificacaoBinaria(quadro);
      break;
    case MANCHESTER:
      fluxoBrutoDeBits = CamadaFisicaReceptoraDecodificacaoManchester(quadro);
      break;
    case BIPOLAR:
      fluxoBrutoDeBits = CamadaFisicaReceptoraDecodificacaoBipolar(quadro);
      break;
  }

  // chama proxima camada
  CamadaEnlaceDadosReceptora(fluxoBrutoDeBits);
}

/*****************************************************************************
Funcao responsavel por utilizar a decodificacao binaria
*****************************************************************************/
vector<int> CamadaFisicaReceptoraDecodificacaoBinaria(vector<int> quadro){
  // implementacao do algoritmo para DECODIFICAR
  vector<int> result;

  for (int i = 0; i < quadro.size(); i++) {
    result.push_back(quadro[i] == 0 ? 0 : 1);
  }
  
  return result;
}

/*****************************************************************************
Funcao responsavel por utilizar a decodificacao manchester
*****************************************************************************/
vector<int> CamadaFisicaReceptoraDecodificacaoManchester(vector<int> quadro) {
  // implementacao do algoritmo para DECODIFICAR
  vector<int> result;

  for (int i = 0; i < (int) quadro.size(); i += 2) {
    if (quadro[i] == 0 && quadro[i+1] == 1) {
      result.push_back(0);
    } else {
      result.push_back(1);
    }
  }
  
  return result;
}

/*****************************************************************************
Funcao responsavel por utilizar a decodificacao bipolar
*****************************************************************************/
vector<int> CamadaFisicaReceptoraDecodificacaoBipolar(vector<int> quadro) {
  // implementacao do algoritmo para DECODIFICAR
  vector<int> result;
  for(int i = 0; i < quadro.size(); i++){
    result.push_back(quadro[i] == 0 ? 0 : 1);
  }

  return result;
}

/*****************************************************************************
Funcao responsavel por transformar os bits na mensagem original
*****************************************************************************/
void CamadaDeAplicacaoReceptora(vector<int> quadro) {
  string mensagem;

  for (int i = 0; i < (int) quadro.size(); i++) {
    mensagem.push_back((char) quadro[i]);
  }
    
  // chama proxima camada
  AplicacaoReceptora(mensagem);
}

/*****************************************************************************
Funcao responsavel por exibir a mensagem decodificada ao usuario
*****************************************************************************/
void AplicacaoReceptora(string mensagem) {
  cout << "A mensagem recebida foi:\n" << mensagem << endl;
}

/*****************************************************************************
Funcao responsavel por inicializar a interface gráfica
*****************************************************************************/
void iniciarGUI() {
  WINDOW* screen = initscr(); // Inicializar tela
  start_color(); // Inicializar esquema de cores
  curs_set(0); // Desativar cursor

  // Esquema de cores
  init_pair(0, COLOR_WHITE, COLOR_BLACK); // Pixel branco com fundo preto
  attron(COLOR_PAIR(0)); // Atribuir cor acima
  attron(A_BOLD); // Atribuir negrito
  attron(A_STANDOUT); // Atribuir destaque 
}

/*****************************************************************************
Funcao responsavel por exibir graficamente um bit de amplitude positiva
*****************************************************************************/
void exibirPositivo(int shift) {
  double x;

  if (tipoDeCodificacao == MANCHESTER) {
    for (x=0.0 + shift; x < COLS/16.0 + shift; x += 1.0) {
      // imprimir na tela um pixel
      mvprintw(5, (int)(x), " ");
    }
  } else {
    for (x=0.0 + shift; x < COLS/8.0 + shift; x += 1.0) {
      // imprimir na tela um pixel
      mvprintw(5, (int)(x), " ");
    }
  }
}

/*****************************************************************************
Funcao responsavel por exibir graficamente um bit de amplitude nula
*****************************************************************************/
void exibirZero(int shift) {
  double x;

  if (tipoDeCodificacao == MANCHESTER) {
    for (x=0.0 + shift; x < COLS/16.0 + shift; x += 1.0) {
      // Imprimir na tela um pixel
      mvprintw(10, (int)(x), " ");
    }
  } else {
    for (x=0.0 + shift; x < COLS/8.0 + shift; x += 1.0) {
      // Imprimir na tela um pixel
      mvprintw(10, (int)(x), " ");
    }
  }
}

/*****************************************************************************
Funcao responsavel por exibir graficamente um bit de amplitude negativa
*****************************************************************************/
void exibirNegativo(int shift) {
  double x;

  for (x=0.0 + shift; x < COLS/8.0 + shift; x += 1.0) {
    // Imprimir na tela um pixel
    mvprintw(15, (int)(x), " ");
  }
}

/*****************************************************************************
Funcao responsável por exibir graficamente a mensagem codificada que caminha
pelo meio de transmissao, podendo ser na forma binaria, manchester ou bipolar
*****************************************************************************/
void transmitirInformacao(vector<int> bits) {
  for (int i = 0; i < bits.size(); i++) {
    switch (tipoDeCodificacao) {
      case BINARIA:
        // Imprime um byte por vez na tela
        if (i % 8 == 0) {
          erase(); // Apaga conteudo da tela
          deslocamento = 0;
        }

        if (bits[i] == 0) {
          exibirZero(deslocamento);
        } else {
          exibirPositivo(deslocamento);
        }

        deslocamento += COLS/8; 
    
        sleep_for(milliseconds(periodoDeTransmissao));
        break;
      case MANCHESTER:
        // Imprime 2 bytes por vez
        // Pois a codificacao Manchester tem o dobro de bits
        if (i % 16 == 0) {
          erase();
          deslocamento = 0;
        }

        if (bits[i] == 0) {
          exibirZero(deslocamento/2);
        } else {
          exibirPositivo(deslocamento/2);
        }

        deslocamento += COLS/8; 
    
        sleep_for(milliseconds(periodoDeTransmissao/2));
        break;
      case BIPOLAR:
        if (i % 8 == 0) {
          erase();
          deslocamento = 0;
        }

        if (bits[i] == 0) {
          exibirZero(deslocamento);
        } else if (bits[i] == 1) {
          exibirPositivo(deslocamento);
        } else {
          exibirNegativo(deslocamento);
        }
        deslocamento += COLS/8; 
    
        sleep_for(milliseconds(periodoDeTransmissao));
        break;
    }

    refresh(); // Escreve informacao na tela
  }

  endwin(); // Fecha interface grafica
}
