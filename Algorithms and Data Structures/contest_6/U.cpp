#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

long long StrToLong(const std::string& in) {
  long long answer = 0;
  for (long long i = 0; i != (long long)in.size(); ++i) {
    if (in[i] == '1') {
      answer += 1 << i;
    }
  }
  return answer;
}

std::vector<bool> Findcliques(const std::vector<long long>& one,
                              long long size) {
  long long rem = 0;
  std::vector<bool> cliques(1 << size, false);
  cliques[0] = true;
  for (long long i = 1; i != (1 << size); ++i) {
    for (long long j = 0; j != size; ++j) {
      if ((bool)(i & (1 << j))) {
        rem = j;
        break;
      }
    }
    long long cliq = i - (1 << rem);
    cliques[i] = (cliques[cliq] && ((one[rem] & i) == cliq));
  }
  return cliques;
}

std::vector<long long> Subcliques(const std::vector<long long>& one,
                                  long long size) {
  std::vector<long long> subcliques(1 << size, 0);
  subcliques[0] = 1;
  long long rem = 0;
  for (long long i = 1; i != (1 << size); ++i) {
    for (long long j = 0; j != size; ++j) {
      if ((bool)(i & (1 << j))) {
        rem = j;
        break;
      }
    }
    long long cliq = i - (1 << rem);
    subcliques[i] = subcliques[cliq] + subcliques[cliq & one[rem]];
  }
  return subcliques;
}

int main() {
  long long n;
  long long answer = 0;
  std::cin >> n;
  long long left = n / 2;
  long long right = n / 2 + n % 2;
  std::vector<long long> left_left(left);
  std::vector<long long> right_right(right);
  std::vector<long long> left_right(left);
  std::string in;
  for (long long i = 0; i != left; ++i) {
    std::cin >> in;
    left_left[i] = StrToLong(in.substr(0, left));
    left_right[i] = StrToLong(in.substr(left, right));
  }
  for (long long i = 0; i != right; ++i) {
    std::cin >> in;
    right_right[i] = StrToLong(in.substr(left, right));
    // std::cout << in.substr(left, right) << '\n';
  }
  std::vector<bool> cliques_left = Findcliques(left_left, left);

  std::vector<long long> subcliques = Subcliques(right_right, right);

  std::vector<int> con(1 << left, 0);
  con[0] = (1 << right) - 1;
  long long rem;
  for (long long i = 1; i != (1 << left); ++i) {
    if (!cliques_left[i]) {
      continue;
    }
    for (long long j = 0; j != left; ++j) {
      if ((bool)(i & (1 << j))) {
        rem = j;
        break;
      }
    }
    long long cliq = i - (1 << rem);
    con[i] = (con[cliq] & left_right[rem]);
    answer += subcliques[con[i]];
  }
  std::cout << answer + subcliques[subcliques.size() - 1];
}
