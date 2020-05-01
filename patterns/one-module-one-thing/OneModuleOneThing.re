// # 1 Module for 1 Thing

// Normally it's a good idea to write a module that deals with _one thing only_,
// to the point that if there is one type in that module, then it should be
// obvious what that type represents.

// For example, a module that represents a player in a game could look like this:

// ```reason
module PlayerNoT = {
  type inventory = list(string);
  type player = {
    name: string,
    inventory,
  };
  type life =
    | Alive(int)
    | Dead;
  type stats = {
    player,
    life,
  };
};
// ```

// But it's not obvious that the core of the module is the `stats` type, that
// includes both a player with an inventory, and some life points.

// > Rule of Thumb: if you can name your type `t` without making any less clear
// > what the type represents, your module might be dealing with a single thing.

// If we refactor this into a few modules, where for each there is a single _core_
// type called `t`, we would end up with something like:

// ```reason
module Inventory = {
  type t = list(string);
};
module Player = {
  type t = {
    name: string,
    inventory: Inventory.t,
  };
};
module Life = {
  type t =
    | Alive(int)
    | Dead;
};
module Stats = {
  type t = {
    player: Player.t,
    life: Life.t,
  };
};
// ```

// Now it's obvious what each module deals with and what should we expect to find
// in each one of them.

// Another example from the standarad library are the `Hashtbl` and `Queue`
// modules:

// ```reason
// module Queue : {
//   type t('a);
  /* ... */
// };

// module Hashtbl : {
//   type key;
//   type t('a);
//   /* ... */
// };
// ```

// Where the type that represents a `Queue` or a `Hashtbl` is called `t`.

// This convention is a folklore from OCaml that I believe we should adopt in
// Reason too: it guides our modules to focus on one datatype at a time.