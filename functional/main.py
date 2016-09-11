import unittest


class Test(unittest.TestCase):
    def test_aaa(self):
        ##! [aaa]
        for i in xrange(6):
            print(i ** 2)
            ##! [aaa]
