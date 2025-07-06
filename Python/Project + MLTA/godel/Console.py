import sys
import os
import re
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QTextEdit,
    QWidget,
    QVBoxLayout,
    QLabel,
    QGraphicsOpacityEffect,
)
from PyQt5.QtCore import (
    Qt,
    QTimer,
    QPoint,
    QObject,
    QEvent,
)
from PyQt5.QtGui import (
    QColor,
    QTextCharFormat,
    QTextCursor,
    QFont,
    QCloseEvent,
    QIcon,
)
from Logic.Executer import Parser, Executer
#from Logic.. import Executer

HISTORY_FILE = "history.txt"
#Roma`s version
#BOARD_COLOR = "#7442c8"
#CURSOR_COLOR = "white"
#NUMBER_COLOR = "#00FF00"
#ERROR_COLOR = "red"
#FUNCTION_COLOR = "#00BFFF"
#pale purple
#BOARD_COLOR = "#7442c8"
#CURSOR_COLOR = "pink"
#FUNCTION_COLOR = "#34c915"
#NUMBER_COLOR   = "#f9ae54"
#ERROR_COLOR    = "#ff3c6d"
#BACKGROUND_COLOR = "#321161"
#TEXT_COLOR = "#fff8f5"
#black
BOARD_COLOR = "#303841"
CURSOR_COLOR = "pink"
FUNCTION_COLOR = "#34c915"
NUMBER_COLOR   = "#ffcc57"
ERROR_COLOR    = "red"
BACKGROUND_COLOR = "black"
TEXT_COLOR = "#fff5fc"

class CommandHistory:
    """История команд для консоли.
    Сохраняет список команд, вводимых пользователем, предоставляет функции
    для добавления команд, просмотра предыдущих и следующих команд, а также
    загрузки и сохранения истории из файла history.txt
    Атрибуты:
        history_file (str): путь к файлу с историей команд
        commands (list[str]): список введенных команд
        index (int): текущая позиция в истории команд"""
    def __init__(self, history_file : str) -> None:
        """
        Аргументы:
            history_file (str): путь к файлу txt для сохранения команд"""
        self.history_file = history_file
        self.commands = self.load_history()
        self.index = len(self.commands)

    def load_history(self) -> list[str]:
        """Загружает историю команд из файла
        Возвращает:
            list[str]: если файл существует, то список.
            Пустой список в противном"""
        if os.path.exists(self.history_file):
            with open(self.history_file, "r", encoding="utf-8") as f:
                return [line.strip() for line in f]
        return []

    def save_history(self) -> None:
        """Сохраняет текущую историю команд в файл"""
        with open(self.history_file, "w", encoding="utf-8") as f:
            for cmd in self.commands:
                f.write(cmd + "\n")

    def add(self, cmd : str) -> None:
        """Добавляет команду в историю и сохраняет изменения в файл.
        Аргументы:
            cmd (str): команда для добавления"""
        self.commands.append(cmd)
        self.index = len(self.commands)
        self.save_history()

    def previous(self) -> str:
        """Возвращает предыдущую команду из истории
        Возвращает:
            str | None: предыдущая команда. None в случае начала истории"""
        if self.index > 0:
            self.index -= 1
            return self.commands[self.index]
        return None

    def next(self) -> str:
        """Возвращает следующую команду из истории
        Возвращает:
            str: следующая команда (пустая строка в случае конца истории)"""
        if self.index < len(self.commands) - 1:
            self.index += 1
            return self.commands[self.index]
        self.index = len(self.commands)
        return ""


class SuggestionManager:
    """Менеджер подсказок для консоли
    Отвечает за отображение окошка автодополнения
    Атрибуты:
        label (QLabel): виджет для отображения подсказки
        opacity (float): текущая прозрачность подсказки
        timer (QTimer): таймер для управления эффектом затухания подсказки"""
    def __init__(self, parent : QWidget) -> None:
        """
        Аргументы:
            parent (QWidget): родительский виджет для подсказки"""
        self.label = QLabel(parent)
        self.label.setStyleSheet("color: #AAAAAA;"
                                 + " background-color: #b427ef;")
        self.label.hide()
        self.opacity = 1.0
        self.timer = QTimer()
        self.timer.timeout.connect(self.fade_out)

    def show_suggestion(self, text : str, position : QPoint) -> None:
        """Отображает текст подсказки на экране
        Аргументы:
            text (str): текст подсказки
            position (QPoint): позиция, где должна быть подсказка"""
        self.label.setText(f"→ {text}")
        self.label.move(position + QPoint(10, 0))
        self.label.show()
        self.opacity = 1.0
        self.timer.start(100)

    def fade_out(self) -> None:
        """Постепенно уменьшает прозрачность подсказки
        до её исчезновения"""
        self.opacity -= 0.05
        if self.opacity <= 0:
            self.timer.stop()
            self.label.hide()
        else:
            effect = QGraphicsOpacityEffect()
            effect.setOpacity(self.opacity)
            self.label.setGraphicsEffect(effect)


class SyntaxHighlighter:
    """Подсветка синтаксиса для консоли
    Отвечает за цвет текста в консоли: числа, функции, ошибки
    Атрибуты:
        area (QTextEdit): текстовая область,
        в которой будет выполнена подсветка"""

    def __init__(self, area : QTextEdit) -> None:
        """
        Аргументы:
            area (QTextEdit): текстовая область для окрашивания"""
        self.area = area

    def highlight(self) -> None:
        """Делает окраску"""
        text = self.area.toPlainText()
        cursor = self.area.textCursor()
        cursor.beginEditBlock()

        def fmt(regex, color):
            fmt = QTextCharFormat()
            fmt.setForeground(QColor(color))
            for m in re.finditer(regex, text):
                start, end = m.span()
                cursor.setPosition(start)
                cursor.setPosition(end, QTextCursor.KeepAnchor)
                cursor.setCharFormat(fmt)

        fmt(r"\b\d+(\.\d+)?\b", NUMBER_COLOR)
        fmt(r"Error.*|Redefinition .*", ERROR_COLOR)
        fmt(r"\b[a-zA-Z_][a-zA-Z0-9_]*(?=\s*\()", FUNCTION_COLOR)
        cursor.endEditBlock()


class CustomConsole(QMainWindow):
    """Консоль со всеми поддерживаемыми для неё функциями
    Атрибуты:
        text (QTextEdit): текстовая область для ввода и вывода команд.
        parser (Parser): обработчик для разбора строк команд.
        history (CommandHistory): история команд.
        highlighter (SyntaxHighlighter): окраска текста
        sugg (SuggestionManager): менеджер подсказок автодополнения
        prompt (str): промпт в консоли
        command_start (int): позиция начала команды в консоли
        cursor_visible (bool): флаг видимости курсора"""

    def __init__(self) -> None:
        """Базовые настройки консоли"""
        super().__init__()

        if os.path.exists("logo.png"):
            self.setWindowIcon(QIcon("logo.png"))
        self.setWindowTitle("Ouro Shell")
        self.resize(800, 600)
        self.setStyleSheet("background-color: " + BOARD_COLOR)
        self.animating = True
        
        w = QWidget()
        self.setCentralWidget(w)
        layout = QVBoxLayout(w)
        self.text = QTextEdit()
        self.text.setFont(QFont("JetBrains Mono", 13))
        self.text.setStyleSheet("background-color:{}; color:{};".format(BACKGROUND_COLOR, TEXT_COLOR))
        self.text.setUndoRedoEnabled(False)
        self.text.installEventFilter(self)
        layout.addWidget(self.text)

        self.parser = Parser()
        self.history = CommandHistory(HISTORY_FILE)
        self.highlighter = SyntaxHighlighter(self.text)
        self.sugg = SuggestionManager(self)

        self.prompt = "Λ> "
        self.command_start = 0
        self.cursor_visible = True
        t = QTimer()
        t.timeout.connect(self.blink)
        t.start(500)

        self.animate_welcome("Welcome to Ouro Shell "
                             + "named after Kurt Friedrich Gödel\n")

    def animate_welcome(self, msg : str) -> None:
        """Отображает вступительное сообщение
        Аргументы:
            msg (str): текст вступления"""
        self.text.setReadOnly(True)
        self._animate(msg, 0)

    def _animate(self, msg : str, i : int) -> None:
        """Функция для анимации вступления
        Аргументы:
            msg (str): сообщение приветствия.
            i (int): текущий индекс символа"""
        if i < len(msg):
            self.text.moveCursor(QTextCursor.End)
            self.text.insertPlainText(msg[i])
            QTimer.singleShot(50, lambda: self._animate(msg, i + 1))
        else:
            self.animating = False
            self.text.setReadOnly(False)
            self.write_prompt()

    def write_prompt(self) -> None:
        """Добавляет промпт в текстовую область консоли"""
        self.text.moveCursor(QTextCursor.End)
        self.text.insertPlainText(self.prompt)
        self.command_start = self.text.textCursor().position()

    def blink(self) -> None:
        """Заставляет мигать курсор в консоли"""
        pal = self.text.palette()
        c = QColor(CURSOR_COLOR if self.cursor_visible else BOARD_COLOR)
        pal.setColor(self.text.palette().Text, c)
        self.text.setPalette(pal)
        self.cursor_visible = not self.cursor_visible

    def process_command(self, cmd : str) -> None:
        """Обрабатывает введённую команду
        Аргументы:
            cmd (str): введенная команда"""
        cmd_str = cmd.strip()
        if not cmd_str:
            return
        if cmd_str.lower() in ("exit", "quit"):
            self.history.add(cmd_str)
            sys.exit(0)
        if cmd_str.lower() == "clear":
            self.text.clear()
            self.write_prompt()
            return
        if cmd_str.lower() == "help":
            self.history.add(cmd_str)
            self.show_help()
            return
        self.history.add(cmd_str)
        try:
            res = self.parser.ProcessLine(cmd_str)
            if res:
                self.append(res)
        except Exception as e:
            self.append(f"Ошибка: {e}")

    def append(self, txt : str) -> None:
        """Добавляет текст в вывод консоли
        Аргументы:
            txt (str): текст для добавления"""
        self.text.moveCursor(QTextCursor.End)
        self.text.insertPlainText(str(txt) + "\n")
        self.highlighter.highlight()

    def eventFilter(self, src : QObject, e : QEvent) -> bool:
        """Обрабатывает события дизайна консоли
        Аргументы:
            src (QObject): источник события
            e (QEvent): само событие
        Возвращает:
            bool: в зависимости от успеха обработки события"""
        if e.type() == e.KeyPress and src is self.text:
            k = e.key()
            mod = e.modifiers()
            cur = self.text.textCursor()
            pos = cur.position()
            if self.animating:
                return True
            if mod & Qt.ControlModifier:
                if k in (Qt.Key_Z, Qt.Key_Y, Qt.Key_V):
                    return True
            if k in (Qt.Key_Backspace, Qt.Key_Delete):
                if pos <= self.command_start:
                    return True
            if pos < self.command_start:
                cur.setPosition(self.command_start)
                self.text.setTextCursor(cur)
            if k == Qt.Key_Home:
                cur.setPosition(self.command_start)
                self.text.setTextCursor(cur)
                return True
            if k == Qt.Key_Return:
                raw = self.text.toPlainText()[self.command_start:]
                cmd = raw.strip()
                if cmd.lower() in ("exit", "quit"):
                    self.process_command(raw)
                    return True
                self.append("")
                self.process_command(raw)
                if cmd.lower() not in ("clear",):
                    self.write_prompt()
                return True
            if k == Qt.Key_Up:
                prev = self.history.previous()
                if prev is not None:
                    cur.setPosition(self.command_start)
                    cur.movePosition(QTextCursor.End, QTextCursor.KeepAnchor)
                    cur.insertText(prev)
                    self.text.setTextCursor(cur)
                return True
            if k == Qt.Key_Down:
                nxt = self.history.next()
                cur.setPosition(self.command_start)
                cur.movePosition(QTextCursor.End, QTextCursor.KeepAnchor)
                cur.insertText(nxt)
                self.text.setTextCursor(cur)
                return True
            if k == Qt.Key_Tab:
                funcs = sorted(
                    self.parser.AllCommands()
                    )
                self.append("Доступные функции:\n" + "\n".join(funcs))
                self.write_prompt()
                return True
            if k == Qt.Key_Right and cur.atEnd():
                txt = self.text.toPlainText()[self.command_start: pos].strip()
                if self.sugg.label.isVisible():
                    sug = self.sugg.label.text().lstrip("→ ")
                    cur.setPosition(self.command_start)
                    cur.movePosition(QTextCursor.End, QTextCursor.KeepAnchor)
                    cur.insertText(sug)
                    self.sugg.label.hide()
                    return True
                if txt:
                    funcs = list(
                        self.parser.AllCommands()
                        )
                    cand = min((f for f in funcs if f > txt), default=None)
                    if cand:
                        self.sugg.show_suggestion(
                            cand, self.text.cursorRect().topRight()
                        )
                return False
        return super().eventFilter(src, e)

    def show_help(self) -> None:
        """Отображает справочную информацию о консоли, командах
        и хоткеях"""
        if not os.path.exists("README.md"):
            self.append("Файл справки README.md не найден")
            return
        try:
            with open("README.md", "r", encoding="utf-8") as f:
                self.append(f.read())
        except Exception as e:
            self.append(f"Ошибка при чтении справки: {e}")
        hotkeys = (
            "Горячие клавиши:\n"
            "  Enter   - выполнить команду\n"
            "  Up/Down - история команд\n"
            "  Tab     - список функций\n"
            "  Right   - автодополнение в конце строки\n"
            "  clear   - очистить консоль\n"
            "  exit/quit - выйти из приложении"
        )
        self.append(hotkeys)

    def closeEvent(self, e : QCloseEvent) -> None:
        """Обрабатывает закрытие консоли в ходе exit/quit
        Аргументы:
            e (QCloseEvent): событие закрытия"""
        e.accept()


if __name__ == "__main__":
    """Процесс запуска приложения и создания консоли"""
    app = QApplication(sys.argv)
    c = CustomConsole()
    c.show()
    sys.exit(app.exec_())
