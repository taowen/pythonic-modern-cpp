const fs = require("fs");
module.exports = {
    "gitbook": "3.x.x",
    "title": "gitbook-plugin-include-codeblock example",
    "plugins": [
        "theme-faq",
        "-fontsettngs",
        "-sharing",
        "include-codeblock"
    ],
    "pluginsConfig": {
        "include-codeblock": {
            // Before, create user-template.hbs
            "template": fs.readFileSync(__dirname + "/user-template.hbs", "utf-8")
        }
    }
};