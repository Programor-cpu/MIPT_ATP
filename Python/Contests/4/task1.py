import os
from typing import List, Dict

class ChronicleFormatError(Exception):
    pass

class HolocronChronicles:
    def __init__(self, filename: str):
        self.filename = filename
        self.chronicles = self.load_chronicles()

    def load_chronicles(self) -> List[Dict[str, str]]:
        chronicles = []
        if not os.path.exists(self.filename):
            raise ChronicleFormatError("Way in not find")

        with open(self.filename, 'r', encoding='utf-8') as file:
            for line in file:
                line = line.strip()
                parts = [part.strip() for part in line.split('|')]

                if len(parts) == 3:
                    force_align_part = parts[0].split('] ')[0]
                    hero = parts[0].split('] ')[1]
                    event = parts[1].strip()
                    if (force_align_part == '[JEDI'):
                        align = 'jedi'
                    elif (force_align_part == '[SITH'):
                        align = 'sith'
                    else:
                        continue
                    data = parts[2]
                    year_string = data[:-4]
                    if not year_string.isdigit():
                        continue
                    year = int(year_string)
                    chronicles.append({
                        "hero": hero,
                        "event": event,
                        "year": year,
                        "align": align
                    })
                
        return chronicles

    def add_record(self, hero: str, event: str, year: int, align: str) -> None:
        if align.lower() not in ['jedi', 'sith']:
            raise ChronicleFormatError("Only jedi or sith are allowed")
        
        if int(year) < 0:
            raise ValueError("Incorrect year")

        align_str = "[JEDI]" if align == 'jedi' else "[SITH]"
        record = f"{align_str} {hero} | {event} | {year} ПБЯ\n"
        
        with open(self.filename, 'a', encoding='utf-8') as file:
            file.write(record)
