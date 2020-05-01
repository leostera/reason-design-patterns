## Private Types

#### example [`PrivateTypes.re`](./private_types/PrivateTypes.re)

From this [issue](https://github.com/ostera/reason-design-patterns/issues/5#issue-416302595).

> More of a question than an issue, but wasn't sure where else to ask.

> Don't private types achieve the semantics you want? An exposed type which cannot be constructed but can be deconstructed. Are you already aware of this and there's some additional value to the view pattern that I don't understand?

> Here's some code which demonstrates what I mean: https://reasonml.github.io/en/try.html?rrjsx=true&reason=LYewJgrgNgpgBAVQM4wE4HUCWAXAFgNUxgHcAuOAbwCg45sBPAB3mwG4a6n4A3I4uALyUOtAHYBDYDHJJsqTKIDmAGhFxxi6XAXZVtAL7tasbHGDiA1loAUAPwlSZchSri2NWnQEpBAPjpGcCZwvCTkpgL+ocTs+oLCtAzMdPHUtGKSWrLySnrpHuQ6eYYcSTx8qWoOWc65agXaorocJcYwpuZW8XbVym4eXuF+lL3qmq1B7SF8Q5HTJPER-hTV4QB0ow3Yax4lJVSgkLBwAAry3OLYMMho5GmcyRFqjPIJ6RmOcNkuee8NRWoJsFOjZ7JknDlXO5NIUmj45mwqHEhPcyikUVVwV9aip6jDGs0DIFgZZ4EIepk+tCYIN0ctNuNYuwqMEAPrxM6YC5XG6oNYgilSAQAIikwqpHgEAGYAAxedhs1Kc7nXFB81ZwUUwcVjLSywxAA

```re
module UserWithView: {
  type t;
  type view = {
    name: string,
    age: int,
  };
  let make: (~name: string, ~age: int) => t;
  let view: t => view;
} = {
  type t = {
    name: string,
    age: int,
  };
  type view = {
    name: string,
    age: int,
  };
  let make = (~name, ~age): t => {name, age};
  let view: t => view = t => {name: t.name, age: t.age};
};

module PrivateUser: {
  type t =
    pri {
      name: string,
      age: int,
    };
  let make: (~name: string, ~age: int) => t;
} = {
  type t = {
    name: string,
    age: int,
  };
  let make = (~name, ~age): t => {name, age};
};

let _ = PrivateUser.make(~name="me", ~age=30);
let _ = {PrivateUser.name: "me", age: 30};
```

