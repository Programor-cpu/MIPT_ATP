import json
import os


class ProjectDataError(Exception):
    pass

class DeathStarProject:
    def __init__(self, filename: str):
        self.filename = filename
        self.data = {}
        self.load_project()

    def load_project(self) -> dict:
        if not os.path.exists(self.filename):
            raise ProjectDataError(f"Файл {self.filename} не найден.")
        
        try:
            with open(self.filename, 'r', encoding='utf-8') as file:
                self.data = json.load(file)
        except json.JSONDecodeError:
            raise ProjectDataError(f"Некорректный JSON-формат в файле {self.filename}.")
        
        return self.data

    def update_field(self, field: str, value) -> None:
        if field in self.data:
            self.data[field] = value
            return
        raise ProjectDataError(f"Поле '{field}' не существует в данных проекта.")
        
        
    def validate_project(self) -> None:
        required_keys = ["name", "budget", "deadline"]
        for key in required_keys:
            if key not in self.data:
                raise ProjectDataError(f"Отсутствует ключ: '{key}'")
        
        if self.data["budget"] <= 0:
            raise ProjectDataError("Бюджет положителен")

    def save_project(self) -> None:
        with open(self.filename, 'w') as file:
            json.dump(self.data, file, indent=4)

