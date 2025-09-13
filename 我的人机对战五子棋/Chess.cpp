#include "Chess.h"
#include<mmsystem.h>
#include<math.h>
#include<iostream>
#include<conio.h>
//#pragma comment(lib,"winnm.lib")


Chess::Chess(int gradeSize, int marginX, int marginY, float chessSize)
{
	this->gradeSixe = gradeSize;
	this->margin_x = marginX;
	this->margin_y = marginY;
	this->chessSize = chessSize;
	playerFlag = CHESS_BLACK;

	for (int i = 0; i <=gradeSize; i++) {
		vector<int> row;
		for (int j = 0; j <=gradeSixe; j++) {
			row.push_back(-1);
		}
		chessMap.push_back(row);
	}
}

void Chess::init()
{
	//创建窗口
	initgraph(474, 471, EX_SHOWCONSOLE);

	//显示棋盘
	loadimage(0, "res/棋盘.jpg");
	//播放音乐
	//mciSendString("",0,0,0);

	loadimage(&chessBlackImg, "res/黑棋.png", (int)chessSize,(int)chessSize, true);
	loadimage(&chessWhiteImg, "res/白棋.jpg",(int) chessSize,(int)chessSize, true);

	//棋盘清零
	for (int i = 0; i <=gradeSixe; i++) {
		for (int j = 0; j <=gradeSixe; j++) {
			chessMap[i][j] = -1;
			//cout << chessMap[i][j] << " ";
		}
	}
	
}

int Chess::getGradeSize()
{
	return gradeSixe;
}

bool Chess::clickBoard(int x, int y, ChessPos* pos)
{
	int row= 0;
	int col = 0;

	col = (int)((x - margin_x) / chessSize);
	row = (int)((y - margin_y) / chessSize);

	double leftTopPosX  = (margin_x + chessSize * col);
	double leftTopPosY  = (margin_y + chessSize * row);
	double offset = (double)(chessSize * 0.2);
	
	double  len;

	bool ret = false;

	do
	{
		//=左上角的判断
		len = sqrt((x - leftTopPosX) * (x-leftTopPosX)+(y - leftTopPosY) * (y - leftTopPosY));
		
		if (len <offset)
		{
			pos->row = row;
			pos->col = col;
			if (chessMap[pos->row][pos->col ] ==-1) {
			ret = true;
			break;
			}
			
		}

		//右上角判断
		len = sqrt((x - leftTopPosX-chessSize) * (x - leftTopPosX-chessSize) + (y - leftTopPosY) * (y - leftTopPosY));
		
		if (len <= offset)
		{
			
			
			pos->row = row;
			pos->col = col+1;
			if (chessMap[pos->row][pos->col] == -1) {
				ret = true; break;
			}
			
		}


		//左下角判断
		len = sqrt((x - leftTopPosX ) * (x - leftTopPosX ) + (y - leftTopPosY - chessSize) * (y - leftTopPosY - chessSize));
		
		if (len <= offset)
		{
			
			pos->row = row+1;
			pos->col = col;
		
			if (chessMap[pos->row][pos->col] == -1) {
				ret = true;  break;
			}
			
		}
		//右下角判断
		len = sqrt((x - leftTopPosX - chessSize) * (x - leftTopPosX - chessSize) + (y - leftTopPosY - chessSize) * (y - leftTopPosY - chessSize));
	
		if (len <=offset)
		{
			
			pos->row = row + 1 ;
			pos->col = col + 1;
			
			if (chessMap[pos->row][pos->col] == -1) {
				ret = true;  break;
			}
			
		}
	} while (0);

	return ret;
	
}

void Chess::chessDown(ChessPos* pos, chess_kind_t kind)
{

	double  x = margin_x + chessSize * pos->col-0.5* chessSize;
	double  y = margin_y + chessSize * pos->row-0.5* chessSize;

	if (kind == CHESS_WHITE)
	{
		putimagePNG((int)x,(int)y,&chessWhiteImg);
	}
	else
	{
		putimagePNG((int)x,(int) y, &chessBlackImg);
	}
	updateGameMap(pos);
}

int Chess::getchessData(ChessPos* pos)
{
	return chessMap[pos->row][pos->col];
}

int Chess::getchessData(int row, int col)
{
	return chessMap[row][col];
}

void Chess::putimagePNG(int x, int y, IMAGE* picture)
{
	
		// 变量初始化
		DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
		DWORD* draw = GetImageBuffer();
		DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
		int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
		int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
		int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
		int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
		int dstX = 0;    //在显存里像素的角标

		// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
		for (int iy = 0; iy < picture_height; iy++)
		{
			for (int ix = 0; ix < picture_width; ix++)
			{
				int srcX = ix + iy * picture_width; //在显存里像素的角标
				int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
				int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
				int sg = ((src[srcX] & 0xff00) >> 8);   //G
				int sb = src[srcX] & 0xff;              //B
				if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
				{
					dstX = (ix + x) + (iy + y) * graphWidth; //在显存里像素的角标
					int dr = ((dst[dstX] & 0xff0000) >> 16);
					int dg = ((dst[dstX] & 0xff00) >> 8);
					int db = dst[dstX] & 0xff;
					draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
						| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
						| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
				}
			}
		}
	}

void Chess::updateGameMap(ChessPos* pos)
{
	this->chessMap[pos->row][pos->col] = this->playerFlag ? CHESS_BLACK : CHESS_WHITE;
	this->playerFlag = !this->playerFlag;
	lastPos = *pos;
}

bool Chess::checkWin()
{
	int row = lastPos.row;
	int col = lastPos.col;

	//落子位置

	//垂直方向
	const int directions[4][2] = { {0, 1},  // 水平 →
								{1, 0},  // 垂直 ↓
								{1, 1},  // 正斜线 ↘
								{1, -1} }; // 反斜线 ↙

	/*for (const auto& dir : directions) AI算法，等价于：for (int i = 0; i < 4; ++i) {				 // 遍历 directions 数组的4个方向
															const int (&dir)[2] = directions[i];	// dir 是对二维数组元素的引用
															
															directions[i] 表示取第 i 行的 一维数组（类型是 int[2]）。
															(2) int (&dir)[2] 的含义
															引用声明：dir 是一个 对长度为2的整型数组的引用。															
															语法结构：															
															&dir：表示 dir 是一个引用															
															[2]：表示被引用的对象是长度为2的数组															
															int：数组元素类型为 int														
															(3) const 的作用
															确保通过 dir 不能修改原数组的内容（只读）。									
															(4) 整体含义
															将 dir 绑定到 directions[i] 这个一维数组，后续通过 dir[0] 和 dir[1] 可以直接访问方向参数：
															    // 使用 dir[0] 和 dir[1] 操作方向

		int dx = dir[0];
		int dy = dir[1];*/
		
		for (int i = 0; i < 4; i++) {
			int j = 0;
				int dx = directions[i][j];
				int dy = directions[i][j+1];
				int count = 1; // 当前落子本身算1个

				// 正向延伸检查
				for (int i = 1; ; i++) {
					int x = row + dx * i;
					int y = col + dy * i;
					if (x < 0 || x >= gradeSixe ||
						y < 0 || y >= gradeSixe ||
						chessMap[x][y] != !playerFlag) {
						break;
					}
					count++;
				}

				// 反向延伸检查
				for (int i = 1; ; i++) {
					int x = row - dx * i;
					int y = col - dy * i;
					if (x < 0 || x >= gradeSixe ||
						y < 0 || y >= gradeSixe ||
						chessMap[x][y] != !playerFlag) {
						break;
					}
					count++;
				}

				if (count >= 5) {
					return true;
				}
			
	}

	return false;
}



bool Chess::checkOver()
{
	if (checkWin()) {
		if (playerFlag == false) {
			//黑棋赢棋
			/*mciSendString();//胜利音乐
			loadimage();//加载胜利图片
			*/
			cout << "胜利" << endl;
		}
		else {
			//白棋赢棋
			/*mciSendString();//失败音乐
			loadimage();//加载失败图片
			*/
			cout << "失败" << endl;
		}
		_getch();		//暂停
		return true;

	}
	return false;
}
