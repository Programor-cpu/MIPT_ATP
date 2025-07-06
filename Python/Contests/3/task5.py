import random
import string


class Commit:
    """Commit with message, hash and links prev and next"""

    def __init__(self, hash, message, prev):
        """Creates commit.

        Args:
            hash (str): its hash
            message (str): message
            prev (Commit): ref to prev commit"""
        self.hash = hash
        self.message = message
        self.next = None
        self.prev = prev

    def __str__(self):
        """Returns hash and message in str

        Returns:
            str: information"""
        return f"[{self.hash}]: {self.message}"


class Repository:
    """Repository - double connected list with commits in it"""

    def __init__(self):
        """Creates empty repository"""
        self.hashes = set()
        self.head = None
        self.tail = None

    def add_commit(self, hash, message):
        """Adds commit

        Args:
            hash (str): hash
            message (str): commit's message"""
        self.hashes.add(hash)
        new_commit = Commit(hash, message, self.head)
        if not self.head:
            self.head = new_commit
            self.tail = new_commit
        else:
            self.head.next = new_commit
            self.head = new_commit

    def remove_commit(self, hash):
        """Deletes commit by its hash

        Args:
            hash (str): commit's hash"""
        current = self.head
        while current:
            if current.hash == hash:
                if not current.prev:
                    if not current.next:
                        self.head = None
                        self.tail = self.head
                    else:
                        self.tail.prev = None
                        self.tail = current.next
                else:
                    if not current.next:
                        self.head.next = None
                        self.head = current
                    else:
                        current.prev.next = current.next
                        current.next.prev = current.prev
                return
            self.hashes.remove(current.hash)
            current = current.prev

    def get_commit(self, hash):
        """Returns commit by its hash

        Args:
            hash (str): looking for hash

        Returns:
            Commit: if it is in
            None: otherwise"""
        if hash not in self.hashes:
            return None
        current = self.head
        while current:
            if current.hash == hash:
                return current
            current = current.prev
        return None

    def print_history(self):
        """Prints history of commits"""
        current = self.tail
        while current:
            print(current.str())
            current = current.next

    def revert_to_commit(self, hash):
        """Reverts to commit by hash

        Args:
            hash (str): hash of new head"""
        if hash not in self.hashes:
            return
        current = self.head
        while current:
            if current.hash == hash:
                self.head = current
                current.next = None
                return
            self.hashes.remove(current.hash)
            current = current.prev
        self.head = None
        self.tail = self.head

    @staticmethod
    def generate_hash(self=None):
        """Generates new hash never used before
        Returns:
            str: generated hash"""
        alphabet = string.ascii_lowercase + string.digits
        hash = "".join(random.choices(alphabet, k=6))
        if self is not None:
            while hash in self.hashes:
                hash = "".join(random.choices(chars, k=6))
            return hash
        return hash

    @classmethod
    def from_list(cls, commits):
        """Creates repository by commits

        Args:
            commits (list): list of commits

        Returns:
            Repository: repository with commits inside"""
        repo = cls()
        for commit in commits:
            repo.add_commit(commit["hash"], commit["message"])
        return repo
