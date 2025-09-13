#pragma once
#include <graphics.h>
#include<vector>
using namespace std;

struct ChessPos {
	int row;
	int col;
	ChessPos(int r=0,int c=0):row(r),col(c){}
};
typedef enum {
	CHESS_WHITE = 0,//白棋
	CHESS_BLACK = 1,//黑棋
}chess_kind_t;

class Chess
{
public:
	Chess(int gradeSize, int marginX, int marginY, float chessSize);

	//棋盘初始化：加载棋盘的图片资源，初始化棋盘的相关数据
	void init();

	//获取棋盘的大小
	int getGradeSize();

	//如果是有效点击，把有效点击的位置（行，列）保存在参数pos中
	bool clickBoard(int x, int y, ChessPos *pos);

	//在棋盘的指定位置（pos），落子（kind）
	void chessDown(ChessPos* pos,chess_kind_t kind);

	//获取指定位置是黑棋还是白棋，还是空白
	int getchessData(ChessPos*pos);
	int getchessData(int row,int col);

	//使用自定义渲染接口，解决easyx加载图片时将透明部分渲染为黑色
	void putimagePNG(int x, int y, IMAGE* picture);

	//判断棋局是否结束
	bool checkOver();

private:
	IMAGE chessBlackImg; //黑棋棋子
	IMAGE chessWhiteImg; //白棋棋子

	int gradeSixe; //棋盘的大小
	int margin_x; //左边的边界
	int margin_y; //右边的边界
	float chessSize;//棋子的大小
	ChessPos  lastPos;
	//存储当前棋局棋子分布数据 0：空白 1：黑子 -1：白子
	vector<vector<int>> chessMap;
private:
	void updateGameMap(ChessPos* pos);
	bool playerFlag;//true:黑子走 flase:白子走
	bool checkWin();//检查是否胜利，如果胜负以分返回true
};

