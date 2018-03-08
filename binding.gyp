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
						"src/linux/**"
					],
					'outputs': [
						"src/linux/tools/lkl/liblkl.a"
					],
					'action': [ 'make', 'KALLSYMS_EXTRA_PASS=1', '-j4', '-C', 'src/linux/tools/lkl', 'static' ],
					'message': 'Compiling LKL kernel..'
				}
			],
			"cflags_cc": [ '-fpermissive', '-Wno-pointer-arith' ],
			"defines": [ "CONFIG_AUTO_LKL_POSIX_HOST" ],
			"libraries": [
				"../src/linux/tools/lkl/liblkl.a"
			],
			"include_dirs": [
				"<!(node -e \"require('nan')\")",
				"src/linux/tools/lkl/include"
			]
		}
	]
}
