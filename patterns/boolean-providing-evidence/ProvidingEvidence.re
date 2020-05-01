/**  ## Providing Evidence

The idea behing **_providing evidence_** is that if we can only create a value that
is valid, the need for checks on it is gone because having a value at all is
evidence enough that the value is valid. We can achieve this with [Smart
Constructors](smart-constructors.md).

Instead of checking if our user is an admin, if we construct an admin from a
user and from then on we can rely on it being an admin user. That is, having an
admin user is evidence enough that we have a user that is an admin.

Sounds a little redundant, so let's see some code. Before we would have
something like:

module UserSimple ={
    type t = {name:string,role:string};
    type is_admin = bool;
    let is_admin = t => bool;
};
let user : UserSimple.t = { name: "Joe Camel", role: "admin" };
user |> UserSimple.is_admin == true;
*/
/**  ## We could instead write:*/


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