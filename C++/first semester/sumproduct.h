#include <iostream>

void Calculation(int**& numbers, int*& sizes, long long& answer, long long current, int index, int* used_once, const int& size) {
	long long current_copy = current;
	if (index == size) {
		answer += current;
	}
	else {
		for (int i = 0; i != sizes[index]; i++) {
			if (used_once[i] == 0) {
				current *= numbers[index][i];
				index += 1;
				used_once[i] = 1;
				Calculation(numbers, sizes, answer, current, index, used_once, size);
				index -= 1;
				used_once[i] = 0;
				if (numbers[index][i] != 0) {
					current /= numbers[index][i];
				}
				else {
					current = current_copy;
				}
			}
		}


	}


}

void Purify(int**& numbers, int*& sizes, int*& used_indexes, const int& size) {
	for (int i = 0; i != size; i++) {
		free(numbers[i]);
	}
	free(numbers);
	free(sizes);
	free(used_indexes);
}


int InformationEmerge(int& size, char**& arr, int**& numbers, int*& sizes) {
	int ingoing;
	int maximum = 0;
	for (int i = 1; i != size + 1; i++) {
		numbers[i - 1] = (int*)malloc((int)(atoi(arr[i])) * sizeof(int));
		sizes[i - 1] = (int)(atoi(arr[i]));
		if ((int)(atoi(arr[i])) > maximum) {
			maximum = (int)(atoi(arr[i]));
		}
	}
	for (int i = 0; i != size; i++) {
		for (int j = 0; j != sizes[i]; j++) {
			std::cin >> ingoing;
			numbers[i][j] = ingoing;
		}
	}
	return maximum;
}

int main(int size, char** arr) {
	--size;
	long long answer = 0;
	int* sizes = (int*)malloc(size * sizeof(int));
	int** numbers = (int**)malloc(size * sizeof(int*));
	int size_of_storage = InformationEmerge(size, arr, numbers, sizes);
	int* used_indexes = (int*)malloc(sizeof(int) * size_of_storage);
	for (int i = 0; i != size_of_storage; i++) {
		used_indexes[i] = 0;
	}
	Calculation(numbers, sizes, answer, 1, 0, used_indexes, size);
	std::cout << answer;
	Purify(numbers, sizes, used_indexes, size);
	
}