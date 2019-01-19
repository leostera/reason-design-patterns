# PolyVariant Error Handling

Something I've struggled with when first starting to work with Reason was how to
deal with errors from different levels of my application appropriately.

I personally dislike Exceptions, so I tend to use the `result` type to represent
when something went well and when something went wrong.

> Note: If you're using BuckleScript, your `result` type is in `Belt.Result`,
> if you're doing Native Reason, then your `result` type should be available on
> the `result` compatibility package or on the standard library of OCaml for
> OCaml versions above 4.02.3.

Unfortunately, sometimes exposing errors from deeper inside our application
requires explicitly threading them through all the layers that use it. This can
be quite tedious and allows for accidentally losing error information.

In the example below, we have a `Database` module that can return two different
errors, both tracked with a variant type, and how we must manually surface them
in the `User` module.

```reason
module Database : {
  type error =
    | Connection_error
    | Invalid_query;
  type t; 
  let query: (string, t) => result(string, error);
};

module User : {
  type error =
    | Username_can't_be_empty
    | Database_error(DB.error);
  type t;
  let get: (Database.t, string) => result(t, errors);
};

switch(User.get("hello")) {
| Ok(user) => /** do something with user */
| Error(User.Username_can't_be_empty) => /** handle empty username */
| Error(User.Database_error(Database.Invalid_query))
| Error(User.Database_error(Database.Connection_error)) =>
  /** Database errors manually surfaced :( */
}
```

Doing this every time we are using the `Database` module means we will have
several `Database_error(Database.error)` variants in different modules.

You may also be able to tell that if `Database.query` depended on another
function that returned a result as well, then one of it's variants would
include other errors inside. Nested errors.

It's clear that there's a duplication that we could get rid of to make our
codebase more maintainable.

An alternative approach I've seen around and have used successfully, is to use
_polymorphic variants_ to propagate errors. The premise is simple, and involves
a single step:

1. Wrap your error type in a polymorphic variant tag that identifies your
   module.

This approach has been described originally in the
[`rresult`](http://erratique.ch/software/rresult/doc/Rresult.html#2_Customerrortypes)
package docs, although it goes into a slightly more involved setup that
guarantees better composition and extensibility. A must read if you're
designing a library!

Let's now refactor the example above to see where it takes us:

```reason
module Database : {
  type error =
    | Connection_error
    | Invalid_query;
  type t;
  let query: (string, t) => result(string, [> | `Database(error)]);
};

module User : {
  type error =
    | Username_can't_be_empty;
  type t;
  let get: (Database.t, string) => result(t, [> | `User(error) ]);
};

switch (User.get("hello")) {
| Ok(user) => /** do something with user */
| Error(`User(User.Username_can't_be_empty)) => /** handle empty username */
| Error(`Database(Database.Invalid_query)) => /** handle invalid query */
| Error(`Database(Database.Connection_error)) => /** handle connection error */
};
```

Here we see that the error type is being expanded by all the nested error
polymorphic variants that are automatically propagated, which means we can now
handle all the possible error cases exhaustively!

This isn't always desirable, but knowing that it's possible and how to do it
gives us more power to design APIs that are safer and better to work with.

It is worth noting that polymorphic variants have plenty more power than regular
variants, but those do not come for free. There's some costs to be considered,
in particular when it comes to internal memory representation.


