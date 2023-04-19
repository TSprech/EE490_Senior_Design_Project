// 2023/04/19 12:40:32

import {Dispatch, SetStateAction, useState} from "react";

// Thanks: https://dev.to/cedricagoliki/better-reacts-usestate-hook-cg4
export default class StateObj<S> {
  constructor(value: S) {
    [this.state, this.setState] = useState(value);
  }

  get value(): S {
    return this.state;
  }

  set value(v: S) {
    this.setState(v)
  }

  state: S;
  setState: Dispatch<SetStateAction<S>>;
}

// export default function useStateObj (value: any) {
// export const useStateObj = (value) => {
//     const [state, setState] = useState(value)
//     return {
//       get value() {
//         return state
//       },
//       set value(v) {
//         setState(v)
//       }
//     }
//   }

