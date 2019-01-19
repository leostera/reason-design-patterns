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

It could be used to make a library that behaves synchronously, to behave
asynchronousyl — and even then you could have several backends for several
concurrency libraries! Lwt, Core.async, or Repromise being some examples of
them.
