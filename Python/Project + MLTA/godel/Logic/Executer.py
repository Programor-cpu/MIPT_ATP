#В лугах привольных
#Заливается песней жаворонок
#Без трудов и забот...
from __future__ import annotations
import re
from collections import deque
from .DataInserter import DB_name, SQL_cashed_functions, ArgsToKey, SQL_data_connections
import sqlite3
import pygame

DB_way = "./Logic/{}"
Code_way = "./Code/{}"
Stuff_way = "./Stuff/{}"

class Primitive:
    node = None
    s = None
    """Класс реализовывает поведение примитивных функций при построении"""
    @staticmethod
    def NumberCase__() -> bool:
        """ Дали число - вернем число. """
        if Primitive.s == 'Z':
            Primitive.s = '0'
        if Primitive.s.isdigit():
            Primitive.node.is_digit = True
            Primitive.node.primitive_term = int(Primitive.s)
            return True
        return False

    @staticmethod
    def ProjectorCase__() -> bool:
        """Pi - прибавить j к i - ому аргументу"""
        if Primitive.s == 'P':
            Primitive.s = 'P1'
        if Primitive.s.startswith('P') and Primitive.s[1:].isdigit():
            Primitive.node.primitive_arg = int(Primitive.s[1:]) - 1
            Primitive.node.primitive_term = 0
            return True
        return False

    @staticmethod
    def AdderCase__() -> bool:
        """Si^j - прибавить j к i - ому аргументу"""
        if Primitive.s.startswith('S') and '^' not in Primitive.s:
            Primitive.s = Primitive.s + '^1'
        if Primitive.s.startswith('S^'):
            Primitive.s = Primitive.s.replace('S^', 'S1^')
        m = re.fullmatch(r'S(\d+)\^(\d+)', Primitive.s)
        if m:
            Primitive.node.primitive_arg = int(m[1]) - 1
            Primitive.node.primitive_term = int(m[2])
            return True
        return False

    @staticmethod
    def GivingPrimitiveAttributes__(node : Node | FakeNode) -> bool:
        """Придает Node примитивные атрибуты, корректно отрабатывает сообщение об ошибке"""
        Primitive.node = node
        Primitive.s = node.name
        if not (Primitive.NumberCase__() or Primitive.ProjectorCase__() or Primitive.AdderCase__()):
            if node.name in Executer.links:
                node.link_to_function = Executer.links[node.name]
                return False
            raise UndefinedFunctionException(
                            "Didn`t manage definition - no function '{}' known".format(node.name))
        return True

    @staticmethod
    def ExecutePrimitiveOnTheSpot(s : str, args : list[int]) -> int:
        class FakeNode():
            def __init__(self, s : str):
                self.name = s
                self.primitive_arg = 0
                self.primitive_term = 0
                self.is_digit = 0
        if Primitive.GivingPrimitiveAttributes__(FakeNode(s)):
            if Primitive.node.is_digit:
                return Primitive.node.primitive_term
            try:
                return args[Primitive.node.primitive_arg] + Primitive.node.primitive_term
            except:
                raise UndefinedFunctionException(
                    "Not enough arguments in {}: expected {}, got {}".format(s, int(Primitive.node.primitive_arg + 1), len(args)))
        return -1


class Function:
    """Функция. Лежит в links, хранит
    одно или два дерева - zero_option, option
    bool recursive - рекурсивна ли
    dict pre_counted_values
    text - информация для поддержания текста"""

    def __init__(self, string : str, name : str):
        self.text = "{} = {}".format(name, string)
        self.is_expected_previous_step = False
        self.pre_counted_values = dict()
        self.name = name

        self.len_of_got_args = 0
        self.last_got_arg = 0

        string.replace(" ", '')
        if ";" in string:
            self.recursive = True
        else:
            self.recursive = False

        if self.recursive:
            funcs = string.split(";")
            self.zero_option = Node(funcs[0])
            self.option = Node(funcs[1])
        else:
            self.option = Node(string)

    def RecursionStep__(self, order : Order) -> int:
        """Шаг вычислений, при необходимости добавляет новые элементы в стек - свое дерево и шаг рекурсии"""
        args = order.args
        res = Casher.Search(self, args)
        if res != -1:
            return res
        order.obj = Casher(self, args)
        global stack_of_calls
        if self.recursive:
            if len(args) == 0:
                raise UndefinedFunctionException("Failed figuring recursion option")
            if args[-1] == 0:
                args.pop()
                Executer.add(self.zero_option, args)
                return -1
            args[-1] -= 1
            Executer.add(self.option, args)
            Executer.add(self, args)
            return -1
        Executer.add(self.option, args)
        return -1

class Node:
    """Вершина дерева, хранит
    str name, - строка
    bool is_leaf, - лист ли
    sons [node...] - список сыновей
    bool is_primitive - в зависимости от него харнит либо:
    link_to_function - ссылка на функцию, не примитивна
    bool is_digit int primitive__arg, int primitive__augend - атрибуты примитивной функции
    important_args - список нужных аргументов"""

    def __init__(self, name : str):
        self.is_digit = False
        self.primitive_arg = 0
        self.primitive_term = 0
        self.is_primitive = False
        if "(" not in name:
            self.name = name
            self.is_leaf = True
            self.is_primitive = Primitive.GivingPrimitiveAttributes__(self)
            return
        self.is_leaf = False
        self.name = name.split('(')[0]
        self.is_primitive = Primitive.GivingPrimitiveAttributes__(self)
        opened = 0
        self.sons = []
        parse = name[name.find("(") + 1:-1].replace(' ', '')
        string_to_go = ""
        for i in parse:
            if i == "(":
                opened += 1
                string_to_go += i
            elif i == ")":
                opened -= 1
                string_to_go += i
            elif i == ',' and opened == 0:
                self.sons.append(Node(string_to_go))
                string_to_go = ""
            else:
                string_to_go += i
        self.sons.append(Node(string_to_go))

    def RecursionStep__(self, order : Order) -> int:
        """Шаг вычислений, при необходимости добавляет новые элементы в стек - свое дерево и шаг рекурсии"""
        args = order.args
        if self.is_leaf:
            if self.is_primitive:
                if self.is_digit:
                    return self.primitive_term
                if len(args) < self.primitive_arg + 1:
                    raise UndefinedFunctionException("Wrong number of arguments in {}: expected {}, got {}".format(self.name, self.primitive_arg + 1, len(args)))
                return self.primitive_term + args[self.primitive_arg]
            order.obj = self.link_to_function
            return -1
        if self.is_primitive:
            if self.is_digit:
                return self.primitive_term
            if self.primitive_term == 0:
                order.obj = self.sons[self.primitive_term]
                return -1
            order.obj = self.sons[self.primitive_arg]
            order.stored_aug += self.primitive_term
            #order.obj = UpStepWithAdd(self.primitive_term)
            #stack_of_calls.append(Order(self.sons[self.primitive_arg], args))
            return -1
        if order.is_new_tree:
            order.args = []
            order.is_new_tree = False
            order.chained_args = args
        if len(order.args) == len(self.sons):
            order.obj = self.link_to_function
            return -1
        Executer.add(self.sons[len(order.args)], order.chained_args)
        return -1


class UpStepWithAdd:
    """Класс в разработке, может пригодиться для корректной отработки некоторых флагов и ошибок"""

    def __init__(self, n : int):
        self.numb = n

    def RecursionStep__(self, order : Order) -> int:
        """Шаг вычислений - передает наверх с нужной суммой"""
        return self.numb + order.args[-1]

class Casher:
    """Имеет доступ к базе данных. Пополняет локальные словари функций и поддерживает откат аргументов индукции"""

    data = set()

    def __init__(self, func : Function, args : list[int]):
        self.func = func
        self.args = args
        self.len_of_args = len(args)
        if len(args) > 0:
            self.last_arg = args[-1]

    def RecursionStep__(self, order : Order) -> int:
        """Шаг вычислений. Откатывает аргументы и пополняет локальный словарь"""
        res = order.args[-1]
        del self.args[self.len_of_args:]
        while len(self.args) < self.len_of_args:
            self.args.append(0)
        if self.len_of_args > 0:
            self.args[-1] = self.last_arg
        self.func.pre_counted_values[ArgsToKey(self.args)] = res
        return res

    @staticmethod
    def Search(func, args: list[int]) -> int:
        """Public method of checking for values"""
        if func.name in SQL_cashed_functions and func.name in Casher.data:
            connection = sqlite3.connect(DB_way.format(DB_name))
            cursor = connection.cursor()
            res = cursor.execute(f'SELECT result FROM {func.name} WHERE {ArgsToKey(args)} = args').fetchall()
            if res:
                return res[0][0]
            connection.close()
        return func.pre_counted_values.get(ArgsToKey(args), -1)


class UndefinedFunctionException(Exception):
    """Класс исключений. Хранит сообщение об ошибке"""

    def __init__(self, s : str):
        self.str_error = s

    def AddToBegin(self, s : str) -> UndefinedFunctionException:
        self.str_error = s + ' ' + self.str_error
        return self

    def AddToEnd(self, s : str) -> UndefinedFunctionException:
        self.str_error += ' ' + s
        return self


class Executer:
    """Класс занимается выполнением функции, менеджментом ошибок, базами данных"""

    stack_of_calls = deque()
    links = dict()

    class Order:
        """Класс отображающий запрос на вычисления,
        нужен для избежания рекурсий и копирования"""

        def __init__(self, func, args: list):
            self.obj = func
            self.args = args
            self.is_new_tree = True
            self.stored_aug = 0

    @staticmethod
    def AllCommands__():
        return Executer.links.keys()

    @staticmethod
    def add(f, a : list[int]):
        Executer.stack_of_calls.append(Executer.Order(f, a))

    @staticmethod
    def Execute(name : str, args : list[int]) -> int:
        """Исполняет метод. Запускает рекурсивный поддерживаемый поток"""
        res = Primitive.ExecutePrimitiveOnTheSpot(name, args)
        if res != -1:
            return res
        try:
            func = Executer.links[name]
        except:
            raise UndefinedFunctionException("Did not manage definition - no {} function".format(name))
        Executer.add(None, [])
        Executer.add(func, args)
        st = Executer.stack_of_calls
        while len(st) > 1:
            stage = st[-1]
            try:
                res = stage.obj.RecursionStep__(stage)
            except UndefinedFunctionException as e:
                while len(st) > 0:
                    st.pop()
                raise e
            if res != -1:
                res += stage.stored_aug
                st.pop()
                st[-1].args.append(res)
        return st.pop().args[0]

class SpecialBehavior:
    """Easter eggs"""
    easter_eggs = ["text", "python", "Condratuk", "linux", "miraculas"]

    @staticmethod
    def IsEaster(s : str) -> bool:
        return s in SpecialBehavior.easter_eggs

    @staticmethod
    def ActivateSpecialBehavior(s : str) -> str:
        if s == "linux":
            return "Guido van Rossum"
        if s == "python":
            return "Linus Torwalds"
        if s == "Condratuk":
            return "I can do everything"
        if s == "text":
            ans = ""
            for i in Executer.links.values():
                ans += '\n' + i.text
            return ans[2:]
        if s == "miraculas":
            pygame.mixer.init()
            pygame.mixer.music.load(Stuff_way.format('lb.mp3'))
            pygame.mixer.music.play()
        return ""

class FileManager:
    """Управляет файлами"""

    imported_modules = set()

    @staticmethod
    def ModuleActivation__(filename : str) -> str:
        if filename in FileManager.imported_modules:
            return ""
        FileManager.imported_modules.add(filename)
        try:
            flag = True
            ans = ""
            with open(Code_way.format(filename), 'r', encoding='utf-8') as file:
                for i in file.readlines():
                    f = len(FileManager.imported_modules)
                    res = Parser.ProcessLine(i.strip())
                    ans += "\n" + i.rstrip()
                    if res:
                        if res.isdigit():
                            ans += " = " + res.rstrip()
                        else:
                            ans += "\n" + res.rstrip()
                            if f == len(FileManager.imported_modules):
                                flag = False
            if flag:
                for i in SQL_data_connections.get(filename, []):
                    Casher.data.add(i)
            return ans[1:]
        except FileNotFoundError:
            return "File {} not found".format(filename)

class Parser:
    """Превращает символы в команды - вызывает конструкторы в нужных местах."""

    exit_commands = ["quit", "exit"]

    def GetName__(self, line : str) -> str:
        return line.split('(')[0]

    @staticmethod
    def AllCommands():
        return Executer.AllCommands__()

    @staticmethod
    def ParseArguments__(s : str) -> list[int]:
        inner_args = re.sub(r'^.*?\((.*)\)$', r'\1', s)
        args = []
        current = ""
        depth = 0
        for char in inner_args:
            if char == '(':
                depth += 1
            elif char == ')':
                depth -= 1
            if char == ',' and depth == 0:
                args.append(current.strip())
                current = ""
            else:
                current += char
        if current:
            args.append(current.strip())
        parsed_args = []
        try:
            for arg in args:
                parsed_args.append(int(arg))
        except ValueError:
            raise UndefinedFunctionException("Failed to parse argument '{}'".format(args))
        return parsed_args

    @staticmethod
    def Definition__(line : str) -> None:
        """Конструирует объект"""
        parts = line.split("=")
        name = parts[0]
        if name in Executer.links:
            raise UndefinedFunctionException("Redefinition of function is forbidden.")
        if Parser.IsPrimitive__(name):
            raise UndefinedFunctionException("Redefinition of primitive function is strictly forbidden!")
        if SpecialBehavior.IsEaster(name):
            return ""
        if Parser.HasInvalidChars__(name):
            raise UndefinedFunctionException("Error: invalid in {}".format(name))
        try:
            Executer.links[name] = Function(parts[1], name)
        except UndefinedFunctionException as e:
            raise e.AddToBegin("Error: Line ignored.")

    @staticmethod
    def Execution__(line : str) -> int:
        name = line.split('(')[0]
        arguments = []
        if name == line:
            line += "()"
        if name:
            arguments = Parser.ParseArguments__(line)
        if SpecialBehavior.IsEaster(name):
            return SpecialBehavior.ActivateSpecialBehavior(name)
        try:
            return Executer.Execute(name, arguments)
        except UndefinedFunctionException as e:
            raise e.AddToBegin("Error occupied:")

    @staticmethod
    def ProcessLine(line : str) -> str:
        """Интерфейсная функция - с нее все начинается"""
        if line in Parser.exit_commands:
            exit(0)
        if line.startswith("import "):
            return FileManager.ModuleActivation__(line[7:])
        line = Parser.SimplifyingLine__(line)
        if line == "":
            return ""
        try:
            if "=" in line:
                Parser.Definition__(line)
                return ""
            return str(Parser.Execution__(line))
        except UndefinedFunctionException as e:
            return e.str_error

    @staticmethod
    def SimplifyingLine__(s : str) -> str:
        """Применение двух правил
        1)ставьте пробелы сколько хотите
        2)# означают комментарии"""
        return s.strip().replace(" ", "").split('#')[0]

    @staticmethod
    def IsPrimitive__(s : str) -> bool:
        """Проверяет, является ли имя примитивной функцией"""
        return bool(re.fullmatch(
            r'^(Z|S(\^0*[1-9]\d*|0*[1-9]\d*(\^0*[1-9]\d*)?)?|P(0*[1-9]\d*)?|\d+)$',
            s
        ))

    @staticmethod
    def HasInvalidChars__(text : str) -> bool:
        """Имя может содержать только значения из [A-Za-z0-9_]"""
        return bool(re.search(r'[^A-Za-z0-9_]', text))
