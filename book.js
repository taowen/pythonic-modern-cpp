const fs = require("fs");
module.exports = {
    "gitbook": "3.x.x",
    "title": "gitbook-plugin-include-codeblock example",
    "plugins": [
        "theme-faq",
        "-fontsettngs",
        "-sharing",
        "include-codeblock",
        "disqus"
    ],
    "pluginsConfig": {
        "include-codeblock": {
            "template": fs.readFileSync(__dirname + "/user-template.hbs", "utf-8")
        },
        "disqus": {
            "shortName": "pythonic-modern-cpp"
        }
    }
};