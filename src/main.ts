import { GUI } from "./addon"
import { IResult } from "copilot-core/lib/types"
import { startUp, handle, run } from "copilot-core"
const { debug, error } = require("b-logger")("gtk.main")
debug("Starting")

// const sh = {
//   cmds: [],
//   push(input: string) {
//     this.cmds.push(input)
//   }
// }
setInterval(() => {
  // debug("tick")
}, 100)


let result: IResult[]
startUp()
  .then(() => {
    GUI.start(async (event) => {
      if (event.type === "text-change") {
        let data: any = event.event
        if (data.text === "quit") {
          process.abort()
        } else {

          // console.log("OK")
          debug("Handle ", data.text)
          try {
            let ret = await handle(data.text)
            debug("Result:", ret)
            result = ret
            GUI.display(ret.slice(0, 10))
          } catch (e) {
            console.warn(e)
          }
        }
      } else if (event.type === "key-release") {
        if (event.event.keyValue === "\r") {
          debug("exec")
          if (result && result.length) {
            run(result[0])
          }
        }
      }
      debug("@event callback end")
    })
  })
  .catch(e => {
    error(`Failed to start up.`, e)
  })