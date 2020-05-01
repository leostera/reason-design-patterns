
let can_request = true;
let expect_deleted_files = false;
  Js.log2("can_request",can_request);
if (can_request && expect_deleted_files) {
  Js.log2("do something...",_)|>ignore;
};

// doesnt do anything if doesnt eveluate to true...blindness
if (can_request && expect_deleted_files) {
  Js.log2("can_request",can_request);
  Js.log2("expect_deleted_files",expect_deleted_files);
  Js.log2("if...",_)|>ignore;
};

let request_status = `Can_request;
let expectation = `Files_were_deleted;

switch (request_status, expectation) {
| (`Can_request, `Files_were_deleted) => Js.log("do something...");
};