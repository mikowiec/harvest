
type vector2 = float * float
type vector3 = float * float * float
type vector4 = float * float * float * float
type color = vector4

type obj = {
	name 		: string;
	transform 	: vector3;
	rotation  	: vector4;
	scale 		: vector3;
	lod		: int;
	animation_data 	: int list;
	subobjects 	: string list;
	shading 	: int;
	diffuse 	: color;
	ambient 	: color;
	emission 	: color;
	specular 	: color;
	shininess 	: float;
	texture 	: string option;
	vertices 	: vector3 array;
	num_polygons 	: int;
	conn_list 	: (int * int * int) list;
	normals 	: vector3 array;
	texture_coords	: vector2 array;
}





