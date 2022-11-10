import { addTimer, removeTimer, TIMER_DISCARD, TIMER_REARM } from '3ds/event-loop';
import { addNext } from './event-loop';

globalThis.setTimeout = (callback, delay, ...args) =>
  addTimer(delay, () => {
    callback(...args);
    return TIMER_DISCARD;
  });

globalThis.clearTimeout = (id) =>
  removeTimer(id);

globalThis.setInterval = (callback, delay, ...args) =>
  addTimer(delay, () => {
    callback(...args);
    return TIMER_REARM;
  });

globalThis.clearInterval = (id) =>
  removeTimer(id);

globalThis.setImmediate = (callback, ...args) =>
  addNext(() => callback(...args));
