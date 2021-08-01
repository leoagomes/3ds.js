declare module "3ds/fs/sync" {
  export interface FileHandle {
    /**
     * Closes the current file handle.
     * 
     * Any subsequent calls on this handle (aside from isClosed() / isOpen()) may fail.
     * 
     * @returns wether the operation was successful.
     */
    close(): boolean;

    /**
     * Flushes the content of the underlying file handle.
     * 
     * @returns wether the operation was successful.
     */
    flush(): boolean;

    /**
     * Returns wether the file handled should be considered closed.
     */
    isClosed(): boolean;

    /**
     * Returns wether the file handled should be considered open.
     */
    isOpen(): boolean;
  }

  /**
   * Opens a new file handle.
   * 
   * @param filename The path for the file to open
   * @param mode The mode in which to open the file (as in cstdio fopen)
   * @returns A new file handle object for the file.
   * @throws a file error if the underlying fopen fails.
   */
  export function fopen(filename: string, mode: string): FileHandle;
}

declare module "3ds/event-loop" {
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
}
