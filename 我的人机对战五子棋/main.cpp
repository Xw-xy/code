/*
������־��
	1��������Ŀ�������ز�
	2�������Ŀ��ܣ������Щ�ࣩ
	3��
*/
#include<iostream>
#include"ChessGame.h"

int main(void)
{
	Man man;
	Chess chess(8,30,25,51);//�������̴�С���󲿣������߽�
	AI ai;
	ChessGame game(& man,& ai,& chess);

	game.play();
	return 0;
}