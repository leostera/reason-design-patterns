# Interpreter Pattern

In essence, build your programs as values that describe what should be done, and
let someone else define how to execute those descriptions.

A very common example of this in pure functional programs is the IO Monad.

For example, when I was writing [HttpKit](https://github.com/ostera/httpkit) I
found very useful to think of a Server as a concrete piece of data, a value
really, rather than something that _was being executed_.

Other libraries such as [lwt](https://github.com/ocsigen/lwt),
[http/af](https://github.com/inhabitedtype/httpaf),
[cmdliner](https://github.com/dbuenzli/cmdliner), or
[markup.ml](https://github.com/aantron/markup.ml) do this successfully,
allowing you to use them without committing to a particular runtime (such as
`lwt`, or `async`, or whatever else you want to use).

Sort of how you build up a list of TODOs. You're not really doing them, just
writing down what should be done, and sometimes how it should be done.

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
