const path = require('path');

// $ webpack --config webpack.config.js

module.exports = {
	mode: "development",
	entry: "./src/main.ts",
	output: {
    path: path.resolve('./public/js'),
    filename: 'main.js'
  },
	resolve: {
		modules: [path.resolve('./src'), path.resolve('./node_modules')],
		extensions: ['.ts', '.js'],
		alias: {
			'@': path.resolve('./')
		}
	},
	watch: true,
	module: {
		rules: [
			{
        test: /\.ts$/,
        use: {
					loader: "ts-loader",
					options: {
						allowTsInNodeModules: true
					},
				},
      }
		]
	}
}