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
  let from_user = (user: User.t) => user.role == Admin ? Some(user) : None;
};
// works with role:User
let user: User.t = {name: "Joe Camel", role: User};
let admin: option(Admin.t) = Admin.from_user(user);
let do_admin_stuff = (admin: Admin.t) =>
  Js.log2("Doing " ++ "admin.name" ++ "Stuff",admin);

let do_user_stuff = (user: User.t) =>
  Js.log("Doing " ++ user.name ++ " `User` Stuff");

switch (admin) {
| Some(admin) => do_admin_stuff(admin)
| None => do_user_stuff(user)
};
// works with role:Admin
let user: User.t = {name: "Joe Camel", role: Admin};
let admin: option(Admin.t) = Admin.from_user(user);
let do_admin_stuff = (admin: Admin.t) =>
  Js.log2("Doing " ++ "admin.name" ++ "Stuff",admin);

let do_user_stuff = (user: User.t) =>
  Js.log("Doing " ++ user.name ++ " `User` Stuff");

switch (admin) {
| Some(admin) => do_admin_stuff(admin)
| None => do_user_stuff(user)
};