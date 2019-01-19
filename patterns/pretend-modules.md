# "Pretend" Modules

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

