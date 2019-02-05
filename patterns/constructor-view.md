# Constructor + View

When working with types, sometimes it's best to keep them abstract to prevent
your users from knowing too much about them.

Making them abstract means that they can't create them directly, and will be
forced to use a constructor function. That construction function may as well be
a [Smart Constructor](smart-constructors.md) (if you need to make some special
checks).

```reason
module User : {
  type t;
  let make : (~name:string, ~age:int) => option(t);
};
```

One problem with this is that because you can't construct `User.t` directly, you
also can't deconstruct it!

So building one means you have no way of recovering the `name` or `age` you used
when calling `User.make`:

```reason
let me: option(User.t) = User.make(~name="leandro", ~age=27);

switch(me) {
| Some(user) => me.name /** nope! this doesn't work */
| None => /** no user here! */
};
```

To solve this we could make a function to extract the name, and another for the
age:

```reason
module User : {
  type t;
  let make : (~name:string, ~age:int) => option(t);
  let name : t => string;
  let age : t => int;
};

let me: option(User.t) = User.make(~name="leandro", ~age=27);

switch(me) {
| Some(user) => User.name(user) == "leandro"
| None => /** no user here! */
};
```

But this is inconvenient if we need to match on multiple values within this type
at the same time. Ideally, we would be able to just do a `switch` and
pattern-match like if we had access to some record. Let's see what that would
look like with a record type we have direct access to:

```reason
type user_that_works_for_pattern_matchin = {
  name: string,
  age: int
};

let me = { name: "leandro", age: 27 };

switch(me) {
| { name: "leandro", _ } => /** it's me! */
| { age: 99, _ } => /** it's a 99 year old user! */
| { age: 12, name: "peter pan" } => /** it's peter pan! */
};
```

Okay, that looks alright! But we still want to have our main type as abstract,
which means we can't just use a public record. What we can do instead is have a
separate type for _viewing_ this data.

```reason
module User : {
  type t;
  type view = { name: string, age: int };
  let make: (~name:string, ~age:int) => option(t);
  let view: t => view;
};
```

Now we have an internal type `t`, and an external type `view` that can be used to
pattern match. This gives us flexibility to implement `t` very efficiently and
still have the developer experience of pattern matching at the cost of a single
function call:

```reason
let me: option(User.t) = User.make(~name="leandro", ~age=27);

switch(me) {
| Some(user) =>
  /** here the user is good! now we can destruct it */
  switch(user |> User.view) {
  | { name: "leandro", _ } => /** it's me! */
  | { age: 99, _ } => /** it's a 99 year old user! */
  | { age: 12, name: "peter pan" } => /** it's peter pan! */
  }
| None => /** no user here! */
};
```

#### Additional References

For a `ppx` that works with these ideas, have a look at
[ppx_view](https://github.com/ocaml-ppx/ppx_view).

