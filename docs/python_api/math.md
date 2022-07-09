# Math

Engine representation of color.

## Properties

```python
PI: float
```

Constant value of `3.14159`.

## Methods

```python
lerp(source: float, destination: float, amount: float) -> float
```

Linearly interpolate between two values.

# Vector2

Engine representation of a two-dimensional vector.

## Properties

```python
x: float
```
X coordinate.

```python
y: float
```
Y coordinate.

## Methods

```python
dot_product(value: crescent_api.Vector2) -> float:
```

Returns the dot product of two Vector2 items.

```python
lerp(source: crescent_api.Vector2, destination: crescent_api.Vector2, amount: float) -> crescent_api.Vector2:
```

Returns the linearly interpolated value of two points.

---

**Static functions that return specific values**

```python
ZERO() -> Vector2(0.0, 0.0):
LEFT() -> Vector2(-1.0, 0.0):
RIGHT() -> Vector2(0.0, 1.0):
UP() -> Vector2(0.0, -1.0):
DOWN() -> Vector2(0.0, 1.0):
```

# Rect2

Engine representation of a rectangle.

## Properties

```python
x : float
```

X coordinate.

```python
y : float
```

Y coordinate.

```python
w : float
```

Rectangle's width.

```python
h : float
```

Rectangle's height.

## Methods

None.
