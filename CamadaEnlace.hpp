#ifndef __CAMADA_ENLACE_H__
#define __CAMADA_ENLACE_H__

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

void CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar (vector<int> quadro);
void CamadaEnlaceDadosTransmissoraControleDeErroCRC (vector<int> quadro);

void CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar (vector<int> quadro);
void CamadaEnlaceDadosReceptoraControleDeErroCRC (vector<int> quadro);

#endif 