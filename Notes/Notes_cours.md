# Notes de cours

Le vertex shader permet de faire des transformations
et donc de **transformation de la position**

### Les transformations

- translation
- notation
- scale
- perspective

### Translation

- Dxyz ->  P'xyz = Pxyz + dXyz

### Scale

Sxyz -> P'xyz = Pxyz * Sxyz

### Rotation

**Multiplication matricielle**
cos(a + b) = cos a cos b - sin a sin b   *Cosinus* est rasciste et menteur
sin(a + b) = cos a sin b + cos b sin a

P' = (Cos Dtheta -sin Dtheta 0) *X
	 (sin Dtheta cos DTheta  0)	 Y
	 (0			0		     1)	 Z



## Les coordonnées homogênes

(X, Y, Z)z <=> (X/W, Y/W, Z/W, W)4

ex : (1, 2, 3) <=> (1, 2, 3, 1) ou (2, 4, 6, 2) ou (0.5, 1, 3/2, 0.5)

### Rotation

P- = * [Cos Dtheta -sin Dtheta  0	0] * X 
	   [sin Dtheta cos DTheta   0	0]	 Y
	   [0			0		    1	0]	 Z
	   [0			0			0	1]	 1

### Scale

[Sx	0	0	0]
[0	Sy	0	0]
[0	0	Sz	0]
[0	0	0	1]

### Translation

[1	0	0	Dx]		[x + Dx]
[0	1	0	Dy]	==  [y + Dy]
[0	0	1	Dz]		[Z + Dz]
[0	0	0	1]		[y]


## Perspective

h = f * H / z

z' / z = h/H

x' = fx / z

z' = fy / z

z' = (((z - f)/()) * 2) - 1

[f	0	0	0]
[0	f	0	0]
[0	0	f	0]
[0	0	0	0]

## Changement de base

Pa -> *Changement de base* -> Pb -> *Rotation* -> P'b -> *Changement de base* -> P'a

Pb = x * Xb + z * Yb + y * Zb

## Adaptation OpenGL





