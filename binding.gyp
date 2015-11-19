{
	"targets": [
		{
			"target_name": "bindings",
			"sources": [
				"src/bindings.cc",
				"src/node_lkl.cc"
			],
			"libraries": [
				"/home/petrosagg/projects/linux/tools/lkl/lib/liblkl.so"
			],
			"include_dirs": [
				"<!(node -e \"require('nan')\")",
				"/home/petrosagg/projects/linux/tools/lkl/include"
			]
		}
	]
}
