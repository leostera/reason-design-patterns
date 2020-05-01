module User: {
  type t;
  let is_admin: t => bool;
};
module Admin: {
  type t;
  let from_user: User.t => option(t);
};