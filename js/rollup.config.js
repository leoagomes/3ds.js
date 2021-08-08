import json from '@rollup/plugin-json'

export default {
  input: 'src/index.js',
  output: {
    file: '../romfs/index.js',
    format: 'cjs',
  },
  plugins: [json()],
};
