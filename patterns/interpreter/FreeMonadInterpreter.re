/**
  Sample Free Monad module.

  A more elegant approach to defining this can be seen in:

  https://www.cl.cam.ac.uk/~jdy22/papers/lightweight-higher-kinded-polymorphism.pdf
*/
module FreeM = {
  module type FUNCTOR = {
    type t('a);
    let map: ('a => 'b, t('a)) => t('b);
  };

  module type FREE = {
    type f('a);
    type t('a) =
      | Free(f(t('a)))
      | Return('a);

    let return: 'a => t('a);
    let (>>=): (t('a), 'a => t('b)) => t('b);

    let lift: f('a) => t('a);
  };

  module Make = (M: FUNCTOR) : (FREE with type f('a) = M.t('a)) => {
    type f('a) = M.t('a);
    type t('a) =
      | Free(f(t('a)))
      | Return('a);

    let return = x => Return(x);
    let rec (>>=) = (x, f) => {
      switch (x) {
      | Free(y) => Free(M.map(z => z >>= f, y))
      | Return(y) => f(y)
      };
    };

    let lift = x => Free(M.map(return, x));
  };
};

/**
  Our Storage DSL
*/
module Storage = {
  type t('next) =
    | Get(string, string => 'next)
    | Put(string, string, 'next)
    | End;

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

/**
  A printer module that given a Storage program will print out the commands
  that will be carried out.
*/
module Storage_printer = {
  open Storage;

  let rec run: Storage.Free.t('a) => unit =
    fun
    | Free(Get(key, k)) => {
        Format.print_string(Format.asprintf("Get(%s)", key));
        Format.force_newline();
        "value" |> k |> run;
      }
    | Free(Put(key, value, k)) => {
        Format.print_string(Format.asprintf("Put(%s, %s)", key, value));
        Format.force_newline();
        run(k);
      }
    | Free(End)
    | Return(_) => ();
};

/**
  An in-memory version of the storage, ideal for testing purposes.
*/
module In_memory_storage: {let run: Storage.Free.t('a) => unit;} = {
  open Storage;

  module DB =
    Hashtbl.Make({
      type t = string;
      let equal = String.equal;
      let hash = Hashtbl.hash;
    });

  let db: DB.t(string) = DB.create(1024);

  let print = () =>
    DB.fold((k, v, acc) => [k ++ " = " ++ v, ...acc], db, [])
    |> List.iter(s => Printf.printf("%s\n", s));

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

/**
  A sample program
*/
module Test = {
  open Storage;
  open Storage.Free;

  let _ = {
    let program =
      put("some key", "some value!")
      >>= (_ => get("some key"))
      >>= (value => put(value, "another value!"))
      >>= (_ => get("some value!"));

    program |> Storage_printer.run;
    program |> In_memory_storage.run;
    In_memory_storage.print();
  };
};
