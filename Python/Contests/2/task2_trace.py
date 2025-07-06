from functools import wraps

def trace(func):
  @wraps(func)
  def wrapper(*args, **kwargs):
    result = func(*args, **kwargs)
    print(f"Функция {func.__name__} вернула значение {res}")
    return result
  return wrapper
