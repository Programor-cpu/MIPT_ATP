class Student:
    """Student with name, mood. Can botat and travel around"""

    
    def __init__(self, name):
        """Student init by name

    Args:
        name (str): name itself"""
        self.name = name
        self.mood = "sad"  
        self.current_location = None  

    def find_new_location(self, location):
        """Student finds new location

    Args:
        location (Location): new location"""
        self.current_location = location

    def bot(self):
        """Changes mood via location"""
        if self.current_location:
            self.mood = "neutral"
            if self.current_location.comfort_level >= 5:
                self.mood = "happy" 
        else:
            self.mood = "sad"
            

    def express_mood(self):
        """Returns their mood

    Returns:
        str: their mood"""
        return f"I am {self.mood}."


class Location:
    """Location with name and comfort level"""
    
    def __init__(self, name, comfort_level):
        """Creates location by following parameters

    Args:
        name (str): name itself
        comfort_level (int): comfort level (0 to 10)"""
        self.name = name
        self.comfort_level = comfort_level

    def describe(self):
        """Returns in string a description of location

    Returns:
        str: string with location name and comfort level"""
        return f"Место: {self.name}, Уровень комфорта: {self.comfort_level}/10"


class University:
    """University. Contains students and able to see their transportations"""
    
    def __init__(self):
        """University init. Empty at the beggining"""
        
        self.students = []
        self.locations = [] 

    def add_student(self, student):
        """Student adds to university

        Args:
            student (Student): student"""
        self.students.append(student)

    def add_location(self, location):
        """Location adds to university

    Args:
        location (Location): location"""
        self.locations.append(location)

    def assign_location(self, student, location):
        """Assigns location for student

    Args:
        student (Student): student
        location (Location): location

    Returns:
        bool: Yes if student travelled to that location. False otherwise"""
        for one_location in self.locations:
            if one_location.name == location.name:
                student.find_new_location(one_location)
                return True
        return False

    def check_student_moods(self):
        """Returns students' happiness status

    Returns:
        bool: True if all are happy. False otherwise"""
        for student in self.students:
            if student.mood == "sad":
                return False
        return True
