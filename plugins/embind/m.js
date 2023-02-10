var coap_te = require("./build/emcoap-te.js");

async function run() {
  const instance = await coap_te();
  console.log(instance.code_class(instance.code.bad_request));
}

try {
  run();
} catch(e) {
  console.error("Error loading module", e);
}