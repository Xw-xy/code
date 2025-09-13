#include "Man.h"
#include<stdio.h>
#include<iostream>
#include"graphics.h"
#include <conio.h>
using namespace std;
void Man::init(Chess* chess)
{
	this->chess = chess;
}

void Man::go()
{
	MOUSEMSG msg;
	ChessPos pos(0,0);
	while (1)
	{
		//获取鼠标点击消息
		
			msg = GetMouseMsg();
			
			if (msg.uMsg == WM_LBUTTONDOWN && chess->clickBoard(msg.x, msg.y, &pos)) {

				break;
			}

	}
	printf("%d,%d\n",pos.row,pos.col);
	//落子
	chess->chessDown(&pos, CHESS_BLACK);
	//closegraph();
	
}
