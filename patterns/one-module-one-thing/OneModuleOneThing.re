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
