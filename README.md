# Reason Design Patterns

An unofficial collection of "Design Patterns" collected from learning,
experimenting, and working with Reason and OCaml.

I'll begin by saying that these "patterns" have worked well for me, but, since
experience is subjective, YMMV. They are not infallible, they are 100% not
something I invented (but merely rediscovered), and if you have any input
please send PRs!

#### Patterns
1. [Interface First](#interface-first)
2. ["Pretend" Modules](#pretend-modules)
3. [1 Module for 1 Thing](#1-module-for-1-thing)
4. [Interpreter Pattern](#interpreter-pattern)
4. [Polyvariant Error Handling](#polyvariant-error-handling)


## Interface First

Sketch your module by writing a small interface file that includes only the
minimum things you need the module users to know.

This is tedious, yes. But it will lead to better thought through modules that
expose only the things that are needed.

For example, if you are building software for a bakery, you will need to keep 
track of stock for something like loafs of bread. You could expose it as:

```reason
module Bakery = {
  type bread = | Baked | Prepared | Ordered;
  type stock = list(bread);
};
```

But now whoever consumes `Bakery.stock` can rely on it being a list of `bread`.
Which is perfectly fine if you run a small bakery that doesn't need superb
performance or has a large inventory!

Now take a look at what happens when limit this interface:

```reason
module Bakery : {
  type bread = | Baked | Prepared | Ordered;
  type stock;
} = {
  type bread = | Baked | Prepared | Ordered;
  type stock = list(bread);
};
```

Sure there's a little more duplication (the `type bread` redefinition), but now
as a consumer of `Bakery.stock` I have no clue what `stock` is under the hood.
I simply _can't know_.

This means that if the bakery starts running faster and needs to switch the
implementation from a list to a `Hashtbl` or perhaps just an `Array`, it's
completely possible to do so without hurting the users.

This means that you'll need a few more functions, but they are usually cheap to
write:

```reason
module Bakery : {
  type bread = | Baked | Prepared | Ordered;
  type stock;
  let has_in_stock : (bread, stock) => bool;
  let add : (bread, stock) => stock;
} = {
  type bread = | Baked | Prepared | Ordered;
  type stock = list(bread);
  let has_in_stock = List.mem;
  let add = List.cons;
};
```

Notice how operations also become more semantic all of a sudden! `has_in_stock`
is much more meaningful for your module users than `List.mem`.

Switching the implementation from a `List` to a `Hashtbl` is then a very
self-contained change:

```reason
module Bakery : {
  type bread = | Baked | Prepared | Ordered;
  type stock;
  let has_in_stock : (bread, stock) => bool;
  let add : (bread, stock) => stock;
} = {
  module Stock = Hashtbl.Make({
    type t = string;
    let equal = String.equal;
    let hash = Hashtbl.hash;
  });
  type bread = | Baked | Prepared | Ordered;
  type stock = Stock.t(bread);
  let has_in_stock = (bread, stock) => Stock.mem(stock, bread);
  let add = (bread, stock) => Stock.add(stock, bread);
};
```

And you can tell that the interface to the module never changed. This is why I 
find that starting from an interface made my modules better.




## "Pretend" Modules

Sometimes it's useful to pretend your module already exists, and just use it
first to find ergonomic APIs that closely match your domain problem.

For example, if we're building a car dealership app we will need to keep track
of daily appointments that people have made to test-drive some of your cars.
I've found that sketching out what a nice API for this could look like made my
APIs a lot better.

Whenever I reached for good ol' records, unions and lists, to just start
something out, I'd end up with a lot of code that just deals with lists and
records issues instead of dealing with my domain problems directly. Let's see:

```reason
type car_status =
  | New
  | Used(int)
  | Sold;

type car = {
  name: string,
  model: string,
  status: car_status,
};

type appoinment = {
  date: int, /* unix timestamp */
  car,
};

type day =
  | Mon
  | Tues
  | Wed
  | Thur
  | Fri
  | Sat
  | Sun;

type schedule = list((day, list(appointment)));

let car_volvo = {name: "Volvo", model: "XC90", status: New};

let s = [
  (
    Mon,
    [
      {date: 1547475544, car: car_volvo},
      {date: 1547476154, car: car_volvo},
    ],
  ),
];
```

I've found that I end up working a lot more around the quirks of lists and
records than around my domain problem. Silly little things like local module
opens for records to find their attributes end up getting in the way and
sometimes make for strange error messages.

I've done a better job at exploring problem domains by trying to just use a
pretend module:

```reason
/* NOTE: These modules below do not exist yet */

let schedule = Schedule.make();

let car_volvo = Car.make(~name="Volvo", ~model="XC90", ~status=`New);

let a1 = Appointment.on_date(~date=`Unix(1547475544), ~car=car_volvo);
let a2 = Appointment.on_date(~date=`Unix(1547476154), ~car=car_volvo);

let schedule' = schedule |> Schedule.book(a1) |> Schedule.book(a2);
```

And afterwards filling in the definitions that make them work.

To me, this has 2 clear advantages:

1. **Focuses on DX** — how closely does the API/DSL match the way I think about
   the problem domain? _Cons_ing appointments to a schedule is not how we speak
   about them, we rather _add_ appointments to a Schedule, or _schedule_ them,
   or _book_ them. Sounds like a small thing but when you have to talk to the
   product/business side you'll be using the same language.

1. **Keep Your Types to Yourself** — notice how we don't need to know a lot
   about the types being used to make these modules work. I don't know if an
   `Appointment` is a record, a variant, or an indexed array of marshalled
   values. I also don't care. Same goes for the Schedule or even the Car. I do
   need to know about `Unix` dates and about `New` cars but those can be made
   to be very close to the domain I work with.
  
IMO this approach works very well in combination with the [Interface
First](#interface-first) approach to implementing these modules.

> Rule of Thumb: do you see any data types that do not belong to your domain?





## 1 Module for 1 Thing

Normally it's a good idea to write a module that deals with _one thing only_,
to the point that if there is one type in that module, then it should be
obvious what that type represents.

For example, a module that represents a player in a game could look like this:

```reason
module Player = {
  type inventory = list(string);
  type player = {
    name: string,
    inventory,
  };
  type life =
    | Alive(int)
    | Dead;
  type stats = {
    player,
    life,
  };
};
```

But it's not obvious that the core of the module is the `stats` type, that
includes both a player with an inventory, and some life points.

> Rule of Thumb: if you can name your type `t` without making any less clear
> what the type represents, your module might be dealing with a single thing.

If we refactor this into a few modules, where for each there is a single _core_
type called `t`, we would end up with something like:

```reason
module Inventory = {
  type t = list(string);
};
module Player = {
  type t = {
    name: string,
    inventory: Inventory.t,
  };
};
module Life = {
  type t =
    | Alive(int)
    | Dead;
};
module Stats = {
  type t = {
    player: Player.t,
    life: Life.t,
  };
};
```

Now it's obvious what each module deals with and what should we expect to find
in each one of them.

Another example from the standarad library are the `Hashtbl` and `Queue`
modules:

```reason
module Queue : {
  type t('a);
  /* ... */
};

module Hashtbl : {
  type key;
  type t('a);
  /* ... */
};
```

Where the type that represents a `Queue` or a `Hashtbl` is called `t`.

This convention is a folklore from OCaml that I believe we should adopt in
Reason too: it guides our modules to focus on one datatype at a time.

IMO this approach works very well in combination with the [Interface
First](#interface-first) approach to implementing these modules, since it makes
them very loosely coupled by default, and thus possibly more reusable.




## Interpreter Pattern

In essence, if your types have absolutely no idea how they will be executed,
then you sort of have a little description of your program.

For example, when I was writing [HttpKit](https://github.com/ostera/httpkit) I
found very useful to think of a Server as a concrete piece of data, a value really,
rather than something that _was being executed_.

Sort of how you build up a list of TODOs. You're not really doing them, just
writing down what should be done.

Then you expose that datatype with a nice API that lets someone else choose how
to execute it.

Alright, lets do this with a TODO list.

```reason
module Todo_list: {
  type t; /* we don't want people knowing how we save todos */
  let as_list: t => list(string);
} = {
  type t = Queue.t(string);
  let as_list = t => Queue.fold((acc, e) => [e, ...acc], [], t) |> List.rev;
};

module Todo_list_printf = {
  let print: Todo_list.t => unit =
    todos =>
      todos
      |> Todo_list.as_list
      |> List.iter(todo => Printf.printf("TODO: %s", todo));
};
```

Note that `Todo_list_printf` could be implemented in a completely separate
package, or by a completely different person altogether. They do not need to know
what `t` really is, but they can still decide how to print each ToDo.

This of course works for any other operation: saving data to disk, running web
servers, fetching requests, dealing with low level http/s operations, unix
sockets, running form validations, and pretty much anything that you can
_describe_ without _executing_.




## Polyvariant Error Handling

Something I've struggled with when first starting to work with Reason was how to
deal with errors from different levels of my application appropriately.

I personally dislike Exceptions, so I tend to use the `result` type to represent
when something went well, and when something went wrong.

> Note: If you're using BuckleScript, your `result` type is in `Belt.Result`,
> if you're doing Native Reason, then your `result` type should be available on
> the `result` compatibility package or on the standard library of OCaml for
> OCaml versions above 4.02.3.

Unfortunately, sometimes we end up swallowing some of the errors because the 
error types from deeper inside our function can't be surfaced cleanly:

```reason
module DB = {
  type errors =
    | Db_connection_error
    | Invalid_query;
  let query: string => result(string, errors) =
    q =>
      switch (q) {
      | "" => Error(Db_connection_error)
      | "!" => Error(Invalid_query)
      | q => Ok(q)
      };
};

module User = {
  type t = string;
  type errors =
    | Username_can't_be_empty
    | Database_error(DB.errors);
  let get: string => result(t, errors) =
    name =>
      if (name == "") {
        Error(Username_can't_be_empty);
      } else {
        switch (DB.query(name)) {
        | Error(err) => Error(Database_error(err))
        | Ok(result) => Ok(result)
        };
      };
};

/**
  This does not reflect that the `User.get` call can fail due to database
  errors and thus we can't handle them.
*/
switch(User.get("hello")) {
| Ok(user) => /** do something with user */
| Error(Username_can't_be_empty)
| Error(Database_error(db_err)) => /** DB error manually surfaced :( */
}
```

You may be able to tell that if `DB.query` depended on another function that
returned a result as well, then one of it's variants would include other errors
inside. Nested errors.

An alternative approach I've seen around and have used successfully, is to use
_polymorphic variants_ instead to model errors. The premise is simple, if you
had a variant for your error, you backtick it into a polymorphic variant, and
you let the type-system figure out the rest.

Okay, not quite. But let's refactor the example above to see where it takes us:

```reason
module DB = {
  /** Here I'm using _ to let the type system fill in this bit */
  let query: string => result(string, _) =
    q =>
      switch (q) {
      | "" => Error(`Db_connection_error)
      | "!" => Error(`Invalid_query)
      | q => Ok(q)
      };
};

module User = {
  type t = string;
  let get: string => result(t, _) =
    name =>
      if (name == "") {
        Error(`Username_can't_be_empty);
      } else {
        DB.query(name);
      };
};

/**
  Here we see that the error type is being expanded by all the nested error
  polymorphic variants, which means we can now handle all the possible error
  cases exhaustively!
*/
switch (User.get("hello")) {
| Ok(user) => /** do something with user */ ()
| Error(`Username_can't_be_empty)
| Error(`Invalid_query)
| Error(`Db_connection_error) => /** DB error didn't surface :( */ ()
};
```

This isn't always desirable, but knowing that it's possible and how to do it
gives us more power to design APIs that are safer and better to work with.

It is worth noting that polymorphic variants have plenty more power than regular
variants, but those do not come for free. There's some costs to be considered,
in particular when it comes to internal memory representation.
