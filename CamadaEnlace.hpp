#ifndef __CAMADA_ENLACE_H__
#define __CAMADA_ENLACE_H__

#define CONTAGEM_CARACTERES 0
#define INSERCAO_BYTES 1

#define BIT_PARIDADE 0
#define CRC 1

using namespace std;

void CamadaEnlaceDadosTransmissora (vector<int> quadro);

vector<int> CamadaEnlaceDadosTransmissoraEnquadramento (vector<int> quadro);

vector<int> CamadaEnlaceDadosTransmissoraControleDeErro (vector<int> quadro);

vector<int> CamadaEnlaceDadosTransmissoraEnquadramentoContagemDeCaracteres (vector<int> quadro);

vector<int> CamadaEnlaceDadosTransmissoraEnquadramentoInsercaoDeBytes (vector<int> quadro);

void CamadaEnlaceDadosReceptora (vector<int> quadro);

vector<int> CamadaEnlaceDadosReceptoraEnquadramento (vector<int> quadro);

vector<int> CamadaEnlaceDadosReceptoraControleDeErro (vector<int> quadro);

vector<int> CamadaEnlaceDadosReceptoraEnquadramentoContagemDeCaracteres (vector<int> quadro);

vector<int> CamadaEnlaceDadosReceptoraEnquadramentoInsercaoDeBytes (vector<int> quadro);

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar (vector<int> quadro);
vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCRC (vector<int> quadro);

vector<int> CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar (vector<int> quadro);
vector<int> CamadaEnlaceDadosReceptoraControleDeErroCRC (vector<int> quadro);

#endif 