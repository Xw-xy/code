#include "AI.h"

void AI::init(Chess* chess)
{
	this->chess = chess;
	for (int i = 0; i < chess->getGradeSize(); i++)
	{
		vector<int> row;
		for (int j = 0; j < chess->getGradeSize(); j++)
		{
			row.push_back(0);
		}
		scoreMap.push_back(row);
	}
}

void AI::go()
{
	ChessPos pos = think();
	Sleep(1000);
	chess->chessDown(&pos, CHESS_WHITE);
}

void AI::calculateScore()
{


	for (int i = 0; i < scoreMap.size(); i++)
	{
		for (int j = 0; j < scoreMap.size(); j++)
		{
			scoreMap[i][j] = 0;
		}
	}
	int size = chess->getGradeSize();
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {

			//��ÿ������м���
			if (chess->getchessData(row, col) == -1) {

				for (int y = -1; y <= 1; y++) {
					for (int x = -1; x <= 1; x++) {
						if (x == 0 && y == 0) continue;
						if (y == 0 && x != 1)continue;

						int personNum = 0;	//���ַ������壩�ж������������
						int aiNum = 0;		//AI�������壩�ж��ٸ�����������
						int emptyNum = 0;	//�÷����Ͽհ�λ�ĸ���
						//��������ڸ�λ������
						for (int i = 1; i <= 4; i++)
						{
							int curRow = row + i * y;
							int curCol = col + i * x;

							if (curRow >= 0 && curRow < size && curCol >= 0 && curCol < size &&
								chess->getchessData(curRow, curCol) == 1)
							{
								personNum++;
							}
							else if (curRow >= 0 && curRow < size && curCol >= 0 && curCol < size &&
								chess->getchessData(curRow, curCol) == -1)
							{
								emptyNum++;
								break;
							}
							else {
								break;
							}

						}

						//�����������
						for (int i = 1; i <= 4; i++)
						{
							int curRow = row - i * y;
							int curCol = col - i * x;

							if (curRow >= 0 && curRow < size && curCol >= 0 && curCol < size &&
								chess->getchessData(curRow, curCol) == 1)
							{
								personNum++;
							}
							else if (curRow >= 0 && curRow < size && curCol >= 0 && curCol < size &&
								chess->getchessData(curRow, curCol) == -1)
							{
								emptyNum++;
								break;
							}
							else
							{
								break;
							}

						}

						if (personNum == 1) {//����
							scoreMap[row][col] += 10;
						}
						else if (personNum == 2) {
							if (emptyNum == 1)
							{
								scoreMap[row][col] += 30;
							}
							else if (emptyNum == 1)
							{
								scoreMap[row][col] += 40;
							}
						}
						else if (personNum == 3) {
							if (emptyNum == 1)
							{
								scoreMap[row][col] += 60;
							}
							else if (emptyNum == 2)
							{
								scoreMap[row][col] += 200;
							}
						}
						else if (personNum == 4)
						{
							scoreMap[row][col] += 20000;
						}

						//��������ڸ�λ������
						emptyNum = 0;

						for (int i = 1; i <= 4; i++)
						{
							int curRow = row + i * y;
							int curCol = row + i * x;
							if (curRow >= 0 && curRow < size && curCol >= 0 &&
								curCol < size && chess->getchessData(curRow, curCol) == 0)
							{
								aiNum++;
							}
							else if (curRow >= 0 && curRow < size && curCol >= 0 &&
								curCol < size && chess->getchessData(curRow, curCol) ==-1) {
								emptyNum++;
								break;
							}
							else {
								break;
							}
						}
						//������
						for (int i = 1; i <= 4; i++)
						{
							int curRow = row - i * y;
							int curCol = row - i * x;
							if (curRow >= 0 && curRow < size && curCol >= 0 &&
								curCol < size && chess->getchessData(curRow, curCol) == 0)
							{
								aiNum++;
							}
							else if (curRow >= 0 && curRow < size && curCol >= 0 &&
								curCol < size && chess->getchessData(curRow, curCol) == -1) {
								emptyNum++;
								break;
							}
							else {
								break;
							}
						}
						if (aiNum == 0) {
							scoreMap[row][col] += 5;
						}
						else if (aiNum == 2) {
							if (emptyNum == 1) {
								scoreMap[row][col] += 25;
							}
							else if (emptyNum == 1) {
								scoreMap[row][col] += 50;
							}
						}
						else if (aiNum == 3) {
							if (emptyNum == 1) {
								scoreMap[row][col] += 55;
							}
							else if (emptyNum == 2) {
								scoreMap[row][col] += 10000;
							}
							else if (aiNum >= 4) {
								scoreMap[row][col] += 30000;
							}
						}

					}
				}
			}
		}
	}
}

ChessPos AI::think()
{
	calculateScore();
	vector<ChessPos> maxPoints;
	int maxSore = 0;
	int size = chess->getGradeSize();
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++)
		{
			if (chess->getchessData(row, col) == -1) {
				if (scoreMap[row][col] > maxSore) {
					maxSore = scoreMap[row][col];
					maxPoints.clear();
					maxPoints.push_back(ChessPos(row, col));
				}
				else if (scoreMap[row][col] == maxSore) {
					
					maxPoints.push_back(ChessPos(row, col));
				}
			}
		}
	}
	int index = rand() % maxPoints.size();
	return maxPoints[index];
}

