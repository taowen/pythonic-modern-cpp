# coding=utf-8
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

    def test_003(self):
        #! [003]
        self.assertEqual(u'文', u'中文'[1])
        self.assertEqual(2, len(u'中文'))
        self.assertEqual(6, len('中文'))
        #! [003]

    def test_004(self):
        #! [004]
        self.assertEqual('中文', u'中文'.encode('utf8'))
        #! [004]

    def test_005(self):
        #! [005]
        self.assertListEqual(['hello', 'world'], 'hello world'.split(' '))
        #! [005]

    def test_006(self):
        #! [006]
        # small
        self.assertEqual('hello world', 'hello' + ' world')
        # large
        parts = []
        parts.append('h')
        parts.append('e')
        parts.append('ll')
        parts.append('o')
        self.assertEqual('hello', ''.join(parts))
        #! [006]