type user_that_works_for_pattern_matching = {
  name: string,
  age: int,
};

let me = {name: "leandro", age: 27};

switch (me) {
| {name: "leandro", _} => Js.log2(me.name, "is pattern_matching!")
| {age: 99, _} => Js.log("it's a 99 year old user!")
| {age: 12, name: "peter pan"} => Js.log("it's peter pan!")
| {age: _, name: _} => Js.log("nothing to see here!")
};

module User: {
  type t;
  type view = {
    name: string,
    age: int,
  };
  let make: (~name: string, ~age: int) => option(t);
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
  let make = (~name, ~age): option(t) => Some({name, age});
  let view: t => view = t => {name: t.name, age: t.age};
};

let me: option(User.t) = User.make(~name="leandro", ~age=27);

switch (me) {
| Some(user) => User.view(user).name->Js.log2("it's", _)
| None => Js.log("no user here!")
};