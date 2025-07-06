#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>

const int cSide = 3;
const int cPlusOne[] = {0, 0, 1, -1};
const int cPlusTwo[] = {-1, 1, 0, 0};
const int cShest = 6;
const int cSem = 7;
const int cVosem = 8;

class State {
 public:
  std::vector<std::vector<int>> board;
  int x;
  int y;
  int depth;
  std::string moves;
  char prev = ',';
  State(const std::vector<std::vector<int>>& b, int x, int y, int d,
        const std::string& moves, char prev)
      : board(b), x(x), y(y), depth(d), moves(moves), prev(prev) {}
  bool IsReady() {
    return (board[0][0] == 1 && board[0][1] == 2 && board[0][2] == 3 &&
            board[1][0] == 4 && board[1][1] == 5 && board[1][2] == cShest &&
            board[2][0] == cSem && board[2][1] == cVosem);
  }
};

void BFS(const std::vector<std::vector<int>>& start, int x, int y) {
  std::queue<State> q;
  std::set<std::vector<std::vector<int>>> visited;

  q.push(State(start, x, y, 0, "", ','));
  visited.insert(start);

  while (!q.empty()) {
    State curr = q.front();
    q.pop();

    if (curr.IsReady()) {
      std::cout << curr.depth << '\n';
      std::cout << curr.moves << '\n';
      return;
    }

    for (int i = 0; i < 4; i++) {
      std::string new_str = curr.moves;
      char prev;
      int new_x = curr.x + cPlusOne[i];
      int new_y = curr.y + cPlusTwo[i];
      if (-1 < new_x && new_x < 3 && -1 < new_y && new_y < 3) {
        if (cPlusOne[i] == 1 && curr.prev == 'U') {
          continue;
        }
        if (cPlusOne[i] == -1 && curr.prev == 'D') {
          continue;
        }
        if (cPlusTwo[i] == 1 && curr.prev == 'L') {
          continue;
        }
        if (cPlusTwo[i] == -1 && curr.prev == 'R') {
          continue;
        }
        if (cPlusOne[i] == 1) {
          new_str += "D";
          prev = 'D';
        }
        if (cPlusOne[i] == -1) {
          new_str += "U";
          prev = 'U';
        }
        if (cPlusTwo[i] == 1) {
          new_str += "R";
          prev = 'R';
        }
        if (cPlusTwo[i] == -1) {
          new_str += "L";
          prev = 'L';
        }
        std::vector<std::vector<int>> new_board(curr.board);
        std::swap(new_board[curr.x][curr.y], new_board[new_x][new_y]);
        if (visited.find(new_board) == visited.end()) {
          visited.insert(new_board);
          q.push(State(new_board, new_x, new_y, curr.depth + 1, new_str, prev));
        }
      }
    }
  }

  std::cout << "-1" << '\n';
}

int main() {
  int x;
  int y;
  std::vector<std::vector<int>> matrix(cSide, std::vector<int>(cSide));
  for (int i = 0; i != cSide; ++i) {
    for (int j = 0; j != cSide; ++j) {
      std::cin >> matrix[i][j];
      if (matrix[i][j] == 0) {
        x = i;
        y = j;
      }
    }
  }

  BFS(matrix, x, y);
}