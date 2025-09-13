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
	//��������
	initgraph(474, 471, EX_SHOWCONSOLE);

	//��ʾ����
	loadimage(0, "res/����.jpg");
	//��������
	//mciSendString("",0,0,0);

	loadimage(&chessBlackImg, "res/����.png", (int)chessSize,(int)chessSize, true);
	loadimage(&chessWhiteImg, "res/����.jpg",(int) chessSize,(int)chessSize, true);

	//��������
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
		//=���Ͻǵ��ж�
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

		//���Ͻ��ж�
		len = sqrt((x - leftTopPosX-chessSize) * (x - leftTopPosX-chessSize) + (y - leftTopPosY) * (y - leftTopPosY));
		
		if (len <= offset)
		{
			
			
			pos->row = row;
			pos->col = col+1;
			if (chessMap[pos->row][pos->col] == -1) {
				ret = true; break;
			}
			
		}


		//���½��ж�
		len = sqrt((x - leftTopPosX ) * (x - leftTopPosX ) + (y - leftTopPosY - chessSize) * (y - leftTopPosY - chessSize));
		
		if (len <= offset)
		{
			
			pos->row = row+1;
			pos->col = col;
		
			if (chessMap[pos->row][pos->col] == -1) {
				ret = true;  break;
			}
			
		}
		//���½��ж�
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
	
		// ������ʼ��
		DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
		DWORD* draw = GetImageBuffer();
		DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
		int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
		int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
		int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
		int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
		int dstX = 0;    //���Դ������صĽǱ�

		// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
		for (int iy = 0; iy < picture_height; iy++)
		{
			for (int ix = 0; ix < picture_width; ix++)
			{
				int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
				int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
				int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
				int sg = ((src[srcX] & 0xff00) >> 8);   //G
				int sb = src[srcX] & 0xff;              //B
				if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
				{
					dstX = (ix + x) + (iy + y) * graphWidth; //���Դ������صĽǱ�
					int dr = ((dst[dstX] & 0xff0000) >> 16);
					int dg = ((dst[dstX] & 0xff00) >> 8);
					int db = dst[dstX] & 0xff;
					draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
						| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
						| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
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

	//����λ��

	//��ֱ����
	const int directions[4][2] = { {0, 1},  // ˮƽ ��
								{1, 0},  // ��ֱ ��
								{1, 1},  // ��б�� �K
								{1, -1} }; // ��б�� �L

	/*for (const auto& dir : directions) AI�㷨���ȼ��ڣ�for (int i = 0; i < 4; ++i) {				 // ���� directions �����4������
															const int (&dir)[2] = directions[i];	// dir �ǶԶ�ά����Ԫ�ص�����
															
															directions[i] ��ʾȡ�� i �е� һά���飨������ int[2]����
															(2) int (&dir)[2] �ĺ���
															����������dir ��һ�� �Գ���Ϊ2��������������á�															
															�﷨�ṹ��															
															&dir����ʾ dir ��һ������															
															[2]����ʾ�����õĶ����ǳ���Ϊ2������															
															int������Ԫ������Ϊ int														
															(3) const ������
															ȷ��ͨ�� dir �����޸�ԭ��������ݣ�ֻ������									
															(4) ���庬��
															�� dir �󶨵� directions[i] ���һά���飬����ͨ�� dir[0] �� dir[1] ����ֱ�ӷ��ʷ��������
															    // ʹ�� dir[0] �� dir[1] ��������

		int dx = dir[0];
		int dy = dir[1];*/
		
		for (int i = 0; i < 4; i++) {
			int j = 0;
				int dx = directions[i][j];
				int dy = directions[i][j+1];
				int count = 1; // ��ǰ���ӱ�����1��

				// ����������
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

				// ����������
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
			//����Ӯ��
			/*mciSendString();//ʤ������
			loadimage();//����ʤ��ͼƬ
			*/
			cout << "ʤ��" << endl;
		}
		else {
			//����Ӯ��
			/*mciSendString();//ʧ������
			loadimage();//����ʧ��ͼƬ
			*/
			cout << "ʧ��" << endl;
		}
		_getch();		//��ͣ
		return true;

	}
	return false;
}
