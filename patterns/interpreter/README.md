# Interpreter Pattern

In essence, build your programs as values that describe what should be done, and
let someone else define how to execute those descriptions.

It's a very useful pattern to layer the design of your libraries and
applications.

For example, when I was writing [HttpKit](https://github.com/ostera/httpkit) I
found very useful to think of a Server as a concrete piece of data, a value
really, rather than something that _was being executed_.

Other libraries such as [lwt](https://github.com/ocsigen/lwt),
[http/af](https://github.com/inhabitedtype/httpaf),
[cmdliner](https://github.com/dbuenzli/cmdliner), or
[markup.ml](https://github.com/aantron/markup.ml) do this successfully,
allowing you to use them without committing to a particular runtime (such as
`lwt`, or `async`, or whatever else you want to use).

Let's try this with a small TODO List application that uses a `Storage` module
to define the things that can be done to a `Todo` item:

```reason
module Todo : {
  type t;
  let create: (~name: string) => t;
  let empty: unit => t;
};

/** Core module from the ToDo package */
module Storage : {
  type t =
    | Create(Todo.t)
    | Complete(Todo.ID.t)
    | Delete(Todo.ID.t)
    | Update(Todo.t, Todo.t)
    | Chain(t, t);

  let create: Todo.t => t;
  let complete: Todo.t => t;
  let delete: Todo.t => t;
  let update: (Todo.t, ~like: Todo.t) => t;

  /** convenience operator to sequence storage actions */
  let (>>): (t, Todo.t => t) => t;
};
```

Now that we have the module there, it's inviting the assumption that calling
`Storage.create` will not create a new thing at all anywhere. In fact, it
really just returns a value of type `Storage.t` that _describes_ what should be
done.

I've also included a small operator to allow us to chain storage operations
threading the same `Todo.t` in all of them.

```reason
let todo = Todo.empty();

let create_and_complete = Storage.(todo |> create >> complete);

let then_update_and_delete =
  Storage.(
    create_and_complete
    >> update(~like=Todo.create(~name="updated name"))
    >> delete
  );
```

The values we created above merely describe what should happen, but don't
actually execute anything.

Now we can implement a module that will actually take this program description
and interpret it:

```reason
module Async_storage : {
  /* This defines how the storage will actually be executed! */
  let run: Storage.t => Lwt_result.t(unit, error);
};
```

Voila. `Async_storage` knows how to take a description of storage operations (a
`Storage.t`) and returns a promise that it will be executed.

A complete program using both of these modules would look like this:

```reason
let todo = Todo.empty();
let todo' = Todo.create(~name="Interpret this!");

Storage.(
  todo
  |> create
  >> update(~like=todo')
  >> complete
  >> delete      /** up to here we're just creating the description */
  |> Async_storage.run  /** then we interpret it! */
);
```

Note that `Async_storage` could be implemented in a completely separate
package, or by a completely different person altogether.

This pattern seems to be a _Good Way_ of layering your modules since it allows
you to swap the underlying implementations without needing to change any of the
semantics of higher layers.

If you're interested in more general abstractions that solves the same problem,
read on.

## Composable DSLs with Free Monads

While the DSL described above is perfectly usable, it requires a little bit of
work to get better ergonomics. For example, our `Chain` constructor doesn't
really have any semantic meaning and it only serves the purpose of _chaining_
actions together; the same goes for our convenience `(>>)` operator.

```reason
todo
|> create
>> update(~like=todo') /** <-- our custom operator */
>> complete
>> delete
```

Fortunately, there's an approach that takes care of generating all that
boilerplate for us in a quite elegant way, and gives us a sequencing operator
that is not unique to our program but instead has very specific semantics and is
very well documented: `>>=`. This approach involves using *Free Monads*.

> NOTE: For a good read on Free Monads with examples in Haskell have a look at
> [this StackExchange
> Answer](https://softwareengineering.stackexchange.com/questions/242795/what-is-the-free-monad-interpreter-pattern).

*Free Monads* are a construct that helps us build _composable_ DSLs by giving us
a composable structure that we can use to put in whatever we want. You define a
type that describes everything you should be able to do, and the Free Monad
makes it composable.

For the example above, our type could look like:

```reason
module Storage = {
  type t('next) =
    | Get(string, string => 'next)
    | Put(string, string, 'next)
    | End;
};
```

Where we can see that each action is parametrized by the next action that will
occur. Be careful when designing your constructors! That next action could be
nothing at all if `'next` is `unit` or it could be another action.

Giving this, we need to define our type as a _categorical functor_. That is, as
something that can be mapped over with a function. We do this to allow the
continuations to be composed with other functions, which allows us to execute
arbitrary code for each of the steps.

A complete version of our `Storage` module that defines our core type as a
functor, as well as an instance of the `Free` monad for our core type, and
functions to create these values more conveniently, is shown below:

```reason
module Storage = {
  /* ... */
  let map = f =>
    fun
    | Get(name, k) => Get(name, x => x |> k |> f)
    | Put(name, value, next) => Put(name, value, f(next))
    | End => End;

  module Free: FreeM.FREE with type f('a) = t('a) =
    FreeM.Make({
      type f('a) = t('a);
      let map = map;
    });

  let get = k => Free.lift(Get(k, x => x));
  let put = (k, v) => Free.lift(Put(k, v, ()));
  let end_ = () => Free.lift(End);
};
```

With this new module, we can now write expressions of type `Storage.Free.t('a)`
that include arbitrary combinations of steps.

```reason
let program : Storage.Free.t(string) =
  put("some key", "some value!")
  >>= (_ => get("some key"))
  >>= (value => put(value, "another value!"))
  >>= (_ => get("some value!"));
```

And naturally, if we define a function that will take a `Storage.Free.t('a)` we
can recursive into the values and execute all of those commands in the right
sequence.

For example, let's say we will have an in-memory version of our Storage, using a
Hash table as storage.

```reason
module In_memory_runner : {
  let run: Storage.Free.t('a) => unit;
} = {
  open Storage;

  module DB =
    Hashtbl.Make({
      type t = string;
      let equal = String.equal;
      let hash = Hashtbl.hash;
    });

  let db: DB.t(string) = DB.create(1024);

  let rec run: Storage.Free.t('a) => unit =
    fun
    | Free(Get(key, k)) => DB.find(db, key) |> k |> run
    | Free(Put(key, value, k)) => {
        DB.add(db, key, value);
        run(k);
      }
    | Free(End)
    | Return(_) => ();
};
```

This means executing the program we defined above is as easy as calling the
`run` function with the `program` value.

```reason
let result : string = program |> In_memory_runner.run; 
```

And we can define any number of runners that do different things with our
`program`:

* Execute it against production databases
* Execute it in memory for test purposes
* Log it or dump it as a series of commands for inspection and debugging
* Optimize it using custom heuristics

One of the biggest drawbacks of the Free Monad approach to the Interpreter
Pattern is that the structure of the program is not static and rather has to be
discovered by executing the continuations.
