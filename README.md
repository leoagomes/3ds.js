# 3DS.js

A (very early stage) (Homebrew) JavaScript runtime for the Nintendo 3DS.

## Getting Started

- [set up a 3DS build environment](https://3dbrew.org/wiki/Setting_up_Development_Environment). (You'll also need [Node](https://nodejs.org/en/) and [Yarn](https://yarnpkg.com/).)
- clone this repository (`git clone https://github.com/leoagomes/3ds.js.git && cd 3ds.js`)
- treat the `/js` subdirectory as your normal JavaScript aplication ([Code]() works great!)
- run `make`
- use `3ds.js.3dsx` however you like ([emulator](https://citra-emu.org/) or [hacked 3ds](https://3ds.hacks.guide/))

If you're fine with writing only ES5.1 code where the `require` system is really
unstable (though nothing is _really_ stable at the moment), you can bypass JS
bundling and just write directly to an `index.js` in `romfs`.

## Motivation

My ultimate goal for this project is to have something compatible enough with
other JS runtimes as to be able to run both React (probably closer to React
Native) and Pixi.js on some Homebrew devices -- initially the 3DS, with Switch
support planned for the future (when I get a switch I can hack).

Since the project is in its infancy, it is really limited at the moment.
In order to start _somewhere_, I'm first implementing a couple non-standard
interfaces for things like IO and ctrulib access so maybe we can bootstrap a
community around this project.

At the moment, I don't expect anyone to be able to write games using JavaScript
on the 3DS. I'm unsure about performance and there isn't really a way to draw
graphics to the screen as there are no JS bindings to `citro3d` or `citro2d` --
though that is planned! I think this will currently work more as a more friendly
way to write text-based tools for the 3DS, without having to use C (at least
directly).

## Features

Currently, the only api that is stable is whatever [Duktape](https://duktape.org)
provides out of the box plus `console.log`. Everything else is really subject to
change.

If you write your app using the template provided in `/js`, you'll get the latest
JavaScript with support for many features through [core-js](https://github.com/zloirock/core-js)
polyfills. I'm not completely certain how well they will work.

### Roadmap

At the moment, I believe these will be the next steps towards the first release:

- node-like module system
  - current `require` (poorly) emulates node behavior
- proper file I/O -- even if completely synchronous
- promise polyfill
- a version of 3DS.js that is able to run `index.js` from the SD card (no compilations)
  - a JS app loader app capable of launching other JS packages?

After that, I intend on working on the following (not necessarily in that order):

- Duktape debugger over wifi
- `ctrulib` JS bindings
- `citro3d` JS bindings
- `citro2d` JS bindings
- async I/O
- `fetch` -- big goals!
- maybe something like `curl` JS bindings
- something akin to a React Native stack

### Philosophy

As mentioned above, at this stage I'm more interested in having something that
(barely) works and improving it from there. This means that this project is
currently missing a lot of optimizations. We're probably wasting memory and
leaving performance on the table -- which I agree may be problematic on a device
as limited as the (original) 3DS.

There may also be security issues with this code... as always, only run code you
trust on your devices.

## License

Aside from "external" libraries (such as duktape), which are released under
their own licenses, all code in this repository is released under an MIT-like
license that you should definitely check out at LICENSE.

## Contributing

Please do! Your contribution really does help. From documentation mistakes to
test improvements, to killer JS bundler templates or great polyfill, every piece
of code helps!

I currently prioritize code that's easy to read and maintain rather than too
cleverly optimized code, though that will eventually be necessary for
performance. Start a discussion and help improve the project!

If you're going to contribute, just don't be an asshole. You know it when you're
being one, everyone else probably does too. There really is no need to be one.
