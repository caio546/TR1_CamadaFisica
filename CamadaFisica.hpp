#ifndef __CAMADA_FISICA_H__
#define __CAMADA_FISICA_H__

#define BINARIA 0
#define MANCHESTER 1
#define BIPOLAR 2

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

void AplicacaoTransmissora (void);

void CamadaDeAplicacaoTransmissora (string mensagem);

void CamadaFisicaTransmissora (vector<int> quadro);

vector<int> CamadaFisicaTransmissoraCodificacaoBinaria (vector<int> quadro);
vector<int> CamadaFisicaTransmissoraCodificacaoManchester (vector<int> quadro);
vector<int> CamadaFisicaTransmissoraCodificacaoBipolar(vector<int> quadro);

void MeioDeComunicacao (vector<int> fluxoBrutoDeBits);

void CamadaFisicaReceptora (vector<int> quadro);

vector<int> CamadaFisicaReceptoraDecodificacaoBinaria(vector<int> quadro);
vector<int> CamadaFisicaReceptoraDecodificacaoManchester(vector<int> quadro);
vector<int> CamadaFisicaReceptoraDecodificacaoBipolar(vector<int> quadro);

void CamadaDeAplicacaoReceptora(vector<int> quadro);

void AplicacaoReceptora(string mensagem);

void iniciarGUI();

void transmitirInformacao(vector<int> bits);

void exibirPositivo(int shift);
void exibirZero(int shift);
void exibirNegativo(int shift);

#endif 