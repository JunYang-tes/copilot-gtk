const gui = require("../../build/Release/gui.node");
const keybinder = require("node-keybinder")

const TickTock = {
  tick() {
    this.cancel()
    this.timer = setInterval(() => { }, 100)
  },
  cancel() {
    clearInterval(this.timer)
  }
}
TickTock.tick()

keybinder.on("<Alt>S", () => {
  console.log("Ctrl+S gui.show")
  TickTock.tick()
  gui.show()
})
keybinder.on("<Ctrl>H", () => {
  console.log("Ctrl+H gui.hide")
  TickTock.cancel()
  gui.hide()
})
// keybinder.start();
console.log("keybind")
const { startUp, handle, run } = require("copilot-core")
startUp()
  .then(() => {
    let result;
    gui.gui(async (e) => {
      console.log("@ Main callback")
      if (e.type === "text-change") {
        if (e.event.text == "exit") {
          process.exit(0);
        } else {
          let data = e.event
          if (data.text === "quit") {
            process.abort()
          } else {
            try {
              let ret = await handle(data.text)
              result = ret
              gui.display({
                type: "list",
                data: ret.slice(0, 10)
              })
            } catch (e) {
              console.warn(e)
            }
          }

        }
      } else if (e.type === "quit") {
        console.log("GUI exited.")
      } else if (e.type === "key-release" && e.event.keyValue === "\r" && result) {
        run(result[0])
      }
    });
  })

