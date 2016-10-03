# Pythonic Functions

## len

get length of string literal (count of unicode code points)
```
CHECK(2 == (U8("中文") | pyn::len));
```

get length of any container
```
CHECK(3 == ({1,2,3} | pyn::len));
```

## capitalize

```
CHECK(U8("Hello") == (U8("hello") | utf8::capitalize | utf8::to_text));
```

## center

```
CHECK(U8(" abc ") == (U8("abc") | utf8::center(5) | utf8::to_text));
```

## find

```
CHECK(1 == (U8("abc") | utf8::find(U8("b"))));
```

## finditer

```
CHECK((std::vector<size_t>{0, 3}) ==
        (U8("abcab") | utf8::finditer(U8("ab")) | ranges::to_vector));
```