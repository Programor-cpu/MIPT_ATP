#include <iostream>
#include <cstring>

void GetRequest(char* &request) {
	char ingoing = static_cast<char>(getchar());
	int i = 0;
	while (!isspace(ingoing)) {
		request[i] = ingoing;
		ingoing = static_cast<char>(getchar());
		++i;
	}
	request[i] = '\0';
}


int main() {
	char** stack = static_cast<char**>(malloc(1 * sizeof(char*)));
	char ingoing;
	char* request = static_cast<char*>(malloc(6 * sizeof(char)));
	size_t size_stack = 0;
	size_t size_string = 0;
	size_t current_capacity = 1;
	size_t string_capacity = 1;
	while (true) {
		GetRequest(request);
		if (!strcmp(request, "push")) {
			bool flag = true;
			++size_stack;
			if (current_capacity == size_stack) {
				stack = static_cast<char**>(realloc(stack, 2 * current_capacity * sizeof(char*)));
				current_capacity *= 2;
			}
			string_capacity = 1;
			stack[size_stack - 1] = static_cast<char*>(malloc(string_capacity * sizeof(char)));
			size_t iter = 0;
			size_string = 0;
			while (flag || !isspace(ingoing)) {
				ingoing = static_cast<char>(getchar());
				++size_string;
				if (string_capacity == size_string) {
					stack[size_stack - 1] = static_cast<char*>(realloc(stack[size_stack - 1], 2 * string_capacity * sizeof(char)));
					string_capacity *= 2;
				}
				stack[size_stack - 1][iter] = ingoing;
				flag = false;
				++iter;
			}
			stack[size_stack - 1][iter] = '\0';
			std::cout << "ok" << '\n';
			continue;
		}
			
		if (!strcmp(request, "pop")) {
			if (size_stack != 0) {
				for (size_t i = 0; stack[size_stack - 1][i] != '\0'; ++i) {
					std::cout << stack[size_stack - 1][i];
				}
				free(stack[size_stack - 1]);
				--size_stack;
			}
			else {
				std::cout << "error" << '\n';
			}
			continue;
		}
		if (!strcmp(request, "back")) {
			if (size_stack != 0) {
				for (size_t i = 0; stack[size_stack - 1][i] != '\0'; ++i) {
					std::cout << stack[size_stack - 1][i];
				}
			}
			else {
				std::cout << "error" << '\n';
			}
			continue;
		}
		if (!strcmp(request, "size")) {
			std::cout << size_stack << '\n';
			continue;
		}
		if (!strcmp(request, "clear")) {
			for (size_t i = 0; i != size_stack; ++i) {
				free(stack[i]);
			}
			free(stack);
			size_stack = 0;
			size_string = 0;
			current_capacity = 1;
			string_capacity = 1;
			stack = static_cast<char**>(malloc(current_capacity * sizeof(char*)));
			std::cout << "ok" << '\n';
			continue;
		}
		if (!strcmp(request, "exit")) {
			for (size_t i = 0; i != size_stack; ++i) {
				free(stack[i]);
			}
			free(stack);
			free(request);
			std::cout << "bye" << '\n';
			break;
		}
	}
}