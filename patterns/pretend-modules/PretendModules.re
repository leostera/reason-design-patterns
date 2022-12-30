/**
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

 */
type car_status =
  | New
  | Used(int)
  | Sold;

type car = {
  name: string,
  model: string,
  status: car_status,
};

type appointment = {
  date: int,
  /** unix timestamp */
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

let schedule = [
  (
    Mon,
    [
      {date: 1547475544, car: car_volvo},
      {date: 1547476154, car: car_volvo},
    ],
  ),
];
