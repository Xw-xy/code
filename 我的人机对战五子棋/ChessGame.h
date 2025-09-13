#pragma once
#include"Man.h"
#include"Chess.h"
#include"AI.h"
class ChessGame
{
public:
	//开始对局
	ChessGame(Man* man, AI* ai, Chess* chess);
	void play();
	//添加数据成员
private:
	Man* man;
	AI* ai;
	Chess* chess;
};

