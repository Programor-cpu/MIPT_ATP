class Employee:
    """Employee. Two modes job/not job and on/pff tumbler"""
    def __init__(self, name):
        """Creates worker by name

    Args:
        name (str): name itself"""
        self.name = name
        self.work_memory = []
        self.personal_memory = []
        self.is_at_work = False  

    def enter_work(self):
        """enetrs job"""
        self.is_at_work = True

    def leave_work(self):
        """leaves job"""
        self.is_at_work = False

    def add_work_task(self, task):
        """Adds task to worker

    Args:
        task (str): task itself

    Returns:
        bool: True if added. False otherwise"""
        if self.is_at_work:
            self.work_memory.append(task)
            return True
        return False

    def add_personal_event(self, event):
        """Adds personal event to worker

    Args:
        event (str): event
,
    Returns:
        bool: True if succed in adding. False otherwise"""
        if self.is_at_work:
            return False
        self.personal_memory.append(event)
        return True

    def get_current_memory(self):
        """Returns current memory of employee

    Returns:
        list: memories"""
        if self.is_at_work:
            return self.work_memory
        return self.personal_memory


class LumonOffice:
    """LumonOffice (Doofenshmirtz Evil Incorporateeed)"""
    
    def __init__(self):
        """Creates office with employees

    Args:
        employees (list): list of employees"""
        self.employees = []  

    def add_employee(self, employee):
        """Add employee

    Args:
        employee (Employee): employee itself"""
        self.employees.append(employee)

    def start_workday(self):
        """All empoyess are on job now"""
        for employee in self.employees:
            employee.enter_work()

    def end_workday(self):
        """All empoyess are not on job now"""
        for employee in self.employees:
            employee.leave_work()

    def assign_task(self, employee, task):
        """Gives employee a task

    Args:
        employee (Employee): employee
        task (str): task itself"""
        if employee in self.employees:
            employee.add_work_task(task)
