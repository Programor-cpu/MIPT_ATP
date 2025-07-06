import csv
from typing import List, Dict
import os


class CloneValidationError(Exception):
    pass

class CloneWarsCSV:
    def __init__(self, filename: str):
        self.filename = filename
        self.data = self.load_data()

    def load_data(self) -> List[Dict[str, str]]:
        with open(self.filename, mode='r', encoding='utf-8') as file:
            reader = csv.DictReader(file)
            return [row for row in reader]

    def validate_data(self) -> None:
        for entry in self.data:
            serial = entry.get("СерийныйНомер")
            cost = entry.get("Затраты")
            coef = entry.get("Коэффициент")
            if not serial:
                raise CloneValidationError("СерийныйНомер не должен быть пустым.")
            try:
                if float(cost) <= 0:
                    raise CloneValidationError("Нам дают деньги?")
            except ValueError:
                raise CloneValidationError("Затраты должны быть числом.")

            try:
                float(coef) 
            except ValueError:
                raise CloneValidationError("Коэффициент должен быть числом.")

    def add_clone_batch(self, serial: str, cost: int, coef: float) -> None:
        """Добавляет новую запись о партии клонов."""
        if not serial:
            raise CloneValidationError("Серийный номер не должен быть пустым")
        try:
            if float(cost) <= 0:
                raise CloneValidationError("Нам дают деньги?.")
        except ValueError:
                raise CloneValidationError("Затраты должны быть числом.")
        new_entry = {
            "СерийныйНомер": serial,
            "Затраты": str(cost),
            "Коэффициент": str(coef)
        }
        self.data.append(new_entry)
        with open(self.filename, mode='a', newline='', encoding='utf-8') as file:
            writer = csv.DictWriter(file, fieldnames=["СерийныйНомер", "Затраты", "Коэффициент"])
            writer.writerow(new_entry)
