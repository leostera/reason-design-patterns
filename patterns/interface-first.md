# Interface First

#### example [`InterfaceFirst.re`](./interface-first/InterfaceFirst.re)

Sketch your module by writing a small interface file that includes only the
minimum things you need the module users to know.

This is tedious, yes. But it will lead to better thought through modules that
expose only the things that are needed.

For example, if you are building software for a bakery, you will need to keep 
track of stock for something like loafs of bread. You could expose it as:

```reason
module Bakery = {
  type bread = | Baked | Prepared | Ordered;
  type stock = list(bread);
};
```

But now whoever consumes `Bakery.stock` can rely on it being a list of `bread`.
Which is perfectly fine if you run a small bakery that doesn't need superb
performance or has a large inventory!

Now take a look at what happens when limit this interface:

```reason
module Bakery : {
  type bread = | Baked | Prepared | Ordered;
  type stock;
} = {
  type bread = | Baked | Prepared | Ordered;
  type stock = list(bread);
};
```

Sure there's a little more duplication (the `type bread` redefinition), but now
as a consumer of `Bakery.stock` I have no clue what `stock` is under the hood.
I simply _can't know_.

This means that if the bakery starts running faster and needs to switch the
implementation from a list to a `Hashtbl` or perhaps just an `Array`, it's
completely possible to do so without hurting the users.

This means that you'll need a few more functions, but they are usually cheap to
write:

```reason
module Bakery : {
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
```

Notice how operations also become more semantic all of a sudden! `has_in_stock`
is much more meaningful for your module users than `List.mem`.

Switching the implementation from a `List` to a `Hashtbl` is then a very
self-contained change:

```reason
module Bakery : {
  type bread = | Baked | Prepared | Ordered;
  type stock;
  let has_in_stock : (bread, stock) => bool;
  let add : (bread, stock) => stock;
} = {
  module Stock = Hashtbl.Make({
    type t = string;
    let equal = String.equal;
    let hash = Hashtbl.hash;
  });
  type bread = | Baked | Prepared | Ordered;
  type stock = Stock.t(bread);
  let has_in_stock = (bread, stock) => Stock.mem(stock, bread);
  let add = (bread, stock) => Stock.add(stock, bread);
};
```

And you can tell that the interface to the module never changed. This is why I 
find that starting from an interface made my modules better.

## Simple Top-Down Development

This technique is called _top-down development_ and is especially suited to OCaml/Reason because you can _design_ the interface first according to the ideal, semantic way you'd like to use it, then 'fill in the blanks' with an implementation. In fact, to keep the compiler happy while you develop the interface, you can mock up a _dummy implementation_ that will type-check but fail at runtime. The technique is covered in more detail in this blog post (OCaml syntax): https://blog.janestreet.com/simple-top-down-development-in-ocaml/
