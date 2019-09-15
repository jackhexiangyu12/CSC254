let num_nodes = int_of_string (Sys.argv.(1));;

type 'a tree = Empty | Node of 'a * 'a tree * 'a tree;;

let rec enumerate_trees = function 
    | 0 -> [Empty]
    | n -> 
          let trees = ref [] in
            for i = 0 to n - 1 do
              let left_st = enumerate_trees i and right_st = enumerate_trees (n - i - 1) in
                List.iter (fun left -> List.iter (fun right -> trees := (Node(".", left, right)) :: (!trees)) right_st) left_st
            done;
            !trees
;;

let rec tree_to_string = function
    | Empty -> ""
    | Node (v, l, r) -> "(" ^ tree_to_string l ^ v ^ tree_to_string r ^ ")"
;;

let dot_strings = List.map (fun tree -> tree_to_string tree) (enumerate_trees num_nodes);;

let f elem = print_endline elem in List.iter f dot_strings;;