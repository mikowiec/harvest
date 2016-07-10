
open Objtype


val default : obj;;
val tree : obj;;

val read : in_channel -> obj;;

val lines : string -> string list;;

val load : string -> obj;;
val n_tuple : string -> string list;;

