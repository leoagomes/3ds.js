import json from '@rollup/plugin-json';
import { nodeResolve } from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';
import { babel } from '@rollup/plugin-babel';

export default {
  input: './index.js',
  output: {
    file: '../romfs/index.js',
    format: 'iife',
  },
  plugins: [
    commonjs(),
    babel({ babelHelpers: 'bundled' }),
    json(),
    nodeResolve(),
  ],
};
