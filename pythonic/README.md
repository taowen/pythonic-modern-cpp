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

## code_units

```
auto chars = std::vector<char>{};
for (auto c : U8("abc") | utf8::code_units) {
    chars.push_back(c);
}
CHECK((std::vector<char>{'a', 'b', 'c'}) == chars);
```

## code_points

```
auto chars = std::vector<utf8::TextView>{};
for (auto c : U8("中文") | utf8::code_points) {
    chars.push_back(c);
}
CHECK((std::vector<utf8::TextView>{U8("中"), U8("文")}) == chars);
```