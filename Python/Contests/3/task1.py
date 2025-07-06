class Student:
    """ Student class with grades, grant and etc information"""
    
    def __init__(self, name, surname, age):
        """creates class via following data

    Args:
        name (str): name itself
        surname (str): surname itself
        age (int): age itself"""
        self.name = name
        self.surname = surname
        self.age = age
        self.grades_of_subject = {} 
        self.is_grants = False
        
    def grants_status_update(self):
        """Function to update student status"""
        
        if self.average_grade() < 8:
            self.is_grants = False
        else:
            self.is_grants = True

    def add_grade(self, subject, grade):
        """Adds grage for the following subject

        Args:
            subject (str): subject itself
            grade (str): grade itself"""
        if grade >= 1 and grade <= 10:
            if subject not in self.grades_of_subject:
                self.grades_of_subject[subject] = []  
            self.grades_of_subject[subject].append(grade) 
            self.grants_status_update()  
            return True
        return False

    def has_grants(self):
        """Returns the grant status

        Returns:
            bool: grant status. True if student has it, otherwise False"""
        return self.is_grants

    def average_grade_for_subject(self, subject):
        """Returns average grade for subject. If there are no grades returns 0

        Args:
            subject (str): subject itself

        Returns:
            float: average for subject"""
        if subject in self.grades_of_subject and self.grades_of_subject[subject]:
            return sum(self.grades_of_subject[subject]) / len(self.grades_of_subject[subject])
        return 0.0

    def average_grade(self):
        """Returns average grade for all subjects. If there are no grades returns 0

        Returns:
            float: average"""
        every_grade = [grade for grades in self.grades_of_subject.values() for grade in grades]
        if len(every_grade)>0:
            return sum(every_grade) / len(every_grade)
        return 0
    

