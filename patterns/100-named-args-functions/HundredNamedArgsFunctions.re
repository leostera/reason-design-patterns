module DateTime = Js.Date;
module Item = {
  type t = {
    name: string,
    price: string,
  };
};
module User = {
  type t = {id: int};
};

module Cart: {
  type t;
  let make: (~items: list(Item.t)) => t;
} = {
  type t = list(Item.t);
  let make = (~items): t => {
    items;
  };
};
module Email_preference = {
  type t = bool;
};
module Tracer = {
  type t;
};
module Purchase_order: {
  type t;
  let make: (~purchased_at: DateTime.t=?, ~quantity: int, Item.t) => t;
} = {
  type t = {
    purchased_at: option(DateTime.t),
    quantity: int,
    item: Item.t,
  };
  let make = (~purchased_at=?, ~quantity, item) => {
    purchased_at,
    quantity,
    item,
  };
};
module Purchase_history = {
  module Status = {
    type t = bool;
  };
};

/**
 *
 * This optional parameter in final position will, in practice, not be optional.
  Reorder the parameters so that at least one non-optional one is in final position or, if all parameters are optional, insert a final ().

  Explanation: If the final parameter is optional, it'd be unclear whether a function application that omits it should be considered fully applied, or partially applied. Imagine writing `let title = display("hello!")`, only to realize `title` isn't your desired result, but a curried call that takes a final optional argument, e.g. `~showDate`.

  Formal rule: an optional argument is considered intentionally omitted when the 1st positional (i.e. neither labeled nor optional) argument defined after it is passed in.

 */
// As written get above warning. Great docs, ReasonML!
module Purchase_preference: {
  type t;
  let make:
    (
      ~should_email: Email_preference.t=?,
      ~history_status: Purchase_history.Status.t=?,
      unit
    ) =>
    t;
} = {
  type t = {
    should_email: option(Email_preference.t),
    history_status: option(Purchase_history.Status.t),
  };
  let make = (~should_email=?, ~history_status=?, ()) => {
    should_email,
    history_status,
  };
};

module Shopping_session: {
  type t;
  let make: (~user: User.t, ~cart: Cart.t) => t;
} = {
  type t = {
    user: User.t,
    cart: Cart.t,
  };
  let make = (~user: User.t, ~cart: Cart.t) => {
    {user, cart};
  };
};
// TODO: implement purchase_item function
// let purchase_item:
//   (
//     ~trace: Tracer.t=?,
//     ~sess=Shopping_session.t,
//     ~prefs=Purchase_preference.t,
//     Purchase_order.t
//   ) =>
//   Belt.Result.t(_, _);
// ```
// let purchase_item = (~sess: Shopping_session.t, ~prefs: Purchase_preference.t) => {
//   sess,
//   prefs,
// } /*   result(_, _)*/;
