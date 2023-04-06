"""
    Singleton unit tests

    @author Ovidiu Ciule
"""
from util.Singleton import *
import unittest

class TestSingletonClass(unittest.TestCase):

    def testSingleton(self):
        s1 = Singleton()
        s2 = Singleton()
        self.failUnlessEqual(id(s1), id(s2), "different ids")
        self.failUnlessEqual(s1, s2, "different objects")

    class SingChild(Singleton):
        def __init__(self, x):
            self.__x = x

    def testSingletonChild(self):
        s1 = self.SingChild(3)
        s2 = self.SingChild(5)
        self.failUnlessEqual(id(s1), id(s2), "different ids")
        self.failUnlessEqual(s1, s2, "different objects")

if __name__ == '__main__':
    unittest.main()
