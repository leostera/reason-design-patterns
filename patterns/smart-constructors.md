# Smart Constructors

The idea behing Smart Constructors is that we can only have a value that passes
a certain criteria that can not be enforced by the type-system, and thus the
need for repeatedly checking the value is gone.

To be useful, Smart Constructors normally require a type to be _abstract_. That
is, other modules can't construct it or deconstruct it directly. This forces
you to use the smart constructor to create values.

For example, let's say we need to represent email addresses in our program. One
option would be to simply make an alias for a string:

```reason
module Email : {
  type t = string;
};
```

Then any string could be an `Email.t`. And we'd probably want to have a way of
checking if it's a valid email address or not:

```reason
module Email : {
  type t = string
  let is_valid : t => bool;
};
```

A problem with this is that the ability to create an `Email.t` is unrestricted
— anyone can create one even if it's not valid, and having a function like
`Email.is_valid` does little to help us use the values correctly. This is a
symptom of [_Boolean Blindness_](boolean-blindness.md).

```reason
let me : Email.t = "reasonsthlm" /** not a valid email address! */
me |> Email.is_valid == false;   /** must check manually before usage */
```

Now, something interesting happens when we make the type `Email.t` abstract:
you can no longer create a value of that type arbitrarily. You can only create
them within the implementation of the Email module.

```reason
module Email : {
  type t;
  let is_valid : t => bool;
};

let me : Email.t = "reasonsthlm" /** compile-time error! */
```

Which means we need to provide another way of creating these values. Normally,
that way is a function. Because we have a function, we can decide through
arbitrary logic whether the value meets our desired criteria: is a valid email
address.

```reason
module Email : {
  type t;
  let make: string => option(t);
};

let me : option(Email.t) = Email.make("reasonsthlm") /** this should be None */
```

Which also gets rid of the `Email.is_valid` function! Now we know that if there
is a value of type `Email.t`, it's a valid email address.

Getting the data out of this email address is normally done as well through a
separate function, called a _viewer_. In this case, an `Email.to_string`
function makes sense to get the email as a plain string value. For more complex
abstract types, the [constructor+view](constructor-view.md) pattern is helpful.

```reason
module Email : {
  type t;
  let make: string => option(t);
  let to_string : t => string;
};

let me : option(Email.t) = Email.make("leandro@reasonsthlm.se")

switch(me) {
| Some(email) => Email.to_string(email) == "leandro@reasonsthlm.se"
}
```
