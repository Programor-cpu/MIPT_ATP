#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

const unsigned long long cOne = 1;
const unsigned long long cWin = 1147797409030816545;
const unsigned long long cMinusOne = -1;

const short int cPlus[] = {4, -4, 1, -1};
const short int cShestd = 60;
const short int cShestnad = 16;
const short int cSem = 7;
const short int cFive = 5;
const short int cOd = 11;
const short int cVos = 8;
const short int cNine = 9;
const short int cDv = 12;

class Mask {
 private:
  unsigned long long num_ = 0;

 public:
  bool Ready() const { return (num_ == cWin); }
  short int operator[](const unsigned long long& index) const {
    unsigned long long divider = cOne << (4 * index);
    return (num_ / divider) % (1 << 4);
  }
  Mask(const unsigned long long& num) : num_(num) {}
  Mask& operator+=(const unsigned long long& another) {
    num_ += another;
    return *this;
  }
  void SwapNumbers(const unsigned long long& one,
                   const unsigned long long& two) {
    unsigned long long first = (*this)[one];
    unsigned long long second = (*this)[two];
    num_ -= (first << (4 * one));
    num_ -= (second << (4 * two));
    num_ += (second << (4 * one));
    num_ += (first << (4 * two));
  }
  unsigned long long GetMask() const { return num_; }
  bool operator<(const Mask& another) const { return (num_ < another.num_); }
};

short int Angle(short int evristics, const Mask& board) {
  if (board[3] != 4 && (board[2] == 3 || board[cSem] == cVos)) {
    evristics += 2;
  }
  if (board[0] != 1 && (board[1] == 2 || board[4] == cFive)) {
    ++evristics;
  }
  if (board[cDv] != cDv + 1 &&
      (board[cDv + 1] == cDv + 1 || board[cVos] == cNine)) {
    ++evristics;
  }
  return evristics;
}

short int Manhatten(const Mask& board) {
  short int evristics = 0;
  for (short int i = 0; i != 4; ++i) {
    for (short int j = 0; j != 4; ++j) {
      if ((board[i * 4 + j]) != 0) {
        evristics += ((std::abs(j - (board[i * 4 + j] - 1) % 4) +
                       std::abs(i - (board[i * 4 + j] - 1) / 4)));
      }
    }
  }
  for (short int i = 0; i != 4; ++i) {
    short int m = -1;
    for (short int j = 0; j != 4; ++j) {
      if (board[i * 4 + j] != 0 && i == (board[i * 4 + j] - 1) / 4) {
        if (m >= board[i * 4 + j]) {
          evristics += 2;
        } else {
          m = board[i * 4 + j];
        }
      }
    }
  }
  for (short int j = 0; j != 4; ++j) {
    short int m = -1;
    for (short int i = 0; i != 4; ++i) {
      if (board[i * 4 + j] != 0 && j == (board[i * 4 + j] + 3) % 4) {
        if (m >= board[i * 4 + j]) {
          evristics += 2;
        } else {
          m = board[i * 4 + j];
        }
      }
    }
  }
  return Angle(evristics, board);
}

short int GetDepth(
    unsigned long long now,
    std::unordered_map<unsigned long long, unsigned long long>& visited) {
  short depth = 0;
  while (visited[now] != cMinusOne) {
    ++depth;
    now = visited[now];
  }
  return depth;
}

short int FindZero(const Mask& watch) {
  short int x = -1;
  for (short int i = 0; i != cShestnad; ++i) {
    if (watch[i] == 0) {
      x = i;
      break;
    }
  }
  return x;
}

std::string PrintAnswer(
    std::unordered_map<unsigned long long, unsigned long long>& visited) {
  std::string answer;
  unsigned long long start = cWin;
  while (visited[start] != cMinusOne) {
    short int delta = FindZero(Mask(visited[start])) - FindZero(Mask(start));
    if (delta == 4) {
      answer = 'D' + answer;
    }
    if (delta == -4) {
      answer = 'U' + answer;
    }
    if (delta == 1) {
      answer = 'R' + answer;
    }
    if (delta == -1) {
      answer = 'L' + answer;
    }
    start = visited[start];
  }
  return answer;
}

void BFS(const Mask& start) {
  std::vector<std::pair<int short, std::string>> possible_answers;
  std::priority_queue<
      std::pair<std::pair<short int, short int>, Mask>,
      std::vector<std::pair<std::pair<short int, short int>, Mask>>,
      std::greater<>>
      pq;
  std::unordered_map<unsigned long long, unsigned long long> visited;
  pq.push({{0, Manhatten(start)}, start});
  visited[start.GetMask()] = -1;

  while (!pq.empty() && (int)possible_answers.size() < 2) {
    Mask curr(pq.top().second);
    pq.pop();
    short int depth = GetDepth(curr.GetMask(), visited);
    if (curr.Ready()) {
      std::cout << depth << '\n';
      std::cout << PrintAnswer(visited) << '\n';
      return;
    }
    short int x = FindZero(curr);

    for (int i = 0; i != 4; ++i) {
      char new_char = '.';
      short int new_x = x + cPlus[i];
      if (-1 < new_x && new_x < cShestnad) {
        if (cPlus[i] == 4) {
          new_char = 'U';
        }
        if (cPlus[i] == -4) {
          new_char = 'D';
        }
        if (cPlus[i] == 1 && (x != 3 && x != cSem && x != cOd)) {
          new_char = 'L';
        }
        if (cPlus[i] == -1 && (x != 4 && x != cVos && x != cDv)) {
          new_char = 'R';
        }
        if (new_char == '.') {
          continue;
        }
        Mask new_board(curr);
        new_board.SwapNumbers(x, (new_x - 0));
        if (!visited.contains(new_board.GetMask())) {
          visited[new_board.GetMask()] = curr.GetMask();
          if (depth < cShestd + 1) {
            pq.push({{Manhatten(new_board) + depth + 1, depth + 1}, new_board});
          }
        }
      }
    }
  }
}

int main() {
  unsigned long long in;
  Mask matrix(0);
  for (unsigned long long i = 0; i != 4; ++i) {
    for (unsigned long long j = 0; j != 4; ++j) {
      std::cin >> in;
      matrix += in << (4 * (4 * i + j));
    }
  }
  BFS(matrix);
}