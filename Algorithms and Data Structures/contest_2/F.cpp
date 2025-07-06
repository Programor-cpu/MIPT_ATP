#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

long long int Calculation(std::string& arr) {
  std::stack<long long int> que;
  for (int i = 0; i < (int)arr.length(); i++) {
    char a = arr[i];
    if (isdigit(a) != 0 || a == '0') {
      que.push(a - '0');
    } else {
      if (a != ' ') {
        if (a == '+') {
          long long int one = que.top();
          que.pop();
          long long int two = que.top();
          que.pop();
          que.push(one + two);
        }
        if (a == '-') {
          long long int one = que.top();
          que.pop();
          long long int two = que.top();
          que.pop();
          que.push(two - one);
        }
        if (a == '*') {
          long long int one = que.top();
          que.pop();
          long long int two = que.top();
          que.pop();
          que.push(one * two);
        }
      }
    }
  }
  return que.top();
}

int main() {
  std::string arr;
  getline(std::cin, arr);

  std::cout << Calculation(arr);
}