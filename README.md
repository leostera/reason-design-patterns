# 🗺 Reason Design Patterns

An unofficial collection of "Design Patterns" collected from learning,
experimenting, and working with Reason and OCaml.

These "patterns" have worked well for me, but, since experience is subjective,
YMMV. They are not infallible, they are 100% not something I invented (but
merely rediscovered), and if you have any input please send PRs!

## Project Layout

1. [Yawaramin's Modular Project Structure](https://dev.to/yawaramin/a-modular-ocaml-project-structure-1ikd)

## Domain Modeling

If your code says too much about how things are done, fights records and
variants, or is plagued by empty lists and booleans that mean special things,
take a look at these:

1. [Boolean Blindness](patterns/boolean-blindness.md)
1. ["Pretend" Modules](patterns/pretend-modules.md)

## Module Design

When you are writing modules and libraries, it's foundational to get some
things right. A stable interface that leaks no implementation details, good containment of concerns, and the right amount of flexibility are some of the things to consider.

Find below some patterns to help you write modules:

1. [Interface First](patterns/interface-first.md)
1. ["Pretend" Modules](patterns/pretend-modules.md)
1. [1 Module for 1 Thing](patterns/1-module-1-thing.md)
1. [Interpreter Pattern](patterns/interpreter-pattern.md)
1. [100-Named-Args Functions](patterns/100-named-args-functions.md)

## Error Handling that Scales

1. [PolyVariant Error Propagation](patterns/polyvariant-error-propagation.md)
1. [Rresult's Guide to Custom Error Types](http://erratique.ch/software/rresult/doc/Rresult.html#usage)
