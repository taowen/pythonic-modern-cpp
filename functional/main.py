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

    def test_004(self):
        #! [004]
        names = ['raymond', 'rachel', 'matthew']
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertListEqual([
            ('raymond', 'red'),
            ('rachel', 'green'),
            ('matthew', 'blue')
        ], zip(names, colors))
        #! [004]

    def test_005(self):
        #! [005]
        colors = ['red', 'green', 'blue', 'yellow']
        for i, color in enumerate(colors):
            print(i, color)
            #! [005]

    def test_006(self):
        #! [006]
        d = {'matthew': 'blue', 'rachel': 'green', 'raymond': 'red'}
        for k, v in d.iteritems():
            print(k, v)
            #! [006]

    def test_007(self):
        #! [007]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertListEqual(['blue', 'green', 'red', 'yellow'], sorted(colors))
        #! [007]

    def test_008(self):
        #! [008]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertListEqual(['yellow', 'red', 'green', 'blue'], sorted(colors, reverse=True))
        #! [008]

    def test_009(self):
        #! [009]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertListEqual(['red', 'blue', 'green', 'yellow'], sorted(colors, key=lambda e: len(e)))
        #! [009]
