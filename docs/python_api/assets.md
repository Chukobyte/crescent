# Audio Source

Engine representation of an audio source.

## Properties

```python
file_path: str
```

File path of the audio source.

## Methods

None.

# Font

Engine representation of a font.

## Properties

```python
file_path: str
```
File path of the font.

```python
uid: str
```
Unique identifier for the font.

```python
size: int
```
Size of the font.

## Methods

None.

# Texture

Engine representation of a texture.

## Properties

```python
file_path: str
```

The file path of the texture.

```python
wrap_s: str
```

The wrap s of the texture.  Defaults to `clamp_to_border`.

```python
wrap_t: str
```

The wrap t of the texture.  Defaults to `clamp_to_border`.

```python
filter_min: str
```

The filter min of the texture.  Defaults to `nearest`.

```python
filter_mag: str
```

The filter mag of the texture.  Defaults to `nearest`.

## Methods

None.

# Animation

Engine representation of an animation.

## Properties

```python
name: str
```

The name of the animation.

```python
speed: int
```

The speed of the animation in milliseconds per frame.

```python
loops: bool
```

Whether or not the animation loops automatically when finished.

```python
frames: List[crescent_api.AnimationFrame]
```

A list of `AnimationFrame`s.

## Methods

None.

# Animation Frame

Engine representation of an animation frame.

## Properties

```python
frame: int
```

Current frame number within an animation.

```python
texture_path: str
```

The texture path of the current animation frame.

```python
draw_source: crescent_api.Rect2
```

The draw source rectangle of the animation frame.

## Methods

None.

# Input Action

Engine representation of an input action.

## Properties

```python
name: int
```

The name of the input action

```python
values: List[str]
```

List of input values for an action.

```python
device_id: int
```

The device id (e.g. used for multiple controllers).  Defaults to the first device at `0`.

## Methods

None.
