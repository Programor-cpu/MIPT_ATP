import json
from typing import Generator


class JediTrialFailure(Exception):
    pass

def jedi_trials(trials_file: str, female_names_file: str) -> Generator[str, None, None]:
    try:
        with open(trials_file, 'r', encoding='utf-8') as f:
            trials = json.load(f)
            if not trials:
                raise ValueError("Файл пуст.")
        
        with open(female_names_file, 'r', encoding='utf-8') as f:
            female_names = json.load(f)
    
    except FileNotFoundError as e:
        raise ValueError(f"Файл не найден.")
    except json.JSONDecodeError:
        raise ValueError(f"Ошибка декодирования JSON в файле.")

    for trial in trials:
        if not all(key in trial for key in ['candidate', 'trial', 'difficulty', 'success']):
            raise ValueError("Поля неверны")
        
        candidate = trial['candidate']
        trial_name = trial['trial']
        difficulty = trial['difficulty']
        success = trial['success']

        if not(isinstance(candidate, str) and isinstance(trial_name, str) and isinstance(difficulty, int) and isinstance(success, bool)):
            raise ValueError("Неверные типы данных.")
        
        if difficulty < 1 or difficulty > 10:
            raise ValueError("Сложность должна быть в диапозоне 1-10.")
        suffix = "ел" if candidate not in female_names else "ла"
        if success:
            yield f"Успех: {candidate} прош{suffix} испытание {trial_name} (сложность {difficulty})"
        else:
            yield f"Провал: {candidate} не прош{suffix} испытание {trial_name} (сложность {difficulty})"
            raise JediTrialFailure(f"Обучение завершено: {candidate} исключен{suffix} после 1 провала.")

