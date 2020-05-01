module User: {
  type t;
  let is_admin: t => bool;
};
module Admin: {
  type t;
  let from_user: User.t => option(t);
};
let do_user_stuff : User.t => unit;
let do_admin_stuff: Admin.t => unit;