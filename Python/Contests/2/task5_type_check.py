from functools import wraps
import inspect

def type_check(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        signature = inspect.signature(func)
        real_params = signature.bind(*args, **kwargs)
        for arg_name, arg_value in real_params.arguments.items():
            expected_arg_type = signature.parameters[arg_name].annotation
            if not(isinstance(arg_value, expected_arg_type) or expected_arg_type is inspect.Parameter.empty):
                raise TypeError(f"Аргумент '{arg_name}' должен быть типа {expected_arg_type.__name__}, а получен тип {type(arg_value).__name__}")
        result = func(*args, **kwargs)
        expected_return_type = signature.return_annotation
        if not(isinstance(result, expected_return_type) or expected_return_type is inspect.Signature.empty):
            raise TypeError(f"Возвращаемое значение должно быть типа {expected_return_type.__name__}, а получен тип {type(result).__name__}")
        return result
    return wrapper

@type_check
def concatination(a: str, b: str) -> str:
    return a + b
