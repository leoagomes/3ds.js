
export const TIMER_DISCARD: 0;
export const TIMER_REARM: 1;

export type TimerOp = typeof TIMER_DISCARD | typeof TIMER_REARM;
export type TimerId = string;

/**
 * Adds a timer to the event loop.
 * 
 * @example Adding a one-shot timer.
 * ```ts
 * el.addTimer(5000, function() {
 *   console.log('one-shot timer!');
 *   return el.TIMER_DISCARD;
 * })
 * ```
 * 
 * @example Adding a recurring timer.
 * ```ts
 * el.addTimer(5000, function() {
 *   console.log('recurring timer!');
 *   return el.TIMER_REARM;
 * })
 * ```
 * 
 * @param interval The number of milliseconds to wait before activating the
 *                 timer.
 * @param callback The function to call on activation.
 * @returns An object that represents that specific timer and can be used with
 *          {@link removeTimer}.
 */
export function addTimer(interval: number, callback: () => TimerOp): TimerId;

/**
 * Removes a timer from the event loop.
 * 
 * @example Removing a previously-added timer.
 * ```ts
 * var id = el.addTimer(5000, function() { return el.TIMER_REARM; });
 * el.removeTimer(id);
 * ```
 * 
 * @param id A reference to the timer to be removed.
 * @returns True if the given timer was removed from the event loop. False if a
 *          timer with the given id was not found.
 */
export function removeTimer(id: TimerId): boolean;
