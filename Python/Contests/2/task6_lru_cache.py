from functools import wraps

def lru_cache(maxsize):
    def decorator(func):
        cache = {}
        func_order = []
        @wraps(func)
        def wrapper(*args):
            if args not in cache:
                result = func(*args)
                cache[args] = result
                func_order.append(args)
            else:
                func_order.remove(args)
                func_order.append(args)
                return cache[args]
            if  maxsize<len(cache):
                del cache[func_order.pop(0)]
            return result
        return wrapper
    return decorator
