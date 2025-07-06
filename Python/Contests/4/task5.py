import json


class NoImperialOfficersError(Exception):
    pass

class ImperialRosterIterator:
    def __init__(self, officers_file: str, ranks_file: str, min_rank: int=0, min_contract_value: int=0):
        self.min_rank = min_rank
        self.min_contract_value = min_contract_value
        self.officers = self.load_officers(officers_file)
        self.ranks = self.load_ranks(ranks_file)
        self.valid_entries = self.validate_and_filter_entries()

    def load_officers(self, filename: str):
        try:
            with open(filename, 'r', encoding='utf-8') as file:
                officers_data = json.load(file)
                if not officers_data:
                    raise ValueError("Файл списка офицеров пуст.")
                return officers_data
        except json.JSONDecodeError:
            raise ValueError("Некорректные данные в файле офицеров.")
    
    def load_ranks(self, filename: str):
        try:
            with open(filename, 'r', encoding='utf-8') as file:
                ranks_data = json.load(file)
                return ranks_data
        except json.JSONDecodeError:
            raise ValueError("Некорректные данные в файле званий.")

    def validate_and_filter_entries(self):
        valid_entries = []

        for entry in self.officers:
            if not('name' in entry and 'role' in entry):
                raise ValueError("Отсутствуют нужные поля.")
            
            name = entry['name']
            role = entry['role']

            if role == "officer":
                if not('rank' in entry and entry['rank'] in self.ranks):
                    continue
                rank_value = self.ranks[entry['rank']]
                if rank_value >= self.min_rank:
                    valid_entries.append((name, "Офицер", rank_value))
            elif role == "mercenary": # В Звездных воинах для наёмников более уместен термин "bounty hunter", но ладно.
                if not('contract_value' in entry and entry['contract_value'] > 0):
                    continue
                contract_value = entry['contract_value']
                if contract_value >= self.min_contract_value:
                    valid_entries.append((name, "Наемник", contract_value))

        if not valid_entries:
            raise NoImperialOfficersError("Нет подходящих офицеров или наемников.")

        return valid_entries

    def __iter__(self):
        officers = sorted(
            [entry for entry in self.valid_entries if entry[1] == "Офицер"],
            key=lambda x: x[2], reverse=True
        )
        
        mercenaries = sorted(
            [entry for entry in self.valid_entries if entry[1] == "Наемник"],
            key=lambda x: x[2], reverse=True
        )

        sorted_entries = officers + mercenaries
        
        for name, role, value in sorted_entries:
            if role == "Офицер":
                yield f"{name} (Офицер, Ранг: {value})"
            else:
                yield f"{name} (Наемник, Контракт: {value} кредитов)"
