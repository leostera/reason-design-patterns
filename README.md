# Reason Design Patterns

An unofficial collection of "Design Patterns" collected from learning,
experimenting, and working with Reason and OCaml.

I'll begin by saying that this "patterns" have worked well for me, but, since
experience is subjective, YMMV. They are not infallible, they are 100% not
something I invented but merely rediscovered, and if you have better ones
please PR them :)

#### Patterns
1. [Interface First](#interface-first)
2. ["Pretend" Modules](#pretend-modules)
3. [1 Module for 1 Thing](#1-module-for-1-thing)
4. [Interpreter Pattern](#interpreter-pattern)
4. [Polyvariant Error Handling](#polyvariant-error-handling)


## Interface First

Sketch your module by writing a small interface file that includes only the
minimum things you need our users to know.

This is tedious, yes. But it will lead to better thought modules that expose
only the things that are needed. For example, if your application needs to
register certain thing, let's say a loaf of bread, you could expose it as:

```reason
module Bakery = {
  type bread = | Baked | Prepared | Ordered;
  type stock = list(bread);
};
```

But now whoever consumes `Bakery.stock` can rely on it being a list of `bread`.
Which is perfectly fine if you run a small bakery that doesn't need superb
performance or has a big stock!

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

Sure there's a little more duplication, but now as a consumer of `Bakery.stock`
I have no clue what it is under the hood. I simply _can't know_. This means that
if the bakery starts running faster and needs to switch the implementation from
a list to a `Hashtbl` or perhaps just an `Array`, it's completely possible to
do so without hurting the users.

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

Sometimes it's useful to pretend your module is already existing, and just use
it first to find good ergonomics for it.

For example, if we're building a car dealership app we will need to keep track
of daily appointments that people have made to test-drive some of our cars. I've
found that sketching out what a nice API for this could look like made my APIs
a lot better.

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

I've found that I end up working a lot more around the quirks of lists and records
than around my domain problem. Silly little things like local module opens for
records to find their attributes end up getting in the way and sometimes make for 
strange error messages.

I've done a better job at exploring by trying to just use a pretend module:

```reason
/* NOTE: This modules below do not exist yet */

let schedule = Schedule.make();

let car_volvo = Car.make(~name="Volvo", ~model="XC90", ~status=`New);

let a1 = Appointment.on_date(~date=`Unix(1547475544), ~car=car_volvo);
let a2 = Appointment.on_date(~date=`Unix(1547476154), ~car=car_volvo);

let schedule' = schedule |> Schedule.book(a1) |> Schedule.book(a2);
```

To me, this has 2 clear advantages:

1. **Focuses on DX** — how closely does the API/DSL match the way I think about
   the problem domain? _Cons_ing appointments to a schedule is not how we speak
   about it, we rather _add_ appointments to a Schedule. (Or _schedule_ them,
   or _book_ them). Sounds like a small thing but when you have to talk to the
   product/business side you'll be using the same language.

1. **Keep Your Types to Yourself** — notice how we don't need to know a lot
   about the types being used to make these modules work. I don't know if an
   `Appointment` is a record, a variant, or an indexed array of marshalled
   values. I also don't care. Same goes for the Schedule or even the Car.
  
IMO this approach works very well with the [Interface First](#interface-first)
approach to implementing these modules.

> Rule of Thumb: do you see any data types that do not belong to your domain?



## 1 Module for 1 Thing

Normally it's a good idea to write a module that deals with _one thing only_.
To the point that if there is one type in that module, then it should be
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

> Rule of Thumb: if you can name your type `t` without making it confusing what
> the type represents, your module should deal with a single thing.

If we refactor this into a few modules, where there is a single _core_ type called `t`, we would end up with something like:

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

Another example from the standarad library are the `Hashtbl` and `Queue` modules:

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
