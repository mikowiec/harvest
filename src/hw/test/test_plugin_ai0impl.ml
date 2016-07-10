
(* imperative variant just as an example... *)

type point = { x: float; y: float; z: float }

type obj = { mutable pos: point;
             mutable tick: int
           }

let mkpos x y z = { x = x; y = y; z = z }

external get_time : unit -> int64 = "hw_time_get_time"

let tm = let now = get_time () in Int64.to_float now

let create () = { pos = mkpos 1. 2. 3. ; tick = 0 }

let get_pos obj = obj.pos

let think obj = obj.pos <- mkpos obj.pos.z obj.pos.x obj.pos.y;
                obj.tick <- obj.tick + 1;
                ()

let _ =
    let cr = Callback.register in
    cr "create" create; cr "get_pos" get_pos; cr "think" think;
    ()






