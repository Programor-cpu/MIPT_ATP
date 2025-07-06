#Летний жаворонок
#Взметнулся над горами, в небеса,
#В пустынную бездну...

import sqlite3
DB_name = 'Godel.db'
SQL_cashed_functions = ["Prime","nPrime"]
SQL_data_connections = {"Primes" : ["Prime", "nPrime"]}

def ArgsToKey(args : list[int]) -> str:
    """Короче так хранить будем"""
    return ','.join(map(str, args))

def Creating_Databases() -> None:
    """Будем хранить немного таблиц, зато на каждую таблицу будет своя функция"""
    connection = sqlite3.connect(DB_name)
    cursor = connection.cursor()
    for func_name in SQL_cashed_functions:
        cursor.execute(f'''
            CREATE TABLE IF NOT EXISTS "{func_name}" (
                args TEXT PRIMARY KEY,
                result INTEGER NOT NULL
            )
        ''')
    connection.commit()
    connection.close()

def PreCount(args : list[int], func : str) -> int:
    """Нужно вызвать соответствующее поведение"""
    if func == 'Prime' or func == 'nPrime':
        pass
        #return Parser().ProcessLine('{}({})'.format(func, args[0]))

def FilingBasesWithData(n : int) -> None:
    """Заполняем таблицу данными"""
    connection = sqlite3.connect(DB_name)
    cursor = connection.cursor()
    for f in SQL_cashed_functions:
        for i in range(0, n):
            SQ_args = ArgsToKey([i])
            res = PreCount([i], f)
            cursor.execute(f'INSERT OR REPLACE INTO {f} (args, result) VALUES (?, ?)', (SQ_args, res))
    connection.commit()
    connection.close()

#res = Parser().ProcessLine('Import Primes')
#Creating_Databases()
#FilingBasesWithData(200)
