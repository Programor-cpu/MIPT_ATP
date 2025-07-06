import time
from functools import wraps

def retry(attempts, delay):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            final_exception = None
            for try_index in range(0, attempts):
                try:
                    return func(*args, **kwargs)
                except Exception as exception:
                    final_exception = exception
                    print(f"Попытка {try_index+1} завершилась неудачей: {exception}. Повтор через {delay} сек...")
                    time.sleep(delay)
            raise final_exception
        return wrapper
    return decorator
