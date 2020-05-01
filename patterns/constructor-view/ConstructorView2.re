/**
  This is inconvenient if we need to match on multiple values within this type
  at the same time. Ideally, we would be able to just do a `switch` and
  pattern-match like if we had access to some record. Let's see what that would
  look like with a record type we have direct access to in `ContructorView3`:
 */

module User: {
  type t;
  let make: (~name: string, ~age: int) => option(t);
  let name: t => string;
  let age: t => int;
} = {
  type t = {
    name: string,
    age: int,
  };

  let make = (~name, ~age) => Some({name, age});
  let name = user => user.name;
  let age = user => user.age;
};

let me: option(User.t) = User.make(~name="leandro", ~age=27);

switch (me) {
| Some(user) => User.name(user) == "leandro"
| None => false
};

let checkMe =
  switch (me) {
  | Some(user) => User.name(user) == "leandro"
  | None => false
  };

Js.log(checkMe);