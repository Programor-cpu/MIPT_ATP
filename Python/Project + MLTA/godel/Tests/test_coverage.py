import unittest
from unittest.mock import patch, MagicMock

# Импортируем нужные классы из Console.py
from Console import Function, Node, Executer, Parser, SpecialBehavior, UndefinedFunctionException

class TestFunction(unittest.TestCase):
    def test_simple_non_recursive_function(self):
        f = Function("S(0)", "f")
        self.assertFalse(f.recursive)
        self.assertEqual(f.name, "f")
        self.assertIsNotNone(f.option)

    def test_recursive_function(self):
        f = Function("Z;S(P(1))", "f")
        self.assertTrue(f.recursive)
        self.assertIsNotNone(f.zero_option)
        self.assertIsNotNone(f.option)

class TestNode(unittest.TestCase):
    def test_leaf_node_primitive(self):
        n = Node("Z")
        self.assertTrue(n.is_leaf)
        self.assertTrue(n.is_primitive)

    def test_non_leaf_node(self):
        n = Node("S(P(1))")
        self.assertFalse(n.is_leaf)
        self.assertTrue(n.is_primitive)
        self.assertEqual(len(n.sons), 1)

class TestExecuter(unittest.TestCase):
    def test_execute_primitive(self):
        res = Executer.ExecutePrimitiveOnTheSpot("P1", [5])
        self.assertEqual(res, 5)

    def test_execute_sum(self):
        res = Executer.ExecutePrimitiveOnTheSpot("S1^3", [2])
        self.assertEqual(res, 5)

    def test_execute_fail(self):
        with self.assertRaises(UndefinedFunctionException):
            Executer.Execute("non_existing", [])

class TestParser(unittest.TestCase):
    def test_parse_arguments(self):
        args = Parser.ParseArguments__("f(1, 2, 3)")
        self.assertEqual(args, [1,2,3])

    def test_invalid_argument(self):
        with self.assertRaises(UndefinedFunctionException):
            Parser.ParseArguments__("f(1, x, 3)")

    def test_simplifying_line(self):
        line = Parser.SimplifyingLine__(" f( 1,2) # some comment")
        self.assertEqual(line, "f(1,2)")

    def test_module_activation_and_nprime_execution(self):
        result = Parser.ModuleActivation__('Primes')  # путь к файлу Primes


        # Теперь можно попробовать что-то выполнить
        res_nprime_0 = Executer.Execute("nPrime", [0])
        self.assertEqual(res_nprime_0, 2)

        res_nprime_5 = Executer.Execute("nPrime", [5])
        self.assertEqual(res_nprime_5, 13)

        res_nprime_10 = Executer.Execute("nPrime", [10])
        self.assertEqual(res_nprime_10, 31)

        res_sum_0_0 = Executer.Execute("Sum", [0, 0])
        self.assertEqual(res_sum_0_0, 0)

        res_sum_2_3 = Executer.Execute("Sum", [2, 3])
        self.assertEqual(res_sum_2_3, 5)

        res_sum_5_5 = Executer.Execute("Sum", [5, 5])
        self.assertEqual(res_sum_5_5, 10)

        # Mul tests
        res_mul_0_0 = Executer.Execute("Mul", [0, 0])
        self.assertEqual(res_mul_0_0, 0)

        res_mul_2_3 = Executer.Execute("Mul", [2, 3])
        self.assertEqual(res_mul_2_3, 6)

        res_mul_5_5 = Executer.Execute("Mul", [5, 5])
        self.assertEqual(res_mul_5_5, 25)

        # Pow tests
        res_pow_2_0 = Executer.Execute("Pow", [2, 0])
        self.assertEqual(res_pow_2_0, 1)

        res_pow_2_3 = Executer.Execute("Pow", [2, 3])
        self.assertEqual(res_pow_2_3, 8)

        res_pow_3_2 = Executer.Execute("Pow", [3, 2])
        self.assertEqual(res_pow_3_2, 9)

        res_min_2_3 = Executer.Execute("Min", [2, 3])
        self.assertEqual(res_min_2_3, 2)

        res_min_5_1 = Executer.Execute("Min", [5, 1])
        self.assertEqual(res_min_5_1, 1)

        res_min_4_4 = Executer.Execute("Min", [4, 4])
        self.assertEqual(res_min_4_4, 4)

        # Max tests
        res_max_2_3 = Executer.Execute("Max", [2, 3])
        self.assertEqual(res_max_2_3, 3)

        res_max_5_1 = Executer.Execute("Max", [5, 1])
        self.assertEqual(res_max_5_1, 5)

        res_max_4_4 = Executer.Execute("Max", [4, 4])
        self.assertEqual(res_max_4_4, 4)

        # Eq tests
        res_eq_2_2 = Executer.Execute("Eq", [2, 2])
        self.assertEqual(res_eq_2_2, 1)

        res_eq_5_1 = Executer.Execute("Eq", [5, 1])
        self.assertEqual(res_eq_5_1, 0)

        res_eq_0_0 = Executer.Execute("Eq", [0, 0])
        self.assertEqual(res_eq_0_0, 1)

class TestSpecialBehavior(unittest.TestCase):
    def test_activate_python(self):
        self.assertEqual(SpecialBehavior.ActivateSpecialBehavior("python"), "Linus Torwalds")

    def test_activate_unknown(self):
        self.assertEqual(SpecialBehavior.ActivateSpecialBehavior("unknown"), "")

class TestUndefinedFunctionException(unittest.TestCase):
    def test_add_to_begin_end(self):
        e = UndefinedFunctionException("error")
        e.AddToBegin("Start:").AddToEnd("End.")
        self.assertIn("Start:", e.str_error)
        self.assertIn("End.", e.str_error)

if __name__ == '__main__':
    unittest.main()
