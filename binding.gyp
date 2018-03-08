{
	"targets": [
		{
			"target_name": "bindings",
			"sources": [
				"src/node_lkl.cc",
				"src/disk.cc",
				"src/async.cc",
				"src/worker.cc",
				"src/bindings.cc"
			],
			'actions': [
				{
					'action_name': "lkl_make",
					'inputs': [
						"vendor/linux/**"
					],
					'outputs': [
						"vendor/linux/tools/lkl/liblkl.a"
					],
					'action': [ 'make', '-j4', '-C', 'vendor/linux/tools/lkl', 'static' ],
					'message': 'Compiling LKL kernel..'
				}
			],
			"cflags_cc": [ '-fpermissive', '-Wno-pointer-arith' ],
			"defines": [ "CONFIG_AUTO_LKL_POSIX_HOST" ],
			"libraries": [
				"../vendor/linux/tools/lkl/liblkl.a"
			],
			"include_dirs": [
				"<!(node -e \"require('nan')\")",
				"vendor/linux/tools/lkl/include"
			]
		}
	]
}
