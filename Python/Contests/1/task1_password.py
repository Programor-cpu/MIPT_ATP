def restore_permutation(n, m, edges):
    from collections import defaultdict

    # Создаем граф
    graph = defaultdict(list)
    for u, v, s in edges:
        graph[u].append((v, s))
        graph[v].append((u, s))

    # Массив для хранения значений x
    x = [0] * (n + 1)
    visited = [False] * (n + 1)

    def dfs(node):
        visited[node] = True
        for neighbor, sum_value in graph[node]:
            if not visited[neighbor]:
                # Вычисляем значение для соседней вершины
                x[neighbor] = sum_value - x[node]
                dfs(neighbor)

    # Начинаем с вершины 1 и задаем ей произвольное значение
    x[1] = 1  # Начнем с 1 для первой вершины
    dfs(1)

    # Проверяем корректность значений
    value_set = set(x[1:n+1])
    if len(value_set) == n and all(v in value_set for v in range(1, n + 1)):
        return x[1:n+1]
    else:
        return []

# Чтение входных данных
n, m = map(int, input().split())
edges = [tuple(map(int, input().split())) for _ in range(m)]

# Восстановление перестановки
result = restore_permutation(n, m, edges)

# Вывод результата
print(" ".join(map(str, result)))
