
open Objtype

let default = {
	name = "default";
	transform = 0.0, 0.0, 0.0;
	rotation = 0.0, 0.0, 0.0, 1.0;
	scale = 1.0, 1.0, 1.0;
	lod = 0;
	animation_data = [];
	subobjects = [];
	shading = 0;
	diffuse = 0.0, 0.0, 0.0, 1.0;
	ambient = 0.0, 0.0, 0.0, 0.0;
	emission = 0.0, 0.0, 0.0, 0.0;
	specular = 0.0, 0.0, 0.0, 1.0;
	shininess = 0.0;
	texture = None;
	vertices = [| |];
	num_polygons = 4;
	conn_list = [];
	normals = [| |];
	texture_coords = [| |];
	}

let tree = { default with
	name = "tree";
	scale = 15.0, 15.0, 15.0;
	vertices = [|
		-0.5, -0.5, 0.0;
		 0.5, -0.5, 0.0;
		-0.5,  0.5, 0.0;
		 0.5,  0.5, 0.0;
		-1.13789e-015, -0.5,  0.5;
		-1.13789e-015, -0.5, -0.5;
		 1.13789e-015,  0.5,  0.5;
		 1.13789e-015,  0.5, -0.5
		|];
	num_polygons = 4;
	conn_list = [0, 1, 2; 1, 3, 2; 4, 5, 6; 5, 7, 6];
	normals = [|
		0.0, 0.0, 1.0; 0.0, 0.0, 1.0;
		0.0, 0.0, 1.0; 0.0, 0.0, 1.0;
		1.0, -2.27596e-015, 0.0; 1.0, -2.27596e-015, 0.0;
		1.0, -2.27596e-015, 0.0; 1.0, -2.27596e-015, 0.0;
		|];
	texture_coords = [|
		0.0, 0.0; 1.0, 0.0;
		0.0, 1.0; 1.0, 1.0;
		0.0, 0.0; 1.0, 0.0;
		0.0, 1.0; 1.0, 1.0;
		|];
	}

let ssub pos len s = try String.sub s pos len
		     with Invalid_argument _ -> ""

let getvar s = 
	let i = String.index s ':' in
	String.sub s 0 i, String.sub s i (String.length s - i)

let n_tuple s = 
	let i = String.index s '['
	and j = String.index s ']' in
	Str.split (Str.regexp "[ \t,]+") (ssub (i+1) (j-i-1) s)

let float_n_tuple l = List.map float_of_string (n_tuple l)
let int_n_tuple l = List.map int_of_string (n_tuple l)

let str_list s =
	let i = String.index s '['
	and j = String.index s ']' in
	Str.split (Str.regexp "[, ]+") (ssub (i+1) (j-i-1) s)

let parse_vec3 s = match float_n_tuple s with
			[a; b; c] -> a, b, c | _ -> 0., 0., 0.
let parse_vec4 s = match float_n_tuple s with
			[a; b; c; d] -> a, b, c, d | _ -> 0., 0., 0., 0.

let parse_vec2 s = match float_n_tuple s with
			[a; b] -> a, b | _ -> 0., 0.
let parse_veci3 s = match int_n_tuple s with
			[a; b; c] -> a, b, c | _ -> 0, 0, 0


let int3_list s = []
let mk_vec2_arr n = Array.create n (1., 1.)
let mk_vec3_arr n = Array.create n (1., 1., 1.)

let maybe_str = function
	  "" -> None
	| s -> Some s

let parse_tbl : (string, Objtype.obj -> string -> Objtype.obj) Hashtbl.t =
	let tbl = Hashtbl.create 30 in
	let _ = List.map ~f:(fun (a,b) -> Hashtbl.add tbl ~key:a ~data:b)
	[ "name", (fun o v -> { o with name = v}) ;
	  "transform", (fun o v -> { o with transform = parse_vec3 v}) ;
	  "rotation", (fun o v -> { o with rotation = parse_vec4 v}) ;
	  "scale", (fun o v -> { o with scale = parse_vec3 v }) ;
	  "lod", (fun o v -> { o with lod = int_of_string v}) ;
	  "animation_data", (fun o v -> { o with animation_data = [] }) ;
	  "subobjects", (fun o v -> { o with subobjects = str_list v }) ;
	  "shading data", (fun o v -> { o with shading = int_of_string v }) ;
	  "diffuse_color", (fun o v -> { o with diffuse = parse_vec4 v }) ;
	  "ambient", (fun o v -> { o with ambient = parse_vec4 v }) ;
	  "emission_color", (fun o v -> { o with emission = parse_vec4 v }) ;
	  "specular_color", (fun o v -> { o with specular = parse_vec4 v }) ;
	  "shininess", (fun o v -> { o with shininess = float_of_string v }) ;
	  "texture", (fun o v -> { o with texture = maybe_str v }) ;
	  "number of polygons", (fun o v -> { o with num_polygons = int_of_string v }) ;
	  "conn_list", (fun o v -> { o with conn_list = int3_list v }) ;
	  "number of vertices", (fun o v -> let n = (int_of_string v) in
	  				{ o with vertices = mk_vec3_arr n;
						 normals  = mk_vec3_arr n;
						 texture_coords = mk_vec2_arr n}) ;
	]
	in tbl

let lines = Str.split ~sep:(Str.regexp "[\r\n]+")

let split_at i s = ssub 0 i s, ssub (i+2) (String.length s - i - 2) s

let rec fill_vertices fc i n arr =
	if i = n then arr
	else let line = input_line fc in
	     let j = String.index line ':' in
	     let sub = ssub (j+1) (String.length line - j - 1) line in
	     let a0,a1,a2,_ = parse_vec4 sub in
	     arr.(i) <- a0,a1,a2 ;
	     fill_vertices fc (i+1) n arr

let rec fill_normals fc i n arr =
	if i = n then arr
	else let line = input_line fc in
	     let j = String.index line ':' in
	     let sub = ssub (j+1) (String.length line - j - 1) line in
	     let a0,a1,a2 = parse_vec3 sub in
	     arr.(i) <- a0,a1,a2 ;
	     fill_normals fc (i+1) n arr

let rec fill_texcoords fc i n arr =
	if i = n then arr
	else let line = input_line fc in
	     let j = String.index line ':' in
	     let sub = ssub (j+1) (String.length line - j - 1) line in
	     let f0,f1 = parse_vec2 sub in
	     arr.(i) <- f0,f1 ;
	     fill_texcoords fc (i+1) n arr

let rec fill_connlst fc n xs =
	if n = 0 then xs
	else let line = input_line fc in
	     let j = String.index line ':' in
	     let sub = ssub (j+1) (String.length line - j - 1) line in
	     let it = parse_veci3 sub in
	     fill_connlst fc (n-1) (it::xs)

let sub3 s = (ssub 3 (String.length s - 4) s)

let ext_data fc obj line = match line.[2] with
   | 'l' -> let line' = input_line fc in
   	    let it = parse_veci3 (sub3 line') in
	    let cs = fill_connlst fc (obj.num_polygons - 1) [it] in
	    { obj with conn_list = cs }
   | 'x' -> let vs = obj.texture_coords in
   	    let line' = input_line fc in
	    let f0,f1 = parse_vec2 (sub3 line') in
	    vs.(0) <- f0,f1 ;
	    fill_texcoords fc 1 (Array.length vs) vs ;
	    obj
   | 'r' -> let vs = obj.normals in
   	    let line' = input_line fc in
	    let f0,f1,f2 = parse_vec3 (sub3 line') in
	    vs.(0) <- f0,f1,f2 ;
	    fill_normals fc 1 (Array.length vs) vs ;
	    obj
   | ':' -> let vs = obj.vertices in
	    let f0,f1,f2,_ = parse_vec4 (sub3 line) in
	    vs.(0) <- f0,f1,f2 ;
	    fill_vertices fc 1 (Array.length vs) vs ;
	    obj
   | _ -> obj


let rec read_bit tbl obj fc =
	try let line = input_line fc in
             try let i = String.index line ':' in
  		  let (var, arg) = split_at i line in
                  read_bit tbl (Hashtbl.find tbl (String.lowercase var) obj arg) fc
	     with _ -> read_bit tbl (ext_data fc obj line) fc
	with End_of_file -> obj

let read fc = read_bit parse_tbl default fc

let load o = let fc = open_in ("../../../../data/objects/" ^ o)
	     in read fc

