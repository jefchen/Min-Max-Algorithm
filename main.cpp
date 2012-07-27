/*
 * main.cpp
 *
 *  Created on: May 27, 2012
 *      Author: jeffreychen_55
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int POS_INF = 100;
const int NEG_INF = -100;
const int depth = 4;

enum Player { black, white, empty };

struct Space {
	int value;
	Player player;
};

struct MoveVector {
	int x;
	int y;
	int x_dir;
	int y_dir;

	MoveVector() {}
	MoveVector(int x, int y, int x_dir, int y_dir) : x(x), y(y), x_dir(x_dir), y_dir(y_dir) {
		// initialize
	}
	/*
	MoveVector& operator=(MoveVector& copy) {
		this->x = copy.x;
		this->y = copy.y;
		this->x_dir = copy.x_dir;
		this->y_dir = copy.y_dir;
		return *this;
	}
	*/
};

struct Move {
	bool hasMove;
	struct MoveVector move;
};

void makeMove(int x, int y, int x_dir, int y_dir, struct Space board[4][4]) {
	Player player = board[x][y].player;

	// find blocked position
	int last_x = x;
	int last_y = y;
	while (true) {
		last_x += x_dir;
		last_y += y_dir;
		if (last_x < 0 || last_y < 0 || last_x >= 4 || last_y >= 4) break;
		if (board[last_x][last_y].player != player && board[last_x][last_y].player != empty) break;
	};

	// check if its blocked
	if (abs(last_x-x) <= 1 && abs(last_y-y) <= 1) {
		// there is no move
		return;
	}

	// move the pieces
	if (abs(last_x-x) == 2 || abs(last_y-y) == 2) {
		// move everything into the first block
		board[x+x_dir][y+y_dir].player = board[x][y].player;
		board[x+x_dir][y+y_dir].value += board[x][y].value;
		board[x][y].player = empty;
		board[x][y].value = 0;
	} else if (abs(last_x-x) == 3 || abs(last_y-y) == 3) {
		// move one into the first block
		board[x+x_dir][y+y_dir].player = board[x][y].player;
		board[x+x_dir][y+y_dir].value += 1;
		board[x][y].value -= 1;

		// then move everything into the second block
		if (board[x][y].value != 0) {
			board[x+2*x_dir][y+2*y_dir].player = board[x][y].player;
			board[x+2*x_dir][y+2*y_dir].value += board[x][y].value;
		}

		board[x][y].player = empty;
		board[x][y].value = 0;
	} else {
		// move one into the first block
		board[x+x_dir][y+y_dir].player = board[x][y].player;
		board[x+x_dir][y+y_dir].value += 1;
		board[x][y].value -= 1;

		// then move two into the second block, and if not enough, move one only
		if (board[x][y].value >= 2) {
			board[x+2*x_dir][y+2*y_dir].player = board[x][y].player;
			board[x+2*x_dir][y+2*y_dir].value += 2;
			board[x][y].value -= 2;
		}
		else if (board[x][y].value == 1) {
			board[x+2*x_dir][y+2*y_dir].player = board[x][y].player;
			board[x+2*x_dir][y+2*y_dir].value += 1;
			board[x][y].value = 0;;
		}

		// then the rest to the last block
		if (board[x][y].value != 0) {
			board[last_x-x_dir][last_y-y_dir].player = board[x][y].player;
			board[last_x-x_dir][last_y-y_dir].value += board[x][y].value;
		}

		board[x][y].player = empty;
		board[x][y].value = 0;
	}
}

vector<MoveVector> getMoveList(struct Space board[4][4], Player player) {
	vector<MoveVector> moveList;

	// collect the list of moves
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			if (board[i][j].player != player) continue;
			if (i-1 >= 0 && j-1 >= 0 && (board[i-1][j-1].player == empty || board[i-1][j-1].player == player)) moveList.push_back(MoveVector(i,j,-1,-1));
			if (i-1 >= 0 && (board[i-1][j].player == empty || board[i-1][j].player == player)) moveList.push_back(MoveVector(i,j,-1,0));
			if (i-1 >= 0 && j+1 < 4 && (board[i-1][j+1].player == empty || board[i-1][j+1].player == player)) moveList.push_back(MoveVector(i,j,-1,1));
			if (j-1 >= 0 && (board[i][j-1].player == empty || board[i][j-1].player == player)) moveList.push_back(MoveVector(i,j,0,-1));
			if (j+1 < 4 && (board[i][j+1].player == empty || board[i][j+1].player == player)) moveList.push_back(MoveVector(i,j,0,1));
			if (i+1 < 4 && j-1 >= 0 && (board[i+1][j-1].player == empty || board[i+1][j-1].player == player)) moveList.push_back(MoveVector(i,j,1,-1));
			if (i+1 < 4 && (board[i+1][j].player == empty || board[i+1][j].player == player)) moveList.push_back(MoveVector(i,j,1,0));
			if (i+1 < 4 && j+1 < 4 && (board[i+1][j+1].player == empty || board[i+1][j+1].player == player)) moveList.push_back(MoveVector(i,j,1,1));
		}
	}

	return moveList;
}

struct Move whitemove(struct Space board[4][4]) {
	// get list of moves
	vector<MoveVector> moveList = getMoveList(board, white);
	Move move;

	// initialize random seed
	srand(time(NULL));

	// randomly pick a move if there is one
	if (moveList.size() == 0) {
		move.hasMove = false;
		return move;
	} else {
		MoveVector randomMove = moveList[rand() % moveList.size()];
		makeMove(randomMove.x, randomMove.y, randomMove.x_dir, randomMove.y_dir, board);
		move.hasMove = true;
		move.move = randomMove;
		return move;
	}
}

void deepCopy(struct Space newBoard[4][4], struct Space board[4][4]) {
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			newBoard[i][j].player = board[i][j].player;
			newBoard[i][j].value = board[i][j].value;
		}
	}
}

void printBoard(struct Space board[4][4]) {
	cout << "******************" << endl;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			cout << board[i][j].value;
			if (board[i][j].player == black) {
				cout << "b, ";
			} else if (board[i][j].player == white) {
				cout << "w, ";
			} else {
				cout << ",  ";
			}
		}
		cout << endl;
	}
	cout << "******************" << endl;
}

void printFlagBoard(int board[4][4]) {
	cout << "******************" << endl;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			cout << board[i][j] << ", ";
		}
		cout << endl;
	}
	cout << "******************" << endl;
}

int distance(struct Space board[4][4], int x, int y, Player player) {
	int distance = 0;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			if (board[4][4].player == player) {
				distance += max(abs(x-i), abs(y-j));
			}
		}
	}
	return distance;
}

int evaluate(struct Space board[4][4]) {
	//cout << "evaluating the move" << endl;
	int blackFlagBoard[4][4] = {0};
	int whiteFlagBoard[4][4] = {0};

	//cout << "CLEARED ->" << endl;
	//printBoard(board);
	//printFlagBoard(blackFlagBoard);

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			if (board[i][j].player == black) {
				if (i-1 >= 0 && j-1 >= 0 && board[i-1][j-1].player == empty) blackFlagBoard[i-1][j-1] = 1;
				if (i-1 >= 0 && board[i-1][j].player == empty) blackFlagBoard[i-1][j] = 1;
				if (i-1 >= 0 && j+1 < 4 && board[i-1][j+1].player == empty) blackFlagBoard[i-1][j+1] = 1;
				if (j-1 >= 0 && board[i][j-1].player == empty) blackFlagBoard[i][j-1] = 1;
				if (j+1 < 4 && board[i][j+1].player == empty) blackFlagBoard[i][j+1] = 1;
				if (i+1 < 4 && j-1 >= 0 && board[i+1][j-1].player == empty) blackFlagBoard[i+1][j-1] = 1;
				if (i+1 < 4 && board[i+1][j].player == empty) blackFlagBoard[i+1][j] = 1;
				if (i+1 < 4 && j+1 < 4 && board[i+1][j+1].player == empty) blackFlagBoard[i+1][j+1] = 1;
			} else if (board[i][j].player == white) {
				if (i-1 >= 0 && j-1 >= 0 && board[i-1][j-1].player == empty) whiteFlagBoard[i-1][j-1] = 1;
				if (i-1 >= 0 && board[i-1][j].player == empty) whiteFlagBoard[i-1][j] = 1;
				if (i-1 >= 0 && j+1 < 4 && board[i-1][j+1].player == empty) whiteFlagBoard[i-1][j+1] = 1;
				if (j-1 >= 0 && board[i][j-1].player == empty) whiteFlagBoard[i][j-1] = 1;
				if (j+1 < 4 && board[i][j+1].player == empty) whiteFlagBoard[i][j+1] = 1;
				if (i+1 < 4 && j-1 >= 0 && board[i+1][j-1].player == empty) whiteFlagBoard[i+1][j-1] = 1;
				if (i+1 < 4 && board[i+1][j].player == empty) whiteFlagBoard[i+1][j] = 1;
				if (i+1 < 4 && j+1 < 4 && board[i+1][j+1].player == empty) whiteFlagBoard[i+1][j+1] = 1;
			}
		}
	}

	//cout << "BLACK ->" << endl;
	//printFlagBoard(blackFlagBoard);
	//cout << "WHITE ->" << endl;
	//printFlagBoard(whiteFlagBoard);

	int whiteResult = 0;
	int blackResult = 0;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			blackResult += blackFlagBoard[i][j];
			whiteResult += whiteFlagBoard[i][j];
		}
	}

	/*
	int whiteResult = 0;
	int blackResult = 0;
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			if (blackFlagBoard[i][j] == 1) {
				blackResult += distance(board, i, j, white);
			}
			if (whiteFlagBoard[i][j] == 1) {
				whiteResult += distance(board, i, j, black);
			}
		}
	}
	*/


	//printBoard(board);
	//cout << "evaluation is: whiteResult=" << whiteResult << ", blackResult=" << blackResult << endl;
	if (whiteResult == 0) {
		return POS_INF;
	} else if (blackResult == 0) {
		return NEG_INF;
	} else {
		return blackResult-whiteResult;
	}
}

int evaluateMove(struct Space board[4][4], struct MoveVector newMove, int numMove, Player player, int prune) {

	// make a copy of the board and make the move
	struct Space newBoard[4][4];
	deepCopy(newBoard, board);
	makeMove(newMove.x, newMove.y, newMove.x_dir, newMove.y_dir, newBoard);
    //printBoard(newBoard);

	if (numMove == depth) {
		//cout << "depth reached" << endl;
		return evaluate(newBoard);
	}

	vector<MoveVector> moveList = getMoveList(newBoard, player);
	if (moveList.size() == 0) {
		return evaluate(newBoard);
	}

	// traverse the tree (depth-first)
	int result = (player == black ? NEG_INF : POS_INF);
	Player nextPlayer = (player == black ? white : black);
	int temp = 0;
	for (int i=0; i<moveList.size(); i++) {
		temp = evaluateMove(newBoard, moveList[i], numMove+1, nextPlayer, result);
		if (player == white) {
			// min node
			if (temp < prune) {
				// prune the node
				return temp;
			} else if (temp < result) {
				result = temp;
			}
		} else {
			// max node
			if (temp > prune) {
				// prune the node
				return temp;
			} else if (temp > result) {
				result = temp;
			}
		}
	}

	return result;
}


struct Move blackmove(struct Space board[4][4]) {
	vector<MoveVector> moveList = getMoveList(board, black);
	MoveVector bestMove;
	Move move;
	int result = NEG_INF;
	int temp;

	// check if there is any move left to do
	if (moveList.size() == 0) {
		//cout << "black has no move" << endl;
		move.hasMove = false;
		return move;
	}

	// get the best move
	for (int i=0; i<moveList.size(); i++) {
		temp = evaluateMove(board, moveList[i], 0, black, NEG_INF);
		if (temp > result) {
			bestMove = moveList[i];
			result = temp;
		}
	}

	// make the move
	makeMove(bestMove.x, bestMove.y, bestMove.x_dir, bestMove.y_dir, board);

	move.hasMove = true;
	move.move = bestMove;
	return move;
}

int main() {

	struct Space board[4][4];

	// initialize
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			board[i][j].player = empty;
			board[i][j].value = 0;
		}
	}
	board[0][0].player = black;
	board[0][0].value = 10;
	board[3][3].player = white;
	board[3][3].value = 10;

	cout << "GAME STARTED" << endl;

	// black and white rotate make move
	struct Move move;
	int moveCount = 0;
	while (true)
	{
		move = blackmove(board);
		moveCount++;
		if (move.hasMove) {
			cout << "black moved: x=" << move.move.x << ", y=" << move.move.y << ", x_dir=" << move.move.x_dir  << ", y_dir=" << move.move.y_dir << endl;
		} else {
			cout << "black has no move left" << endl;
			break;
		}
		printBoard(board);

		move = whitemove(board);
		moveCount++;
		if (move.hasMove) {
			cout << "white moved: x=" << move.move.x << ", y=" << move.move.y << ", x_dir=" << move.move.x_dir  << ", y_dir=" << move.move.y_dir << endl;
		} else {
			cout << "white has no move left" << endl;
			break;
		}
		printBoard(board);
	}

	cout << "GAME ENDED: moveCount=" << moveCount << endl;
	return 0;
}

