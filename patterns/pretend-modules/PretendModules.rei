
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