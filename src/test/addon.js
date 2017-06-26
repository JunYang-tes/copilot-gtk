const gui = require("../../build/Release/gui.node");
const keybinder = require("node-keybinder")
keybinder.on("<Ctrl>S", () => {
  console.log("Ctrl+S gui.show")
  gui.show();
})
keybinder.on("<Ctrl>H", () => {
  console.log("Ctrl+H gui.hide")
  gui.hide();
})
// keybinder.start();
console.log("keybind")

setInterval(() => console.log("tick"), 1000);
gui.gui(async (e) => {
  console.log("@ Main callback")
  console.log(e);
  if (e.type === "text-change") {
    if (e.event.text == "exit") {
      process.exit(0);
    } else {
      let cnt = ~~(Math.random() * 10) + 3;
      let data = []
      await new Promise(res => setTimeout(res, 100))
      while (cnt--) {
        data.push(
          {
            title: e.event.text + cnt,
            icon: "./assets/icon.png",
            text: "<span foreground=\"red\"><b>I</b> am red</span><span foreground=\"green\"> I am green</span>"
          }
        )
      }
      gui.display({
        type: "list",
        data
      })
    }
  } else if (e.type === "quit") {
    console.log("GUI exited.")
  }
});