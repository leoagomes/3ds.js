import json from '@rollup/plugin-json';
import { nodeResolve } from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';
import { babel } from '@rollup/plugin-babel';

const extensions = ['.js', '.jsx', '.ts', '.tsx'];

export default {
  input: './index.js',
  external: ['3ds/event-loop'],
  output: {
    file: '../romfs/index.js',
    format: 'cjs',
    globals: {},
  },
  plugins: [
    nodeResolve({ extensions }),
    commonjs(),
    babel({ extensions, babelHelpers: 'bundled', include: ['**/*'] }),
    json(),
  ],
};
