

let create () = ()

let get_pos obj = 0., 0., 0.

let think obj = ()


let _ =
    let () = List.iter (fun (n,f) -> Callback.register n f)
            ["create", create; "get_pos", get_pos; "think", think]
    in ()


