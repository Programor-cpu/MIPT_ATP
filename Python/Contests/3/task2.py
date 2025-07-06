class Author:
    """Author class, books as class field"""
    
    def __init__(self, name):
        """Author init by his name

    Args:
        name (str): name itself"""
        self.name = name
        self.books = []

    def add_book(self, book):
        """adds book to author

    Args:
        book (Book): book"""
        book.author = self
        self.books.append(book)
        


class Book:
    """Book class. Can't do anything"""
    
    def __init__(self, title, author, year, format=""):
        """Creates book

     Args:
        title (str): book's title
        author (str): author's name
        year (str): year of production"""
        self.title = title
        self.author = author
        self.year = year
        self.format = format
        author.add_book(self)


class Library:
    """ Library conatins books and is able to find them by author or format"""
    
    def __init__(self):
        """Library init

    Args:
        books (list): book list"""
        self.books = []  

    def add_book(self, book):
        """adds book

    Args:
        book (Book): book itself"""
        self.books.append(book) 

    def find_books_by_author(self, author_name):
        """Returns book by author

    Args:
        author_name (str): their name

    Returns:
        list: list of their book"""
        author_books = [book for book in self.books if book.author.name == author_name]
        return author_books

    def find_books_by_format(self, format):
        """Returns book by format

    Args:
        format_ (str): format itself

    Returns:
        list: books list by format"""
        format_books = [book for book in self.books if book.format == format]
        return format_books
