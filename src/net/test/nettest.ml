
open Printf
open Unix

let with_connect host port body = 
  let hent = gethostbyname host in
  let addr = Array.get hent.h_addr_list 0 in
  let (ic,oc) = open_connection (ADDR_INET (addr,port)) in
  let restore _ = shutdown_connection ic in
  let _    = try body ic oc with e -> restore (); raise e in
  restore ()


let with_server port body = 
  let addr = inet_addr_of_string "127.0.0.1" in
  establish_server body (ADDR_INET (addr,port))

let rec trim s = let last = String.length s - 1 in
  match String.get s last with
     '\r' | '\n' -> trim (String.sub s 0 last)
   | _ -> s

let in_line c = trim (input_line c)

let in_int c = int_of_string (in_line c)

let out_line c s = output_string c (s^"\r\n"); flush c

let simul_server _ =  ()

exception Inv_format

let split_cline str =
  if Str.string_match (Str.regexp "\([^: \t]+\)[ \t]*:[ \t]*\(.*\)") str 0 
   then (Str.matched_group 1 str, Str.matched_group 2 str)
    else (print_string ("not a valid config line: " ^ str); raise Inv_format)

let get_cline ic = split_cline (in_line ic)

let game_info ic oc =
    out_line oc "gameinfo";
    let state = int_of_string (in_line ic) in
    let rec ply_loop acc = 
      match in_line ic with
        "end" -> acc
      | str -> ply_loop ((int_of_string str) :: acc) in
    let ps = ply_loop [] in
    printf "state %d, players %d\n" state (List.length ps)

let join ic oc =
    out_line oc "join";
    let id = in_int ic in
    printf "joined with id %d\n" id

let simul_client body = with_connect "localhost" 4247
 (fun ic oc -> 
    let inp = in_line ic in
    print_string ("connected, resp: " ^ inp ^ "!\n");
    body ic oc;
    out_line oc "quit")



let run_test _ = simul_client game_info
 



let _ = if Array.length Sys.argv == 1 then ()
 else match Array.get Sys.argv 1 with 
      | "-c" -> simul_client game_info
      | "-j" -> simul_client join
      | "-w" -> simul_client (fun ic oc -> game_info ic oc; join ic oc; let _ = Sys.command "sleep 10" in ())
      | "-s" -> simul_server ()
      | _ -> ()


