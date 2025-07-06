#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

bool TheBalance(std::string& arr) {
  std::stack<char> stak;

  for (int i = 0; i < (int)arr.length(); i++) {
    if (arr[i] == '(' || arr[i] == '[' || arr[i] == '{') {
      stak.push(arr[i]);
      continue;
    }

    if (stak.empty()) {
      return false;
    }

    if (arr[i] == ')') {
      char the_last_one = stak.top();
      stak.pop();
      if (the_last_one == '{' || the_last_one == '[') {
        return false;
      }
    }
    if (arr[i] == ']') {
      char the_last_one = stak.top();
      stak.pop();
      if (the_last_one == '{' || the_last_one == '(') {
        return false;
      }
    }
    if (arr[i] == '}') {
      char the_last_one = stak.top();
      stak.pop();
      if (the_last_one == '(' || the_last_one == '[') {
        return false;
      }
    }
  }

  return (stak.empty());
}

int main() {
  std::string arr;
  std::cin >> arr;

  if (TheBalance(arr)) {
    std::cout << "YES";
  } else {
    std::cout << "NO";
  }
}