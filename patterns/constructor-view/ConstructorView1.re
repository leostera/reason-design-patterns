
module User: {
  type t;
  let make: (~name: string, ~age: int) => option(t);
} = {
  type t = {
    name: string,
    age: int,
  };

  let make = (~name, ~age) => Some({name, age});
};

let me: option(User.t) = User.make(~name="leandro", ~age=27);

// switch(me) {
// | Some(user) => me.name /** nope! this doesn't work */
// | None => /** no user here! */
// };
