(* Module signature for types that can be linked *)
module type Linkable = sig
  type t

  val get_next : t -> t option
  val set_next : t -> t option -> unit
end

(* Functor to create linked list operations for any Linkable type *)
module MakeSLL (Node : Linkable) = struct
  (* Insert a node after the given node *)
  let insert_after node new_node =
    let next_node = Node.get_next node in
    Node.set_next node (Some new_node);
    Node.set_next new_node next_node

  (* Insert a node at the front of the singly linked list *)
  let insert_front new_node existing_head =
    Node.set_next new_node (Some existing_head);
    new_node

  (* Insert a node at the end of the singly linked list *)
  let rec insert_back new_node = function
    | None -> new_node
    | Some node -> (
        match Node.get_next node with
        | None ->
            Node.set_next node (Some new_node);
            node
        | Some next ->
            ignore (insert_back new_node (Some next));
            node)
end

(* Example usage with UI_BOX type *)
type ui_box = { mutable next : ui_box option; data : int }

(* UI_BOX implementation of Linkable *)
module UI_Box_Linkable = struct
  type t = ui_box

  let get_next box = box.next
  let set_next box next = box.next <- next
end

(* Create the UI_BOX-specific linked list operations *)
module UI_Box_SLL = MakeSLL (UI_Box_Linkable)

(* Helper functions for UI_BOX *)
let create_ui_box data = { next = None; data }

let rec print_list = function
  | None -> Printf.printf "End of list\n"
  | Some node ->
      Printf.printf "UI_BOX(data=%d) -> " node.data;
      print_list node.next

let%expect_test "Test singly linked list operations" =
  let box1 = create_ui_box 35 in
  let box2 = create_ui_box 358 in
  let box3 = create_ui_box 338 in
  UI_Box_SLL.insert_after box1 box2;
  UI_Box_SLL.insert_after box2 box3;
  print_list (Some box1);
  [%expect
    {| UI_BOX(data=35) -> UI_BOX(data=358) -> UI_BOX(data=338) -> End of list |}]

let%test "t" = Int.equal 1 1
