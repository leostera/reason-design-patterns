# 🗺 Reason Design Patterns

An unofficial collection of "Design Patterns" collected from learning,
experimenting, and working with Reason and OCaml.

These "patterns" have worked well for me, but, since experience is subjective,
YMMV. They are not infallible, they are 100% not something I invented (but
merely rediscovered), and if you have any input please send PRs!

If you've found this content useful, considering ordering a copy my book: [Practical ReScript](https://practicalrescript.com)

## Project Layout

1. [Yawaramin's Modular Project
   Structure](https://dev.to/yawaramin/a-modular-ocaml-project-structure-1ikd)

## Working with Types

Whether you are modeling TODO lists or mission-critical stuff, these patterns
should help you build types that are safe to use, refactor, and evolve.

1. [Smart Constructors](patterns/smart-constructors.md), to give your types
   additional refinements that can only be done at runtime
2. [Constructor + View](patterns/constructor-view.md), to keep your types
   abstract without losing pattern-matching

## Domain Modeling

If your code says too much about how things are done, fights records and
variants, or is plagued by empty lists and booleans that mean special things,
take a look at these:

1. [Boolean Blindness](patterns/boolean-blindness.md)
2. ["Pretend" Modules](patterns/pretend-modules.md)

## Module Design

When you are writing modules and libraries, it's foundational to get some
things right. A stable interface that leaks no implementation details, good
containment of concerns, and the right amount of flexibility are some of the
things to consider.

Find below some patterns to help you write modules:

1. [Interface First](patterns/interface-first.md)
2. ["Pretend" Modules](patterns/pretend-modules.md)
3. [1 Module for 1 Thing](patterns/1-module-1-thing.md)
4. [Interpreter Pattern](patterns/interpreter/README.md)
5. [100-Named-Args Functions](patterns/100-named-args-functions.md)
6. [Private Types](patterns/private-types.md)

## Error Handling that Scales

1. [PolyVariant Error Propagation](patterns/polyvariant-error-propagation.md)
2. [Rresult's Guide to Custom Error
   Types](http://erratique.ch/software/rresult/doc/Rresult.html#usage)

## Examples

1. [smart-constructors](patterns/smart-constructors/SmartConstructors.re)
2. [constructor-views](patterns/constructor-view/ConstructorViewFinal.re)
3. [boolean-blindness](patterns/boolean-blindness/BooleanBlindness.re)
4. [providing-evidence](patterns/providing-evidence/ProvidingEvidence.re)
5. [interface-first](patterns/pretend-modules/InterfaceFirst.re)
6. [pretend-modules](patterns/pretend-modules/PretendModules.re)
7. [interface-first](patterns/pretend-modules/InterfaceFirst.re)
8. [one-module-one-thing](patterns/one-module-one-thing/OneModuleOneThing.re)
9. [100-named-args-functions](patterns/100-named-args-functions/HundredNamedArgsFunctions.re)
10. [private-types](patterns/private-types/PrivateTypes.re)
11. [interpreter-pattern](patterns/interpreter/FreeMonadInterpreter.re)

To run examples:

```sh
npm install
npm start
```
