command_registry = {}

def register_command(func):
    command_registry[func.__name__] = func
    return func

def run_command(name):
    if name in command_registry:
        return command_registry[name]()
    print(f"Команда '{name}' не найдена.")
