import * as EL from './event_loop';

declare namespace _nativeModuleLoaders {
  function event_loop(): typeof EL;
}
