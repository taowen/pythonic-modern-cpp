# Pythonic Functions

## len

get length of string literal (count of unicode code points)
```
CHECK(2 == pyn::len(U8("中文")));
```

get length of any container
```
CHECK(3 == pyn::len({1,2,3}));
```

## capitalize

```
CHECK(U8("Hello") == (U8("hello") | utf8::capitalize | utf8::to_text));
```