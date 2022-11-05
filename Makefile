# This is a makefile for rate.c ... .... ... (add outros se tiver)
# Made by Matheus Pereira do Rego Barros (mprb) due to second implementation for Infra SW discipline.

	$(CC) = gcc

shell:
	$(CC) rate.c -o rate

functions.h:
	$(CC) functions.c -o functions

clean:
		rm rate rate_mprb.out

#PERGUNTAR SE PRECISA EXCLUIR O RESTO!! OU SO O SHELL EM QUESTAO!