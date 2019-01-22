# Boolean Blindness

It is tempting to use `bool` to represent a choice between different things.
Unfortunately

* `true` or `false` have no semantic meaning whatsoever, in any particular
  context, unless the context explicitly assigns meaning to them.

* `bool` has only two values, and if our choices increase we can't add more
  values to it.

* To add a third option, I need a second boolean value, and the amount of
  possible states increases in powers of 2.

Fortunately we have some tools to do this, each fitting different use cases
better:

1. [Giving Names to Values](#giving-names-to-values)
2. [Using Smart Constructors](#using-smart-constructors)

## Giving Names to Values 

We can make use of both Variants and Polymorphic Variants to give meaning to
our programs by more explicitly _naming our values_.

So whenever we see this:

```reason
let can_request = true;
let expect_deleted_files = false;

if (can_request && expect_deleted_files) {
  /** do something */
};
```

It can be written more semantically like this:

```reason
let request_status = `Can_request;
let expectation = `Files_were_deleted;

switch (request_status, expectation) {
| (`Can_request, `Files_were_deleted) => /** do something */
};
```

The advantages being that

* I no longer rely on the name of a variable to understand what the value means.

* If the `request_status` needs more than 2 options, it's trivial to add another
  one and the type-system will help me refactor along to make sure I miss
  nothing.

* Because I have exactly as many options as I need, the surface area for illegal
  states is minimized, and the possible states scale linearly.

In particular I prefer polymorphic variants because they do not need to be
declared beforehand and the type-system will help me unify them as I use them.

You can read more about them here: [Polymorphic Variants, Real World
OCaml](http://dev.realworldocaml.org/variants.html#polymorphic-variants).


## Using Smart Constructors

The idea behing Smart Constructors is that we can only create a value that is
valid and thus the need for checks on it is gone. You can read more about them
in general [here](smart-constructors.md).

Instead of checking if our user is an admin, if we construct an admin from a
user and from then on we can rely on it being an admin user.

Before we would have something like:

```reason
let user : User.t = { name: "Joe Camel", role: "admin" };
user |> User.is_admin == true;
```

We could instead write:

```reason
let user : User.t = { name: "Joe Camel" };
let admin : option(Admin.t) = Admin.from_user(user);
```

Let's dissect that second example. Clearly before we had a function that took
a user and returned a boolean:

```reason
module User : {
  type t;
  let is_admin : t => bool;
};
```

But this means that we are relying on that boolean value all the time to
perform checks on whether the user is or is not an admin. We could go back and
[give these values a name](#giving-names-to-values) but we would still have to
check every time we want to see if a User is an admin or not.

The alternative snippet provides a different function that seems small but has
quite a big implication.

```reason
module Admin : {
  type t;
  let from_user : User.t => option(t);
};
```

If we give `Admin.from_user` a `User.t` value, _maybe_ it returns us an
`Admin.t` value. That is, not all Users are Admins, but if we get an Admin
back, then we do not need to check anymore if that Admin is in fact an Admin.

Now let's expand the first approach to include two function calls, one that's
meant to be for admin users, and another one that's meant to be for regular
users.

```reason
let user : User.t = { name: "Joe Camel", role: "admin" };

if (user |> User.is_admin) {
  do_admin_stuff(user);
} else {
  do_user_stuff(user);
}
```

Where the type of both those functions is `User.t => unit`, we probably will
have to check again inside of `do_admin_stuff` if the user is really an admin.
So the problem here is that the following snippet is perfectly valid:

```reason
if (user |> User.is_admin) {
  do_user_stuff(user);
} else {
  do_admin_stuff(user);
}
```

I guess by now you also agree that this seems unnecessarily dangerous. Let's
refactor both those functions so they are safer to work with:

```reason
let do_user_stuff : User.t => unit;
let do_admin_stuff: Admin.t => unit;
```

Voila. Now we need to get evidence that our user is an admin and we're good to
go!

```reason
let user : User.t = { name: "Joe Camel" };
let admin : option(Admin.t) = Admin.from_user(user);

switch(admin) {
| Some(admin) => do_admin_stuff(admin)
| None => do_user_stuff(user)
};
```

Much more safe to work with, since calling `do_admin_stuff(user)` or
`do_user_stuff(admin)` are both type errors.
