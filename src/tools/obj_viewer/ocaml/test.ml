
open Objtype

let translate (x,y,z) () = GlMat.translate ~x ~y ~z ()
let rotate (a,x,y,z) () = GlMat.rotate ~angle:a ~x ~y ~z ()
let scale (x,y,z) () = GlMat.scale ~x ~y ~z ()
let color (r,g,b) () = GlDraw.color ~alpha:1.0 (r, g, b) 

exception Duh

open Rgb24

let make_image () =
  let image =
    GlPix.create `ubyte ~format:`rgb ~width:512 ~height:512 in
  for i = 0 to 512 - 1 do
    for j = 0 to 512 - 1 do
      Raw.sets (GlPix.to_raw image) ~pos:(3*(i*512+j))
	(if (i land 8 ) lxor (j land 8) = 0
	 then [|255;255;255|]
	 else [|0;0;0|])
    done
  done;
  image

let topdir = "../../../../data/textures/"
let img_hash = Hashtbl.create 10


let load_png tex = 
	let png = match Png.load_as_rgb24 (topdir ^ tex ^ ".png") [] with
	 				  Image.Rgb24 png -> png
					| _ -> raise Duh
	in
	let raw = Raw.create `ubyte (3 * png.width * png.height) in
	for y = 0 to png.height - 1 do
		for x = 0 to png.width - 1 do
			Raw.set raw (3 * (y * png.width + x)) (unsafe_get png x y).Color.r;
			Raw.set raw (3 * (y * png.width + x) + 1) (unsafe_get png x y).Color.g;
			Raw.set raw (3 * (y * png.width + x) + 2) (unsafe_get png x y).Color.b;
		done;
	done;
	GlPix.of_raw raw `rgb png.width png.height


        

let load_tex tex = 
    try Hashtbl.find img_hash tex
    with Not_found -> let pix = try load_png tex
                                with _ -> load_png (try Sys.argv.(2) with _ -> "siffror")
                      in Hashtbl.add img_hash tex pix; pix

let mk_obj_list o =
	let list = GlList.create `compile in
	GlMat.push ();
	translate o.transform ();
	rotate o.rotation ();
	scale o.scale ();
	GlLight.material ~face:`front (`ambient o.ambient);
	GlLight.material ~face:`front (`diffuse o.diffuse);
	GlLight.material ~face:`front (`emission o.emission);
	GlLight.material ~face:`front (`shininess o.shininess);
	let tex = match o.texture with None -> "" | Some a -> a in
        GlPix.store (`unpack_alignment 1);
	GlTex.image2d (load_tex tex);
	GlTex.parameter `texture_2d (`mag_filter `linear);
	GlTex.parameter `texture_2d (`min_filter `linear);
	GlTex.parameter `texture_2d (`wrap_s `clamp);
	GlTex.parameter `texture_2d (`wrap_t `clamp);
	GlTex.env (`mode `replace);
        Gl.enable `texture_2d;
	GlDraw.begins `triangles;
	List.iter ~f:(fun (a,b,c) ->
				let f i =
					GlDraw.normal3 o.normals.(i);
					GlTex.coord2 o.texture_coords.(i);
					GlDraw.vertex3 o.vertices.(i);
				in f a; f b; f c)
			o.conn_list;
	GlDraw.ends ();
	GlMat.pop ();
	GlList.ends ();
	list


let init_gl _ =
    List.iter Gl.enable [`lighting; `light1; `cull_face; `depth_test; `texture_2d];
    let lst = [`position (0., 0., -250., 1.);
    	       `ambient (1., 1., 1., 0.5);
	       `diffuse (1., 1., 1., 1.) ]
    in List.iter ~f:(GlLight.light ~num:1) lst;

class view togl = object (self)
  val mutable view_rotx = 0.0
  val mutable view_roty = 0.0
  val mutable view_rotz = 0.0
  val mutable scale_f = 1.0
  val mutable count = 1

  val mutable objs = []

  method add_obj o = objs <- (mk_obj_list o :: objs)
  
  method rotx a = view_rotx <- a
  method roty a = view_roty <- a
  method scale v = scale_f <- (v /. 500.)

  method draw =
    GlClear.color (0.0,0.0,0.0);
    GlClear.clear [`color;`depth];

    GlMat.push ();
    let k = 1.0 /. scale_f in GlMat.scale ~x:k ~y:k ~z:k ();
    GlMat.rotate ~angle:view_rotx ~x:1.0 ();
    GlMat.rotate ~angle:view_roty ~y:1.0 ();
    GlMat.rotate ~angle:view_rotz ~z:1.0 ();
    List.iter GlList.call objs;
    GlMat.pop ();
    
   Togl.swap_buffers togl;

    count <- count + 1;

  method idle = 
    self#draw

  method reshape =
    let w = Togl.width togl and h = Togl.height togl in
    GlDraw.viewport ~x:0 ~y:0 ~w ~h;
    GlMat.mode `projection;
    GlMat.load_identity ();
    GluMat.perspective 70. 1. (1., 1000.);
    GlMat.mode `modelview;
    GlMat.load_identity();
    GluMat.look_at (0., 0., -20.) (0., 0., 0.) (0., 1., 0.);
    GlClear.color (0.0,0.0,0.0);
    GlClear.clear [`color;`depth];
    init_gl ()
end

let arg = try Sys.argv.(1)
          with _ -> "ship2"

let rec load_objs v n =
  let obj = Object.load n in
  v#add_obj obj;
  List.iter (fun s -> load_objs v (n ^ "_" ^ s)) obj.subobjects

open Tk

let main () =

  let top = openTk () in
  let f = Frame.create top in
  let f2 = Frame.create f in
  let my_scale ?name =
    Scale.create ?name ~min:0. ~max:360. ~showvalue:false
      ~highlightbackground:`Black in
  let my_scale2 ?name =
    Scale.create ?name ~min:0. ~max:2000. ~showvalue:false
      ~highlightbackground:`Black in
  let togl =
    Togl.create ~width:300 ~height:300
      ~rgba:true ~depth:true ~double:true f
  and sh = my_scale ~orient:`Horizontal f
  and sh2 = my_scale2 ~orient:`Horizontal f
  and sv = my_scale ~orient:`Vertical top
  in
  
  Wm.title_set top "Test";

  let view = new view togl in
  Scale.configure ~command:(view#rotx) sv;
  Scale.configure ~command:(view#roty) sh;
  Scale.configure ~command:(view#scale) sh2;
  Scale.set sh 20.; Scale.set sv 40.; Scale.set sh2 100.0;
  Togl.reshape_func togl ~cb:(fun () -> view#reshape);
  Togl.display_func togl ~cb:(fun () -> view#draw);
  Togl.timer_func ~ms:20 ~cb:(fun () -> view#idle);
  init_gl ();
  let oname = arg in
  load_objs view oname;
  pack ~side:`Right ~fill:`Y [sv];
  pack ~side:`Bottom ~fill:`X [sh];
  pack ~side:`Bottom ~fill:`X [sh2];
  pack ~side:`Bottom ~fill:`X [f2];
  pack ~expand:true ~fill:`Both [f];
  pack ~side:`Top ~expand:true ~fill:`Both [togl];
  Tk.mainLoop ()

let _ = if !Sys.interactive then () else main ()


