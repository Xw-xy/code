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
	CHESS_WHITE = 0,//����
	CHESS_BLACK = 1,//����
}chess_kind_t;

class Chess
{
public:
	Chess(int gradeSize, int marginX, int marginY, float chessSize);

	//���̳�ʼ�����������̵�ͼƬ��Դ����ʼ�����̵��������
	void init();

	//��ȡ���̵Ĵ�С
	int getGradeSize();

	//�������Ч���������Ч�����λ�ã��У��У������ڲ���pos��
	bool clickBoard(int x, int y, ChessPos *pos);

	//�����̵�ָ��λ�ã�pos�������ӣ�kind��
	void chessDown(ChessPos* pos,chess_kind_t kind);

	//��ȡָ��λ���Ǻ��廹�ǰ��壬���ǿհ�
	int getchessData(ChessPos*pos);
	int getchessData(int row,int col);

	//ʹ���Զ�����Ⱦ�ӿڣ����easyx����ͼƬʱ��͸��������ȾΪ��ɫ
	void putimagePNG(int x, int y, IMAGE* picture);

	//�ж�����Ƿ����
	bool checkOver();

private:
	IMAGE chessBlackImg; //��������
	IMAGE chessWhiteImg; //��������

	int gradeSixe; //���̵Ĵ�С
	int margin_x; //��ߵı߽�
	int margin_y; //�ұߵı߽�
	float chessSize;//���ӵĴ�С
	ChessPos  lastPos;
	//�洢��ǰ������ӷֲ����� 0���հ� 1������ -1������
	vector<vector<int>> chessMap;
private:
	void updateGameMap(ChessPos* pos);
	bool playerFlag;//true:������ flase:������
	bool checkWin();//����Ƿ�ʤ�������ʤ���Էַ���true
};

