#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "campominado.h"
#include "ia.h"

#define NUM_REPETICOES 10000

struct campo_minado_private_t
{
	bool inicializado;
	campo_minado_estatisticas_t estatisticas;
	signed char** tabuleiro_solucao;
};

typedef struct
{
	int linhas, colunas, minas;
} parametros_t;

void campo_minado_erro_fatal(erros_t erro)
{
	const char* msg_erro[4] =
	{
		"",
		"ERRO_MAIS_MINAS_QUE_CASAS",
		"ERRO_LINHA_OU_COLUNA_INVALIDA",
		"ERRO_TENTATIVA_DE_MARCAR_CASA_INVALIDA"
	};

	if ((int)erro >= 1 && (int)erro <= 3)
		printf("Seu programa cometeu um erro fatal, abortando: %s\n", msg_erro[(int)erro]);
	else
		printf("Erro desconhecido\n");

	exit((int)erro);
}

int main()
{
	parametros_t parametros[3] =
	{
		{ .linhas =  8, .colunas =  8, .minas = 10 },
		{ .linhas = 16, .colunas = 16, .minas = 40 },
		{ .linhas = 16, .colunas = 30, .minas = 99 },
	};

	campo_minado_estatisticas_t estatisticas[3] =
	{
		{ .fracao_minas_corretamente_marcadas = 0.0, .fracao_minas_incorretamente_marcadas = 0.0, .fracao_casas_abertas = 0.0 },
		{ .fracao_minas_corretamente_marcadas = 0.0, .fracao_minas_incorretamente_marcadas = 0.0, .fracao_casas_abertas = 0.0 },
		{ .fracao_minas_corretamente_marcadas = 0.0, .fracao_minas_incorretamente_marcadas = 0.0, .fracao_casas_abertas = 0.0 },
	};

	const char* nomes_modos[3] =
	{
		"iniciante",
		"intermediario",
		"avancado"
	};

	double fracao_vitorias[3] = { 0.0, 0.0, 0.0 };

	int i, j;
    
	srand((unsigned int)time(NULL));

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < NUM_REPETICOES; j++)
		{
			campo_minado_t c;

			campo_minado_inicializa(&c, parametros[i].linhas, parametros[i].colunas, parametros[i].minas);

			while (1)
			{
				bool abortar = false;
				int linha = -1, coluna = -1;
				status_jogada_t status_jogada;

				campo_minado_ia(&c, &linha, &coluna);

				status_jogada = campo_minado_faz_jogada(&c, linha, coluna);
				switch (status_jogada)
				{
					case JOGADA_EXPLODIU:
						abortar = true;
						break;

					case JOGADA_GANHOU:
						fracao_vitorias[i]++;
						abortar = true;
						break;

					case JOGADA_POSICAO_INVALIDA:
						campo_minado_erro_fatal(ERRO_LINHA_OU_COLUNA_INVALIDA);
						break;

					default:
						break;
				}

				if (abortar)
				{
					if (c._private->estatisticas.fracao_casas_abertas < 0 || c._private->estatisticas.fracao_casas_abertas > 1)
						i++;
					estatisticas[i].fracao_casas_abertas += c._private->estatisticas.fracao_casas_abertas;
					estatisticas[i].fracao_minas_corretamente_marcadas += c._private->estatisticas.fracao_minas_corretamente_marcadas;
					estatisticas[i].fracao_minas_incorretamente_marcadas += c._private->estatisticas.fracao_minas_incorretamente_marcadas;
					break;
				}
			}

			campo_minado_destroi(&c);
		}

		fracao_vitorias[i] /= NUM_REPETICOES;
		estatisticas[i].fracao_casas_abertas /= NUM_REPETICOES;
		estatisticas[i].fracao_minas_corretamente_marcadas /= NUM_REPETICOES;
		estatisticas[i].fracao_minas_incorretamente_marcadas /= NUM_REPETICOES;

		printf(
			"Estatisticas modo %s:\nVitorias: %.2lf%%\nCasas abertas: %.2lf%%\nMinas corretamente marcadas: %.2lf%%\nMinas incorretamente marcadas %.2lf%%\n\n",
			nomes_modos[i],
			fracao_vitorias[i]*100.0,
			estatisticas[i].fracao_casas_abertas*100.0,
			estatisticas[i].fracao_minas_corretamente_marcadas*100.0,
			estatisticas[i].fracao_minas_incorretamente_marcadas*100.0
		);
	}
	
	return 0;
}