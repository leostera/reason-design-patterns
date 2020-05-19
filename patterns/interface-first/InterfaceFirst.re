module SmallBakery = {
  type bread =
    | Baked
    | Prepared
    | Ordered;
  type stock = list(bread);
};
module BakeryLimited: {
  type bread =
    | Baked
    | Prepared
    | Ordered;
  type stock;
} = {
  type bread =
    | Baked
    | Prepared
    | Ordered;
  type stock = list(bread);
};
module BakeryLists : {
  type bread = | Baked | Prepared | Ordered;
  type stock;
  let has_in_stock : (bread, stock) => bool;
  let add : (bread, stock) => stock;
} = {
  type bread = | Baked | Prepared | Ordered;
  type stock = list(bread);
  let has_in_stock = List.mem;
  let add = List.cons;
};