import unittest


class Test(unittest.TestCase):
    def test_001(self):
        #! [001]
        self.assertEqual('o w', 'hello world'[4:7])
        #! [001]

    def test_002(self):
        #! [002]
        self.assertEqual('abc\ndef', """abc
def""")
        #! [002]