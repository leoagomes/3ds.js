import { setOnTask } from '3ds/event-loop';

type Task = {
  (): void;
  next: Task | null;
  previous: Task | null;
};

class TaskQueue {
  private firstTask: Task | null = null;

  public add(callback: () => void) {
    const newTask: Task = callback as Task;
    newTask.next = newTask.previous = newTask;

    if (this.firstTask === null) {
      this.firstTask = newTask;
      return
    }

    const { firstTask } = this;
    const lastTask = firstTask.previous;

    lastTask!.next = firstTask.previous = newTask;
    newTask.next = firstTask;
    newTask.previous = lastTask;
  }

  public run() {
    let task = this.firstTask;
    if (task === null) return;

    do {
      try {
        task();
      } catch (e) {
        console.log('error running task');
        console.log(e);
      }
      task = task.next;
    } while (task !== this.firstTask && task !== null);
  }

  public isEmpty() {
    return this.firstTask === null;
  }
};

let currentQueue = new TaskQueue();
let nextQueue = new TaskQueue();

export const addThis = (callback: () => void) => 
  currentQueue.add(callback);

export const addNext = (callback: () => void) =>
  nextQueue.add(callback);

setOnTask(() => {
  if (currentQueue.isEmpty() && nextQueue.isEmpty())  return;

  // console.log('onTask');
  currentQueue.run();
  currentQueue = nextQueue;
  nextQueue = new TaskQueue();
});
