module UserWithView: {
  type t;
  type view = {
    name: string,
    age: int,
  };
  let make: (~name: string, ~age: int) => t;
  let view: t => view;
} = {
  type t = {
    name: string,
    age: int,
  };
  type view = {
    name: string,
    age: int,
  };
  let make = (~name, ~age): t => {name, age};
  let view: t => view = t => {name: t.name, age: t.age};
};

module PrivateUser: {
  type t =
    pri {
      name: string,
      age: int,
    };
  let make: (~name: string, ~age: int) => t;
} = {
  type t = {
    name: string,
    age: int,
  };
  let make = (~name, ~age): t => {name, age};
};

let _ = PrivateUser.make(~name="me", ~age=30);
/**  Cannot create values of the private type PrivateUser.t */
// let _ = {PrivateUser.name: "me", age: 30};