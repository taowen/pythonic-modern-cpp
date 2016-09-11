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

    def test_003(self):
        #! [003]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertListEqual(['yellow', 'blue', 'green', 'red'], list(reversed(colors)))
        #! [003]
