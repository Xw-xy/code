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

			//对每个点进行计算
			if (chess->getchessData(row, col) == -1) {

				for (int y = -1; y <= 1; y++) {
					for (int x = -1; x <= 1; x++) {
						if (x == 0 && y == 0) continue;
						if (y == 0 && x != 1)continue;

						int personNum = 0;	//棋手方（黑棋）有多个连续的棋子
						int aiNum = 0;		//AI方（白棋）有多少个连续的棋子
						int emptyNum = 0;	//该方向上空白位的个数
						//假设黑棋在该位置走子
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

						//反向继续计算
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

						if (personNum == 1) {//连二
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

						//假设白棋在该位置走子
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
						//反方向
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

