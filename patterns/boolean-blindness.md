# Boolean Blindness

It is tempting to use `bool` to represent a choice between different things.
Unfortunately

* `true` or `false` have no semantic meaning whatsoever, in any particular
  context, unless the context explicitly assigns meaning to them.

* `bool` has only two values, and if our choices increase we can't add more
  values to it.

* To add a third option, I need a second boolean value, and the amount of
  possible states increases in powers of 2.

Fortunately we can make use of both Variants and Polymorphic Variants to give
meaning to our programs.

So whenever I see this:

```reason
let can_request = true;
let expect_deleted_files = false;

if (can_request && expect_deleted_files) {
  /** do something */
};
```

I feel I should rewrite it to this:

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
