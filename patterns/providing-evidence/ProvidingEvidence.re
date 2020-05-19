type role =
  | Admin
  | User;

module User = {
  type t = {
    name: string,
    role,
  };

  let is_admin: t => bool = (user: t) => user.role == Admin ? true : false;
};

module Admin: {
  type t;
  let from_user: User.t => option(t);
} = {
  type t = User.t;
  let from_user = user => Some(user);
};

type do_user_stuff = User.t => unit;
type do_admin_stuff = Admin.t => unit;

let user: User.t = {name: "Joe Camel", role: User};
let admin: option(Admin.t) = Admin.from_user(user);
let do_admin_stuff = (admin: Admin.t) =>
  Js.log2("Doing " ++ "admin" ++ "Stuff",admin);

let do_user_stuff = () => Js.log("Doing " ++ "user.name" ++ " Stuff");

switch (admin) {
| Some(admin) => do_admin_stuff(admin)
| None => do_user_stuff()
};