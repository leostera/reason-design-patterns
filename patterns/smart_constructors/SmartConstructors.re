module Email: {
  type t;

  let make: string => option(t);
} = {
  type t = string;

  let make = email => Some(email);
};
// Error: This expression has type string but an expression was expected of type
//  Email.t
// let me : Email.t = "reasonsthlm"; /* compile-time error! */
let me: option(Email.t) = Email.make("reasonsthlm");
let me2 = Email.make("reasonsthlm_me2");
// let me2:Email.t = Email.make("reasonsthlm_me2");
/** runs but... */
Js.log2("ME:None?", me);
Js.log2("ME2:None?", me2);

// let is_email_valid = title => Js.String.length(title) > 3;
module EmailValid: {
  type t = string;
  let is_valid: t => bool;
} = {
  type t = string;
  let is_email_valid = title => Js.String.length(title) > 3;
  let is_valid = email => is_email_valid(email);
};

let me: EmailValid.t = "reasonsthlm";

/** not a valid email address!
  * must check manually before usage
  */
let isFalse = me |> EmailValid.is_valid == false;

Js.log2("ME:EmailValid?", me);
Js.log2("ME:isFalse?", isFalse);

module EmailToString: {
  type t;

  let make: string => option(t);
  let to_string: t => string;
} = {
  type t = string;
  let make = email => Some(email);
  let to_string: t => string = email => email;
};

let me: option(EmailToString.t) = EmailToString.make("leandro@reasonsthlm.se");
let me2 = EmailToString.make("leandro@reasonsthlm.se");

switch (me) {
| Some(email) => EmailToString.to_string(email) == "leandro@reasonsthlm.se"
| None => false
};
switch (me2) {
| Some(email) => EmailToString.to_string(email) == "leandro@reasonsthlm.se"
| None => false
};