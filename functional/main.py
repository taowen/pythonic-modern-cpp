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

    def test_010(self):
        #! [010]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertListEqual([3, 5, 4, 6], [len(color) for color in colors])
        #! [010]

    def test_011(self):
        #! [011]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertTrue(any(color == 'green' for color in colors))
        #! [011]

    def test_012(self):
        #! [012]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertListEqual(['green'], colors[1:2])
        self.assertListEqual(['red', 'green'], colors[:2])
        self.assertListEqual(['green', 'blue', 'yellow'], colors[1:])
        self.assertListEqual(['red', 'green', 'blue', 'yellow'], colors[:-1])
        #! [012]

    def test_013(self):
        #! [013]
        d = {'matthew': 'blue', 'rachel': 'green', 'raymond': 'red'}
        for k in d.keys():
            if k.startswith('r'):
                del d[k]
        self.assertListEqual(['matthew'], d.keys())
        #! [013]

    def test_014(self):
        #! [014]
        colors = ['red', 'green', 'blue', 'yellow']
        self.assertDictEqual({
            'red': 3,
            'green': 5,
            'blue': 4,
            'yellow': 6
        }, dict((color, len(color)) for color in colors))
        #! [014]