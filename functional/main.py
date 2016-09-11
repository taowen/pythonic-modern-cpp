import unittest


class Test(unittest.TestCase):
    def test_001(self):
        #! [001]
        for i in xrange(6):
            print(i ** 2)
            #! [001]

    def test_002(self):
        #! [002]
        colors = ['red', 'green', 'blue', 'yellow']
        for color in colors:
            print(color)
            #! [002]
