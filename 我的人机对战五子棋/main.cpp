/*
开发日志：
	1、创建项目、导入素材
	2、设计项目框架（设计那些类）
	3、
*/
#include<iostream>
#include"ChessGame.h"

int main(void)
{
	Man man;
	Chess chess(8,30,25,51);//传入棋盘大小。左部，顶部边界
	AI ai;
	ChessGame game(& man,& ai,& chess);

	game.play();
	return 0;
}