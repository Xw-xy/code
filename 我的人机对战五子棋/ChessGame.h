#pragma once
#include"Man.h"
#include"Chess.h"
#include"AI.h"
class ChessGame
{
public:
	//��ʼ�Ծ�
	ChessGame(Man* man, AI* ai, Chess* chess);
	void play();
	//������ݳ�Ա
private:
	Man* man;
	AI* ai;
	Chess* chess;
};

