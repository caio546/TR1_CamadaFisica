#include <iostream>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <set>
#include "CamadaEnlace.hpp"
#include "CamadaFisica.hpp"

int tipoDeEnquadramento = CONTAGEM_CARACTERES; //CONTAGEM_CARACTERES ou INSERCAO_BYTES
int tipoDeControleDeErro = HAMMING; // BIT_PARIDADE, CRC ou HAMMING

set<int> potenciasDe2 { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
                        2048, 4096, 8192, 16384, 32768, 65536
                      };

void CamadaEnlaceDadosTransmissora (vector<int> quadro) {
  vector<int> quadroEmBits;

  quadro = CamadaEnlaceDadosTransmissoraEnquadramento(quadro);

  // O problema ta aqui
  for (int i = 0; i < quadro.size(); i++) {
    bitset<8> x = quadro[i];

    for (int j = 7; j >= 0; j--)
      quadroEmBits.push_back(x[j]);
  }

  quadroEmBits = CamadaEnlaceDadosTransmissoraControleDeErro(quadroEmBits);

  CamadaFisicaTransmissora(quadroEmBits);
} 

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

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar (vector<int> quadro) {
  int quantidadeDeBits1 = count(quadro.begin(), quadro.end(), 1);

  if (quantidadeDeBits1 % 2 == 0) {
    quadro.push_back(0);
  } else {
    quadro.push_back(1);
  }

  return quadro;
}

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCRC (vector<int> quadro) {
  vector<int> key{1, 0, 0, 1, 1};
  vector<int> temp(20), rem(20);

  int aux = (int) quadro.size();

  // Completar com zeros o quadro
  for (int i = 0; i < (int) key.size() - 1; i++) {
    quadro.push_back(0);
  }

  for (int i = 0; i < (int) key.size(); i++) {
    rem[i] = quadro[i];
  }

  for (int j = (int) key.size(); j <= (int) quadro.size(); j++) {
    for (int i = 0; i < (int) key.size(); i++) {
      temp[i] = rem[i];
    }

    if (rem[0] == 0) {
      for (int i = 0; i < (int) key.size() - 1; i++) {
        rem[i] = temp[i+1];
      }
    } else {
      for (int i = 0; i < (int) key.size() - 1; i++) {
        rem[i] = (temp[i+1] ^ key[i+1]);
      }
    }

    if (j != (int) quadro.size()) {
      rem[(int) key.size() - 1] = quadro[j];
    }
  }

  for (int i = 0; i < (int) key.size() - 1; i++) {
    quadro[aux + i] = rem[i];
  }

  // cout << "CRC 1:" << endl;
  // for (int i = 0; i < (int) key.size() -1; i++) {
  //   cout << rem[i] << " ";
  // }
  // cout << endl;

  // cout << "Quadro 1:" << endl;
  // for (int i = 0; i < (int) quadro.size(); i++) {
  //   cout << quadro[i] << " ";
  // }
  // cout << endl;

  return quadro;
}

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCodigoDeHamming (vector<int> quadro) {
  // cout << "Debug:" << endl;
  // for (int i = 0; i < (int) quadro.size(); i++) {
  //   if (i % 8 == 0) cout << " | ";
  //   cout << quadro[i] << " ";
  // }
  // cout << endl;

  // int quantidadeDeCodificacoes = (int) quadro.size() / 8;
  // int bitsRestantes = (int) quadro.size() % 8;
  vector<int> ans;

  // vector<int> ans(12 * quantidadeDeCodificacoes + bitsRestantes);

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

    // cout << "Debug:" << endl;
    // for (int i = 0; i < (int) temp.size(); i++) {
    //   if (i % 8 == 0) cout << " | ";
    //   cout << temp[i] << " ";
    // }
    // cout << endl;

    // vector<int> aux;

    // temp = aux;
  }

  // cout << "Debug:" << endl;
  // for (int i = 0; i < (int) ans.size(); i++) {
  //   if (i % 12 == 0) cout << " | ";
  //   cout << ans[i] << " ";
  // }
  // cout << endl;

  return ans;
}

void CamadaEnlaceDadosReceptora (vector<int> quadro) {
  quadro = CamadaEnlaceDadosReceptoraControleDeErro(quadro);

  // cout << "Debug: " << endl;

  // for (int i = 0; i < (int) quadro.size(); i++) {
  //   if (i % 8 == 0) cout << " | ";
  //   cout << quadro[i] << " ";
  // }
  // cout << endl;

  quadro = CamadaEnlaceDadosReceptoraEnquadramento(quadro);

  CamadaDeAplicacaoReceptora(quadro);
}

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

  // cout << "Debug:" << endl;
  // for (int i = 0; i < (int) quadroSemControleDeErros.size(); i++) {
  //   cout << quadroSemControleDeErros[i] << " "; 
  // }
  // cout << endl;

  return quadroSemControleDeErros;
}

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

vector<int> CamadaEnlaceDadosReceptoraControleDeErroCRC (vector<int> quadro) {
  vector<int> key{1, 0, 0, 1, 1};
  vector<int> temp(20), rem(20);

  int aux = (int) quadro.size();

  for (int i = 0; i < (int) key.size(); i++) {
    rem[i] = quadro[i];
  }

  for (int j = (int) key.size(); j <= (int) quadro.size(); j++) {
    for (int i = 0; i < (int) key.size(); i++) {
      temp[i] = rem[i];
    }

    if (rem[0] == 0) {
      for (int i = 0; i < (int) key.size() - 1; i++) {
        rem[i] = temp[i+1];
      }
    } else {
      for (int i = 0; i < (int) key.size() - 1; i++) {
        rem[i] = (temp[i+1] ^ key[i+1]);
      }
    }

    if (j != (int) quadro.size()) {
      rem[(int) key.size() - 1] = quadro[j];
    }
  }

  for (int i = 0; i < (int) key.size() - 1; i++) {
    quadro[aux + i] = rem[i];
  }

  for (int i = 0; i < (int) key.size() -1; i++) {
    aux += rem[i];
  }

  int aux2 = 0;

  if (aux2 != 0) {
    cout << "Houve um erro na transmissão\n";
  } else {
    for (int i = 0; i < (int) key.size() - 1; i++) {
      quadro.pop_back();
    }
  }

  return quadro;
}

vector<int> CamadaEnlaceDadosReceptoraControleDeErroCodigoDeHamming (vector<int> quadro) {
  // cout << "Debug:" << endl;
  // for (int i = 0; i < (int) quadro.size(); i++) {
  //   if (i % 12 == 0) cout << " | ";
  //   cout << quadro[i] << " ";
  // }
  // cout << endl;

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