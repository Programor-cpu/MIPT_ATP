import time
from functools import wraps

def timeit(func):
  @wraps(func)
  def wrapper(*args, **kwargs):
    start_time = time.time()
    try:
      res = func(*args, **kwargs)
    except Exception as excepted:
      end_time = time.time()
      duration = end_time - start_time
      if duration >=0:
        print(f"Функция {func.__name__} выполнена за{duration: .6f} секунд.")
      raise excepted
    end_time = time.time()
    duration = end_time - start_time
    if duration >=0:
      print(f"Функция {func.__name__} выполнена за{duration: .6f} секунд.")
    return res
  return wrapper
