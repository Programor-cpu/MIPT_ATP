#include <iostream>

int tracks = 0;

void Pogruzka(int k, int amount) {
  if (amount <= k) {
    tracks += 1;
  } else {
    if (amount % 2 == 0) {
      Pogruzka(k, amount / 2);
      Pogruzka(k, amount / 2);
    } else {
      Pogruzka(k, amount / 2 + 1);
      Pogruzka(k, amount / 2);
    }
  }
}

int main() {
  int n;
  int k;
  std::cin >> n;
  std::cin >> k;
  Pogruzka(k, n);
  std::cout << tracks;
}