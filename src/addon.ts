import { IResult } from "copilot-core/lib/types"
const gui = require("../build/Release/gui.node")
export interface IEvent {
  type: string,
  event: any
}
export type EventCallback = (event: IEvent) => void
export const GUI = {
  display(data: IResult[]) {
    gui.display({
      type: "list",
      data
    })
  },
  start(cb: EventCallback) {
    gui.gui(cb)
  }
}
const keybinder = require("node-keybinder")
keybinder.on("<Alt>S", () => {
  console.log("Ctrl+S gui.show")
  gui.show();
})
// keybinder.on("<Alt><Space>", () => {
//   console.log("Ctrl+H gui.hide")
//   gui.hide();
// })