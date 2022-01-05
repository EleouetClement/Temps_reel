# Notes cours Temps_reel
## OpenGL
### Uniforms
Les uniforms sont des variables qui permettent de passer de la donnée du programme 
qui tourne sur le CPU vers le programme qui tourne sur le GPU(**shader**)

Il est possible de set un uniform avant chaque drawCall
par exemple avant le **glDrawElements**

### Vertex arrays objects (vao)
Permet de lier des vertex buffers avec une specification 


### Les textures 

glTextureStorage2D(
	texId,
	level,(1) -> Quand on utilise une texture MitMap
	inter,
	width, 
	height,
	internalFormat,
)

#### Texture MitMap

Divisier une texture par deux jusqu'a ce que lorsqu'on
obtienne un ensemble d'image qui prend la même place que 
l'image originale. Cela sert a prendre une TexMap en fonction
de la taille de l'image et permet de ne pas faire trop de calcul
sur les texcels

### glTex[ture]SubImage(
	texId,
	level = 0,
	x0, -> offset
	y0, -> offset
	w,
	h,
	format,
	type,
	ptr -> données
)

### uniform sampler2D

uniform sampler2D colorTexture;

GLuint loc_texture_color = glProgramUniformLocation(prog, "colortexture");



glUniform(loc_texturecolor, id_TextureUnit)

glActiveTexure(GL_TEXTURE0 + id);
glBindTexture2D(texId);

ou

glBindTextureUnit(uint unit, uint texture);